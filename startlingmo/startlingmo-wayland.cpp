/*
    SPDX-FileCopyrightText: 2024 Elysia <c.elysia@foxmail.com>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
#include <QCoreApplication>
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusMessage>

#include "startlingmo.hpp"

extern "C" {
#include "signal.h"
}

extern QTextStream out;

int main(int argc, char **argv) {

  QCoreApplication app(argc, argv);

  createConfigDirectory();
  setupCursor(true);
  signal(SIGTERM, sigtermHandler);

  // Let clients try to reconnect to kwin after a restart
  qputenv("QT_WAYLAND_RECONNECT", "1");

  // Query whether org.freedesktop.locale1 is available. If it is, try to
  // set XKB_DEFAULT_{MODEL,LAYOUT,VARIANT,OPTIONS} accordingly.
  {
    const QString locale1Service = QStringLiteral("org.freedesktop.locale1");
    const QString locale1Path = QStringLiteral("/org/freedesktop/locale1");
    QDBusMessage message = QDBusMessage::createMethodCall(
        locale1Service, locale1Path,
        QStringLiteral("org.freedesktop.DBus.Properties"),
        QStringLiteral("GetAll"));
    message << locale1Service;
    QDBusMessage resultMessage = QDBusConnection::systemBus().call(message);
    if (resultMessage.type() == QDBusMessage::ReplyMessage) {
      QVariantMap result;
      QDBusArgument dbusArgument =
          resultMessage.arguments().at(0).value<QDBusArgument>();
      while (!dbusArgument.atEnd()) {
        dbusArgument >> result;
      }

      auto queryAndSet = [&result](const char *var, const QString &value) {
        const auto r = result.value(value).toString();
        if (!r.isEmpty())
          qputenv(var, r.toUtf8());
      };

      queryAndSet("XKB_DEFAULT_MODEL", QStringLiteral("X11Model"));
      queryAndSet("XKB_DEFAULT_LAYOUT", QStringLiteral("X11Layout"));
      queryAndSet("XKB_DEFAULT_VARIANT", QStringLiteral("X11Variant"));
      queryAndSet("XKB_DEFAULT_OPTIONS", QStringLiteral("X11Options"));
    } else {
      qCWarning(LINGMO_STARTUP)
          << "No valid reply from org.freedesktop.locale1" << resultMessage;
    }
  }

  runEnvironmentScripts();

  if (!qEnvironmentVariableIsSet("DBUS_SESSION_BUS_ADDRESS")) {
    out << "startplasmacompositor: Could not start D-Bus. Can you call "
           "qdbus?\n";
    return 1;
  }

  setupLingmoEnvironment();
  initLanguage();

  qputenv("XDG_SESSION_TYPE", "wayland");

  auto oldSystemdEnvironment = getSystemdEnvironment();
  if (!syncDBusEnvironment()) {
    out << "Could not sync environment to dbus.\n";
    return 1;
  }

  // We import systemd environment after we sync the dbus environment here.
  // Otherwise it may leads to some unwanted order of applying environment
  // variables (e.g. LANG and LC_*)
  importSystemdEnvrionment();

  return 0;
}