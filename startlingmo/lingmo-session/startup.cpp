/*
    SPDX-FileCopyrightText: 2024 Elysia <c.elysia@foxmail.com>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "startup.hpp"
#include <qobjectdefs.h>

#include "debug.h"
#include "startupadaptor.h"
#include "sessiontrack.h"

#include <KJob>

void Startup::updateLaunchEnv(const QString &key, const QString &value) {
  qputenv(key.toLatin1(), value.toLatin1());
}

void Startup::init(QObject *parent) {
  Startup::getInstance()->setParent(parent);
}

Startup::Startup() : QObject(nullptr) {

  new StartupAdaptor(this);
  QDBusConnection::sessionBus().registerObject(
      QStringLiteral("/Startup"), QStringLiteral("org.lingmo.Startup"), this);
  QDBusConnection::sessionBus().registerService(
      QStringLiteral("org.lingmo.Startup"));

  if (qEnvironmentVariable("XDG_SESSION_TYPE") != QLatin1String("wayland")) {
    // Currently don't do anything if we're not running under Wayland
  } else {
    // This must block until started as it sets the WAYLAND_DISPLAY/DISPLAY env
    // variables needed for the rest of the boot fortunately it's very fast as
    // it's just starting a wrapper
    StartServiceJob kwinWaylandJob(QStringLiteral("kwin_wayland_wrapper"),
                                   {QStringLiteral("--xwayland")},
                                   QStringLiteral("org.kde.KWinWrapper"));
    kwinWaylandJob.exec();
  }

  const QVector<KJob *> sequence = {

  };
  KJob *last = nullptr;
  for (KJob *job : sequence) {
    if (!job) {
      continue;
    }
    if (last) {
      connect(last, &KJob::finished, job, &KJob::start);
    }
    last = job;
  }

  connect(sequence.last(), &KJob::finished, this, &Startup::finishStartup);
  sequence.first()->start();
}

void Startup::finishStartup() {
  qCDebug(LINGMO_SESSION) << "Finished";
  // upAndRunning(QStringLiteral("ready"));

  // playStartupSound();
  new SessionTrack(m_processes);
  deleteLater();
}

bool Startup::startDetached(QProcess *process) {
  process->setProcessChannelMode(QProcess::ForwardedChannels);
  process->start();
  const bool ret = process->waitForStarted();
  if (ret) {
    m_processes << process;
  }
  return ret;
}

StartServiceJob::StartServiceJob(const QString &process,
                                 const QStringList &args,
                                 const QString &serviceId,
                                 const QProcessEnvironment &additionalEnv)
    : KJob(), m_process(new QProcess), m_serviceId(serviceId),
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
    qCDebug(LINGMO_SESSION) << m_process << "already running";
    emitResult();
    return;
  }
  qCDebug(LINGMO_SESSION) << "Starting " << m_process->program()
                          << m_process->arguments();
  if (!Startup::getInstance()->startDetached(m_process)) {
    qCWarning(LINGMO_SESSION) << "error starting process"
                              << m_process->program() << m_process->arguments();
    emitResult();
  }

  if (m_serviceId.isEmpty()) {
    emitResult();
  }
}

StartProcessJob::StartProcessJob(const QString &process,
                                 const QStringList &args,
                                 const QProcessEnvironment &additionalEnv)
    : KJob(), m_process(new QProcess(this)) {
  m_process->setProgram(process);
  m_process->setArguments(args);
  m_process->setProcessChannelMode(QProcess::ForwardedChannels);
  auto env = QProcessEnvironment::systemEnvironment();
  env.insert(additionalEnv);
  m_process->setProcessEnvironment(env);

  connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished(int, QProcess::ExitStatus)));
}

void StartProcessJob::start() {
  qCDebug(LINGMO_SESSION) << "Starting " << m_process->program()
                          << m_process->arguments();

  m_process->start();
}

void StartProcessJob::finised(int exitCode, QProcess::ExitStatus) {
  qCInfo(LINGMO_SESSION) << "process job " << m_process->program()
                         << "finished with exit code " << exitCode;
  emitResult();
}
