/*
    SPDX-FileCopyrightText: 2024 Elysia <c.elysia@foxmail.com>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "signal.h"
#include "startlingmo.hpp"

#include <QCoreApplication>

#include <iostream>

void sighupHandler(int) { std::cout << "GOT SIGHUP\n"; }

int main(int argc, char *argv[]) {

  QCoreApplication app(argc, argv);

  // When the X server dies we get a HUP signal from xinit. We must ignore it
  // because we still need to do some cleanup.
  signal(SIGHUP, sighupHandler);

  qputenv("QT_NO_XDG_DESKTOP_PORTAL", QByteArrayLiteral("1"));

  // ToDo: Maybe we can check wether lingmo de is already running?

  createConfigDirectory();
  initLanguage();
  initScreenScaleFactors();

  // NOTE: Be very mindful of what you start this early in the process. The
  // environment is not yet complete.
  setupCursor(false);

  runEnvironmentScripts();

  std::cout << "Starting lingmo de ...\n";

  setupLingmoEnvironment();

  qunsetenv("QT_NO_XDG_DESKTOP_PORTAL");
  auto oldSystemdEnvironment = getSystemdEnvironment();
  if (!syncDBusEnvironment()) {
    // Startup error
    messageBox(QStringLiteral("Could not sync environment to dbus.\n"));
    return 1;
  }

  // We import systemd environment after we sync the dbus environment here.
  // Otherwise it may leads to some unwanted order of applying environment
  // variables (e.g. LANG and LC_*)
  importSystemdEnvrionment();

  if (!startLingmoSession(false))
    return 1;

  std::cout << "Shutting down lingmo de ...\n";

  cleanupPlasmaEnvironment(oldSystemdEnvironment);

  return 0;
}