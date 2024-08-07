/*
    SPDX-FileCopyrightText: 2024 Elysia <c.elysia@foxmail.com>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef STARTUP_HPP
#define STARTUP_HPP

#include <QObject>
#include <QProcess>

#include <KJob>

#include <memory>

class Startup : public QObject {
  Q_OBJECT

public:
  Startup(QObject *parent);
  void init(QObject *parent);

  bool startDetached(QProcess *process);

public Q_SLOTS:
  // alternatively we could drop this?
  void updateLaunchEnv(const QString &key, const QString &value);

  void finishStartup();

private:
  QVector<QProcess *> m_processes;
};

/**
 * Launches a process, and waits for the service to appear on the session bus
 */
class StartServiceJob : public KJob {
  Q_OBJECT
public:
  StartServiceJob(
      const QString &process, const QStringList &args, const QString &serviceId,
      const QProcessEnvironment &additionalEnv = QProcessEnvironment());
  void start() override;

private:
  QProcess *m_process;
  const QString m_serviceId;
  const QProcessEnvironment m_additionalEnv;
};

/**
 * Launches a process, and waits for the process to start
 */
class StartProcessJob : public KJob {
  Q_OBJECT
public:
  StartProcessJob(
      const QString &process, const QStringList &args,
      const QProcessEnvironment &additionalEnv = QProcessEnvironment());
  void start() override;

public Q_SLOTS:
  void finished(int, QProcess::ExitStatus);

private:
  QProcess *m_process;
};

extern std::shared_ptr<Startup> startup_ptr;

#endif // STARTUP_HPP