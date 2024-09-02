/*
 * Copyright (C) 2023-2024 Lingmo OS Team.
 */

#include "processmanager.h"
#include "application.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QSettings>
#include <QStandardPaths>
#include <QThread>
#include <QTimer>

#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusPendingCall>
#include <QDBusServiceWatcher>

#include <KWindowSystem/NETWM>
#include <KWindowSystem>
#include <QX11Info>
#include <qprocess.h>

#include "daemon-helper.h"
#include "debug.h"

ProcessManager *s_self;

ProcessManager::ProcessManager(Application *app, QObject *parent)
    : QObject(parent), m_app(app), m_wmStarted(false), m_waitLoop(nullptr) {
  Q_ASSERT(!s_self);
  s_self = this;

  qApp->installNativeEventFilter(this);
}

ProcessManager::~ProcessManager() {
  qApp->removeNativeEventFilter(this);

  QMapIterator<QString, QProcess *> i(m_systemProcess);
  while (i.hasNext()) {
    i.next();
    QProcess *p = i.value();
    delete p;
    m_systemProcess[i.key()] = nullptr;
  }
}

void ProcessManager::updateLaunchEnv(const QString &key, const QString &value) {
  qputenv(key.toLatin1(), value.toLatin1());
}

void ProcessManager::start() {
  startWindowManager();
  startDaemonProcess();
}

void ProcessManager::logout() {
  QMapIterator<QString, QProcess *> i(m_systemProcess);

  while (i.hasNext()) {
    i.next();
    QProcess *p = i.value();
    p->terminate();
  }
  i.toFront();

  while (i.hasNext()) {
    i.next();
    QProcess *p = i.value();
    if (p->state() != QProcess::NotRunning && !p->waitForFinished(2000)) {
      p->kill();
    }
  }

  QCoreApplication::exit(0);
}

void ProcessManager::startWindowManager() {
  auto detcted_wayland =
      qEnvironmentVariable("XDG_SESSION_TYPE") == QLatin1String("wayland");

  if (detcted_wayland || m_app->wayland()) {
    auto kwinWaylandJob =
        new StartServiceJob(QStringLiteral("lingmo_kwin_wayland_wrapper"),
                            {QStringLiteral("--xwayland")},
                            QStringLiteral("org.lingmo.KWinWrapper"));
    kwinWaylandJob->setParent(this);
    kwinWaylandJob->exec(); // Wait untill kwin_wayland_wrapper started
  } else {
    auto *wmProcess = new QProcess;

    wmProcess->start("kwin_x11", QStringList());
  }
}

void ProcessManager::startDesktopProcess() {
  // When the lingmo-settings-daemon theme module is loaded, start the desktop.
  // In the way, there will be no problem that desktop and launcher can't get
  // wallpaper.

  auto xcb_extra = QProcessEnvironment();
  xcb_extra.insert("QT_QPA_PLATFORM", "xcb");
  const QVector<Job *> sequence = {
      new LaunchProcess(QStringLiteral("lingmo-notificationd"), {}, xcb_extra),
      new LaunchProcess(QStringLiteral("lingmo-statusbar"), {}, xcb_extra),
      new LaunchProcess(QStringLiteral("lingmo-dock"), {}, xcb_extra),
      new LaunchProcess(QStringLiteral("lingmo-filemanager"),
                        QStringList("--desktop"), xcb_extra),

      new LaunchProcess(QStringLiteral("lingmo-launcher"), {}, xcb_extra),
      new LaunchProcess(QStringLiteral("lingmo-powerman"), {}, xcb_extra),
      new LaunchProcess(QStringLiteral("lingmo-clipboard"), {}, xcb_extra),
      new LaunchProcess(QStringLiteral("lingmo-wallpaper-color-pick"), {},
                        xcb_extra),
  };
  Job *last = nullptr;
  for (Job *job : sequence) {
    if (!job) {
      continue;
    }
    if (last) {
      connect(last, &Job::finished, job, &Job::start);
    }
    last = job;
  }

  // connect(sequence.last(), &Job::finished, this, &Startup::finishStartup);
  sequence.first()->start();
}

void ProcessManager::startDaemonProcess() {
  auto xcb_extra = QProcessEnvironment();
  xcb_extra.insert("QT_QPA_PLATFORM", "xcb");
  const QVector<Job *> sequence = {
      new LaunchProcess(QStringLiteral("lingmo-settings-daemon"), {},
                        xcb_extra),
      new LaunchProcess(QStringLiteral("lingmo-xembedsniproxy"), {}, xcb_extra),
      new LaunchProcess(QStringLiteral("lingmo-gmenuproxy"), {}, xcb_extra),
      new LaunchProcess(QStringLiteral("lingmo-permission-surveillance"), {},
                        xcb_extra),
  };
  Job *last = nullptr;
  for (Job *job : sequence) {
    if (!job) {
      continue;
    }
    if (last) {
      connect(last, &Job::finished, job, &Job::start);
    }
    last = job;
  }

  connect(sequence.last(), &Job::finished, this, &ProcessManager::startDesktopProcess);
  sequence.first()->start();
}

