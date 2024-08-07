/*
 * Copyright (C) 2023-2024 LingmoOS Team.
 *
 * Author:     revenmartin <revenmartin@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <QAbstractNativeEventFilter>
#include <QApplication>
#include <QEventLoop>
#include <QMap>
#include <QObject>
#include <QProcess>
#include <memory>
#include <qobjectdefs.h>

#include "daemon-helper.h"

class Application;
class ProcessManager : public QObject, public QAbstractNativeEventFilter {
  Q_OBJECT

public:
  explicit ProcessManager(Application *app, QObject *parent = nullptr);
  ~ProcessManager();

  void start();
  void logout();

  void startWindowManager();
  void startDesktopProcess();
  void startDaemonProcess();

  /**
   * @brief Start the user defined autostart process.
   *        Typically, they are in <home>/.config/autostart/xxx.desktop
   */
  void loadAutoStartProcess();

  bool nativeEventFilter(const QByteArray &eventType, void *message,
                         long *result) override;

private:
  Application *m_app;
  QMap<QString, QProcess *> m_systemProcess;
  QMap<QString, QProcess *> m_autoStartProcess;

  // Daemon helper for desktop components
  std::shared_ptr<LINGMO_SESSION::Daemon> m_desktopAutoStartD;

  // Daemon helper for other daemon components
  std::shared_ptr<LINGMO_SESSION::Daemon> m_daemonAutoStartD;

  // Daemon helper for User Auto Start Process
  std::shared_ptr<LINGMO_SESSION::Daemon> m_userAutoStartD;

  bool m_wmStarted;
  QEventLoop *m_waitLoop;
};

using namespace LINGMO_SESSION;
/**
 * Launches a process, and waits for the process to start
 */
class StartProcessJob : public Job {
  Q_OBJECT
public:
  StartProcessJob(
      const QString &process, const QStringList &args,
      const QProcessEnvironment &additionalEnv = QProcessEnvironment());
  void start() override;

  void finished(int exitCode, QProcess::ExitStatus e);

private:
  QProcess *m_process;
};

/**
 * Launches a process, and waits for the service to appear on the session bus
 */
class StartServiceJob : public Job {
  Q_OBJECT
public:
  StartServiceJob(
      const QString &process, const QStringList &args, const QString &serviceId,
      const QProcessEnvironment &additionalEnv = QProcessEnvironment());

  void start() override;

  bool startDetached(QProcess *process);

private:
  QProcess *m_process;
  const QString m_serviceId;
  const QProcessEnvironment m_additionalEnv;
};
#endif // PROCESSMANAGER_H
