/*
    SPDX-FileCopyrightText: 2024 Elysia <c.elysia@foxmail.com>
    SPDX-FileCopyrightText: 2019 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "startlingmo.hpp"
#include "config-startlingmo.h"

#include <QCoreApplication>
#include <QDBusConnectionInterface>
#include <QDir>
#include <QLoggingCategory>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>
#include <UpdateLaunchEnvironmentJob>

extern QTextStream out;
QTextStream out(stderr);

Q_LOGGING_CATEGORY(LINGMO_STARTUP, "org.lingmo.startup")

void sigtermHandler(int signalNumber) {
  Q_UNUSED(signalNumber)
  if (QCoreApplication::instance()) {
    QCoreApplication::instance()->exit(-1);
  }
}

QStringList allServices(const QLatin1String &prefix) {
  const QStringList services =
      QDBusConnection::sessionBus().interface()->registeredServiceNames();
  QStringList names;

  std::copy_if(services.cbegin(), services.cend(), std::back_inserter(names),
               [&prefix](const QString &serviceName) {
                 return serviceName.startsWith(prefix);
               });

  return names;
}

void gentleTermination(QProcess *p) {
  if (p->state() != QProcess::Running) {
    return;
  }

  p->terminate();

  // Wait longer for a session than a greeter
  if (!p->waitForFinished(5000)) {
    p->kill();
    if (!p->waitForFinished(5000)) {
      qCWarning(LINGMO_STARTUP)
          << "Could not fully finish the process" << p->program();
    }
  }
}

int runSync(const QString &program, const QStringList &args,
            const QStringList &env) {
  QProcess p;
  if (!env.isEmpty())
    p.setEnvironment(QProcess::systemEnvironment() << env);
  p.setProcessChannelMode(QProcess::ForwardedChannels);
  p.start(program, args);

  QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                   &p, [&p] { gentleTermination(&p); });
  //     qCDebug(LINGMO_STARTUP) << "started..." << program << args;
  p.waitForFinished(-1);
  if (p.exitCode()) {
    qCWarning(LINGMO_STARTUP)
        << program << args << "exited with code" << p.exitCode();
  }
  return p.exitCode();
}

void createConfigDirectory() {
  const QString configDir =
      QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
  if (!QDir().mkpath(configDir))
    out << "Could not create config directory XDG_CONFIG_HOME: " << configDir
        << '\n';
}

void setupCursor(bool wayland) {
#ifdef XCURSOR_PATH
  QByteArray path(XCURSOR_PATH);
  path.replace("$XCURSOR_PATH", qgetenv("XCURSOR_PATH"));
  qputenv("XCURSOR_PATH", path);
#endif

  // TODO: consider linking directly
  if (!wayland) {
    QSettings settings(QSettings::UserScope, "lingmoos", "theme");
    qreal scaleFactor = settings.value("PixelRatio", 1.0).toReal();
    QString cursorTheme = settings.value("CursorTheme", "default").toString();
    int cursorSize = settings.value("CursorSize", 24).toInt() * scaleFactor;
    runSync("cupdatecursor", {cursorTheme, QString::number(cursorSize)});
  }
}

// Source scripts found in <config locations>/lingmo-workspace/env/*.sh
// (where <config locations> correspond to the system and user's configuration
// directory.
//
// Scripts are sourced in reverse order of priority of their directory, as
// defined by `QStandardPaths::standardLocations`. This ensures that
// high-priority scripts (such as those in the user's home directory) are
// sourced last and take precedence over lower-priority scripts (such as system
// defaults). Scripts in the same directory are sourced in lexical order of
// their filename.
//
// This is where you can define environment variables that will be available to
// all KDE programs, so this is where you can run agents using e.g. eval
// `ssh-agent` or eval `gpg-agent --daemon`. Note: if you do that, you should
// also put "ssh-agent -k" as a shutdown script
//
// (see end of this file).
// For anything else (that doesn't set env vars, or that needs a window
// manager), better use the Autostart folder.
void runEnvironmentScripts() {
  QStringList scripts;
  auto locations =
      QStandardPaths::standardLocations(QStandardPaths::GenericConfigLocation);

  //`standardLocations()` returns locations sorted by "order of priority". We
  // iterate in reverse
  // order so that high-priority scripts are sourced last and their
  // modifications take precedence.
  for (auto loc = locations.crbegin(); loc != locations.crend(); loc++) {
    QDir dir(*loc);
    if (!dir.cd(QStringLiteral("./lingmo-workspace/env"))) {
      // Skip location if lingmo-workspace/env subdirectory does not exist
      continue;
    }
    const auto dirScripts =
        dir.entryInfoList({QStringLiteral("*.sh")}, QDir::Files, QDir::Name);
    for (const auto &script : dirScripts) {
      scripts << script.absoluteFilePath();
    }
  }
  sourceFiles(scripts);
}

void sourceFiles(const QStringList &files) {
  QStringList filteredFiles;
  std::copy_if(files.begin(), files.end(), std::back_inserter(filteredFiles),
               [](const QString &i) { return QFileInfo(i).isReadable(); });

  if (filteredFiles.isEmpty())
    return;

  filteredFiles.prepend(
      QStringLiteral(CMAKE_INSTALL_FULL_LIBEXECDIR "/lingmo-sourceenv.sh"));

  QProcess p;
  p.start(QStringLiteral("/bin/sh"), filteredFiles);
  p.waitForFinished(-1);

  const auto fullEnv = p.readAllStandardOutput();
  auto envs = fullEnv.split('\0');

  for (auto &env : envs) {
    const int idx = env.indexOf('=');
    if (Q_UNLIKELY(idx <= 0)) {
      continue;
    }

    const auto name = env.left(idx);
    if (isShellVariable(name)) {
      continue;
    }
    setEnvironmentVariable(name, env.mid(idx + 1));
  }
}

bool isShellVariable(const QByteArray &name) {
  return name == "_" || name == "SHELL" || name.startsWith("SHLVL");
}

void setEnvironmentVariable(const QByteArray &name, const QByteArray &value) {
  if (qgetenv(name) != value) {
    qputenv(name, value);
  }
}

bool isSessionVariable(const QByteArray &name) {
  // Check is variable is specific to session.
  return name == "DISPLAY" || name == "XAUTHORITY" ||             //
         name == "WAYLAND_DISPLAY" || name == "WAYLAND_SOCKET" || //
         name.startsWith("XDG_");
}

void setupLingmoEnvironment() {
  // Manually disable auto scaling because we are scaling above
  // otherwise apps that manually opt in for high DPI get auto scaled by the
  // developer AND manually scaled by us
  qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "0");

  // Set defaults
  if (qEnvironmentVariableIsEmpty("XDG_DATA_HOME"))
    qputenv("XDG_DATA_HOME",
            QDir::home()
                .absoluteFilePath(QStringLiteral(".local/share"))
                .toLocal8Bit());
  if (qEnvironmentVariableIsEmpty("XDG_DESKTOP_DIR"))
    qputenv("XDG_DESKTOP_DIR", QDir::home()
                                   .absoluteFilePath(QStringLiteral("/Desktop"))
                                   .toLocal8Bit());
  if (qEnvironmentVariableIsEmpty("XDG_CONFIG_HOME"))
    qputenv(
        "XDG_CONFIG_HOME",
        QDir::home().absoluteFilePath(QStringLiteral(".config")).toLocal8Bit());
  if (qEnvironmentVariableIsEmpty("XDG_CACHE_HOME"))
    qputenv(
        "XDG_CACHE_HOME",
        QDir::home().absoluteFilePath(QStringLiteral(".cache")).toLocal8Bit());
  if (qEnvironmentVariableIsEmpty("XDG_DATA_DIRS"))
    qputenv("XDG_DATA_DIRS", "/usr/local/share/:/usr/share/");
  if (qEnvironmentVariableIsEmpty("XDG_CONFIG_DIRS"))
    qputenv("XDG_CONFIG_DIRS", "/etc/xdg");

  // Environment
  qputenv("DESKTOP_SESSION", "Lingmo");
  qputenv("XDG_CURRENT_DESKTOP", "Lingmo");
  qputenv("XDG_SESSION_DESKTOP", "Lingmo");

  // Qt
  // qputenv("QT_QPA_PLATFORMTHEME", "lingmo");
  // qputenv("QT_PLATFORM_PLUGIN", "lingmo");

  // ref:
  // https://stackoverflow.com/questions/34399993/qml-performance-issue-when-updating-an-item-in-presence-of-many-non-overlapping
  qputenv("QT_QPA_UPDATE_IDLE_TIME", "10");

  qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "0");

  // IM Config
  // qputenv("GTK_IM_MODULE", "fcitx5");
  // qputenv("QT4_IM_MODULE", "fcitx5");
  // qputenv("QT_IM_MODULE", "fcitx5");
  // qputenv("CLUTTER_IM_MODULE", "fcitx5");
  // qputenv("XMODIFIERS", "@im=fcitx");
}

std::optional<QProcessEnvironment> getSystemdEnvironment() {
  auto msg = QDBusMessage::createMethodCall(
      QStringLiteral("org.freedesktop.systemd1"),
      QStringLiteral("/org/freedesktop/systemd1"),
      QStringLiteral("org.freedesktop.DBus.Properties"), QStringLiteral("Get"));
  msg << QStringLiteral("org.freedesktop.systemd1.Manager")
      << QStringLiteral("Environment");
  auto reply = QDBusConnection::sessionBus().call(msg);
  if (reply.type() == QDBusMessage::ErrorMessage) {
    return std::nullopt;
  }

  // Make sure the returned type is correct.
  auto arguments = reply.arguments();
  if (arguments.isEmpty() ||
      arguments[0].userType() != qMetaTypeId<QDBusVariant>()) {
    return std::nullopt;
  }
  auto variant = qdbus_cast<QVariant>(arguments[0]);
  if (variant.type() != QVariant::StringList) {
    return std::nullopt;
  }

  const auto assignmentList = variant.toStringList();
  QProcessEnvironment ret;
  for (auto &env : assignmentList) {
    const int idx = env.indexOf(QLatin1Char('='));
    if (Q_LIKELY(idx > 0)) {
      ret.insert(env.left(idx), env.mid(idx + 1));
    }
  }

  return ret;
}

// Drop session-specific variables from the systemd environment.
// Those can be leftovers from previous sessions, which can interfere with the
// session we want to start now, e.g. $DISPLAY might break kwin_wayland.
static void dropSessionVarsFromSystemdEnvironment() {
  const auto environment = getSystemdEnvironment();
  if (!environment) {
    return;
  }

  QStringList varsToDrop;
  for (auto &nameStr : environment.value().keys()) {
    // If it's set in this process, it'll be overwritten by the following
    // UpdateLaunchEnvJob
    const auto name = nameStr.toLocal8Bit();
    if (!qEnvironmentVariableIsSet(name) && isSessionVariable(name)) {
      varsToDrop.append(nameStr);
    }
  }

  auto msg = QDBusMessage::createMethodCall(
      QStringLiteral("org.freedesktop.systemd1"),
      QStringLiteral("/org/freedesktop/systemd1"),
      QStringLiteral("org.freedesktop.systemd1.Manager"),
      QStringLiteral("UnsetEnvironment"));
  msg << varsToDrop;
  auto reply = QDBusConnection::sessionBus().call(msg);
  if (reply.type() == QDBusMessage::ErrorMessage) {
    qCWarning(LINGMO_STARTUP)
        << "Failed to unset systemd environment variables:" << reply.errorName()
        << reply.errorMessage();
  }
}

// kwin_wayland can possibly also start dbus-activated services which need env
// variables. In that case, the update in startplasma might be too late.
bool syncDBusEnvironment() {
  dropSessionVarsFromSystemdEnvironment();

  // Shell variables are filtered out of things we explicitly load, but they
  // still might have been inherited from the parent process
  QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
  for (auto &name : environment.keys()) {
    if (isShellVariable(name.toLocal8Bit())) {
      environment.remove(name);
    }
  }

  // At this point all environment variables are set, let's send it to the DBus
  // session server to update the activation environment
  auto job = new UpdateLaunchEnvironmentJob(environment);
  QEventLoop e;
  QObject::connect(job, &UpdateLaunchEnvironmentJob::finished, &e,
                   &QEventLoop::quit);
  e.exec();
  return true;
}

void initLanguage() {
  QSettings settings(QSettings::UserScope, "lingmoos", "language");
  QString value = settings.value("language", "").toString();

  // Init Language
  if (value.isEmpty()) {
    QFile file("/etc/locale.gen");
    if (file.open(QIODevice::ReadOnly)) {
      QStringList lines = QString(file.readAll()).split('\n');

      for (const QString &line : lines) {
        if (line.startsWith('#'))
          continue;

        if (line.trimmed().isEmpty())
          continue;

        value = line.split(' ').first().split('.').first();
      }
    }
  }

  if (value.isEmpty())
    value = "en_US";

  settings.setValue("language", value);

  QString str = QString("%1.UTF-8").arg(value);

  const auto lcValues = {"LANG",        "LC_NUMERIC",     "LC_TIME",
                         "LC_MONETARY", "LC_MEASUREMENT", "LC_COLLATE",
                         "LC_CTYPE"};

  for (auto lc : lcValues) {
    const QString value = str;
    if (!value.isEmpty()) {
      qputenv(lc, value.toUtf8());
    }
  }

  if (!value.isEmpty()) {
    qputenv("LANGUAGE", value.toUtf8());
  }
}

// Import systemd user environment.
//
// Systemd read ~/.config/environment.d which applies to all systemd user unit.
// But it won't work if plasma is not started by systemd.
void importSystemdEnvrionment() {
  const auto environment = getSystemdEnvironment();
  if (!environment) {
    return;
  }

  for (auto &nameStr : environment.value().keys()) {
    const auto name = nameStr.toLocal8Bit();
    if (!isShellVariable(name) && !isSessionVariable(name)) {
      setEnvironmentVariable(name,
                             environment.value().value(nameStr).toLocal8Bit());
    }
  }
}

// If something went on an endless restart crash loop it will get blacklisted,
// as this is a clean login we will want to reset those counters This is
// independent of whether we use the Plasma systemd boot
void resetSystemdFailedUnits() {
  QDBusMessage message = QDBusMessage::createMethodCall(
      QStringLiteral("org.freedesktop.systemd1"),
      QStringLiteral("/org/freedesktop/systemd1"),
      QStringLiteral("org.freedesktop.systemd1.Manager"),
      QStringLiteral("ResetFailed"));
  QDBusConnection::sessionBus().call(message);
}

// Reload systemd to make sure the current configuration is active, which also
// reruns generators. Needed for e.g. XDG autostart changes to become effective.
void reloadSystemd() {
  QDBusMessage message = QDBusMessage::createMethodCall(
      QStringLiteral("org.freedesktop.systemd1"),
      QStringLiteral("/org/freedesktop/systemd1"),
      QStringLiteral("org.freedesktop.systemd1.Manager"),
      QStringLiteral("Reload"));
  QDBusConnection::sessionBus().call(message);
}

bool startLingmoSession(bool wayland) {
  resetSystemdFailedUnits();
  reloadSystemd();

  bool rc = true;
  QEventLoop e;

  std::unique_ptr<QProcess, KillBeforeDeleter> startLingmoSession;

  QStringList lingmoSessionOptions;

  if (wayland) {
    lingmoSessionOptions << QStringLiteral("--wayland");
  }

  {
    startLingmoSession.reset(new QProcess);
    qCDebug(LINGMO_STARTUP) << "Using classic boot";

    startLingmoSession->setProcessChannelMode(QProcess::ForwardedChannels);

    startLingmoSession->start(
        QStringLiteral(CMAKE_INSTALL_FULL_BINDIR "/lingmo_session"),
        lingmoSessionOptions);
  }

  if (rc) {
    QObject::connect(QCoreApplication::instance(),
                     &QCoreApplication::aboutToQuit, &e, &QEventLoop::quit);
    e.exec();
  }
  return rc;
}