void ProcessManager::loadAutoStartProcess() {
  QList<QPair<QString, QStringList>> list;

  const QStringList dirs = QStandardPaths::locateAll(
      QStandardPaths::GenericConfigLocation, QStringLiteral("autostart"),
      QStandardPaths::LocateDirectory);
  for (const QString &dir : dirs) {
    const QDir d(dir);
    const QStringList fileNames =
        d.entryList(QStringList() << QStringLiteral("*.desktop"));
    for (const QString &file : fileNames) {
      QSettings desktop(d.absoluteFilePath(file), QSettings::IniFormat);
      desktop.setIniCodec("UTF-8");
      desktop.beginGroup("Desktop Entry");

      // Ignore files the require a specific desktop environment
      if (desktop.contains("NotShowIn")) {
        const QStringList notShowIn = desktop.value("NotShowIn").toStringList();
        if (notShowIn.contains("Lingmo"))
          continue;
      }
      if (desktop.contains("OnlyShowIn")) {
        const QStringList onlyShowIn =
            desktop.value("OnlyShowIn").toStringList();
        if (!onlyShowIn.contains("Lingmo"))
          continue;
      }

      const QString execValue = desktop.value("Exec").toString();

      // 避免冲突
      if (execValue.contains("gmenudbusmenuproxy"))
        continue;

      // 使用 QProcess::splitCommand 来解析命令和参数
      QStringList args = QProcess::splitCommand(execValue);

      // 检查是否至少有一个元素（即程序路径）
      if (!args.isEmpty()) {
        auto program = args.first();
        args.removeFirst(); // 移除程序路径，剩下的都是参数

        list << qMakePair(program, args);
      } else {
        qCWarning(LINGMO_SESSION_D) << "Invalid 'Exec' found in file!";
      }
    }
  }

  m_userAutoStartD = std::make_shared<LINGMO_SESSION::Daemon>(list, false);
}

bool ProcessManager::nativeEventFilter(const QByteArray &eventType,
                                       void *message, long *result) {
  if (eventType != "xcb_generic_event_t") // We only want to handle XCB events
    return false;

  // ref: lxqt session
  if (!m_wmStarted && m_waitLoop) {
    // all window managers must set their name according to the spec
    if (!QString::fromUtf8(
             NETRootInfo(QX11Info::connection(), NET::SupportingWMCheck)
                 .wmName())
             .isEmpty()) {
      qDebug() << "Window manager started";
      m_wmStarted = true;
      if (m_waitLoop && m_waitLoop->isRunning())
        m_waitLoop->exit();

      qApp->removeNativeEventFilter(this);
    }
  }

  return false;
}

bool ProcessManager::startDetached(QProcess *process) {
  process->setProcessChannelMode(QProcess::ForwardedChannels);
  process->start();
  const bool ret = process->waitForStarted();
  if (ret) {
    m_processes << process;
  }
  return ret;
}

StartProcessJob::StartProcessJob(const QString &process,
                                 const QStringList &args,
                                 const QProcessEnvironment &additionalEnv)
    : Job(), m_process(new QProcess(this)) {
  m_process->setProgram(process);
  m_process->setArguments(args);
  m_process->setProcessChannelMode(QProcess::ForwardedChannels);
  auto env = QProcessEnvironment::systemEnvironment();
  env.insert(additionalEnv);
  m_process->setProcessEnvironment(env);

  connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this,
          SLOT(finished(int, QProcess::ExitStatus)));
}

void StartProcessJob::start() {
  qCDebug(LINGMO_SESSION_D)
      << "Starting " << m_process->program() << m_process->arguments();

  m_process->start();
}

void StartProcessJob::finished(int exitCode, QProcess::ExitStatus e) {
  qCDebug(LINGMO_SESSION_D) << "process job " << m_process->program()
                            << "finished with exit code " << exitCode;
  emitResult();
}

StartServiceJob::StartServiceJob(const QString &process,
                                 const QStringList &args,
                                 const QString &serviceId,
                                 const QProcessEnvironment &additionalEnv)
    : Job(), m_process(new QProcess), m_serviceId(serviceId),
      m_additionalEnv(additionalEnv) {
  m_process->setProgram(process);
  m_process->setArguments(args);

  auto watcher =
      new QDBusServiceWatcher(serviceId, QDBusConnection::sessionBus(),
                              QDBusServiceWatcher::WatchForRegistration, this);
  connect(watcher, &QDBusServiceWatcher::serviceRegistered, this,
          &StartServiceJob::emitResult);
}

void StartServiceJob::start() {
  auto env = QProcessEnvironment::systemEnvironment();
  env.insert(m_additionalEnv);
  m_process->setProcessEnvironment(env);

  if (!m_serviceId.isEmpty() &&
      QDBusConnection::sessionBus().interface()->isServiceRegistered(
          m_serviceId)) {
    qCDebug(LINGMO_SESSION_D) << m_process << "already running";
    emitResult();
    return;
  }
  qDebug() << "Starting " << m_process->program() << m_process->arguments();
  if (!ProcessManager::self()->startDetached(m_process)) {
    qCWarning(LINGMO_SESSION_D)
        << "error starting process" << m_process->program()
        << m_process->arguments();
    emitResult();
  }

  if (m_serviceId.isEmpty()) {
    emitResult();
  }
}

LaunchProcess::LaunchProcess(const QString &process, const QStringList &args,
                             const QProcessEnvironment &additionalEnv)
    : Job(), m_process(new QProcess(this)) {
  m_process->setProgram(process);
  m_process->setArguments(args);
  m_process->setProcessChannelMode(QProcess::ForwardedChannels);
  auto env = QProcessEnvironment::systemEnvironment();
  env.insert(additionalEnv);
  m_process->setProcessEnvironment(env);
}

void LaunchProcess::start() {
  qCDebug(LINGMO_SESSION_D)
      << "Starting " << m_process->program() << m_process->arguments();

  m_process->startDetached();

  emitResult();
}