/**
 * @name daemon-helper.cpp
 * @author Elysia <c.elysia@foxmail.com>
 **/
#include "daemon-helper.h"
#include "job_private.hpp"

#include <QCoreApplication>
#include <QDebug>
#include <QPair>
#include <QPointer>
#include <QProcess>
#include <QString>
#include <QTimer>

namespace LINGMO_SESSION {
Daemon::Daemon(const QList<QPair<QString, QStringList>> &processList,
               bool _enableAutoStart, QObject *parent)
    : QObject(parent), m_processList(processList),
      m_enableAutoRestart(_enableAutoStart) {
  for (const auto &processInfo : m_processList) {
    startProcess(processInfo);
  }
}

void Daemon::onProcessError(QProcess::ProcessError error) {
  const QPointer process = qobject_cast<QProcess *>(sender());

  if (!process)
    return;

  QString program = process->program();
  qDebug() << "Process error:" << program << "Error:" << error;

  for (const auto &processInfo : m_processList) {
    if (processInfo.first == program) {
      qDebug() << "Restarting process due to error:" << program;
      QTimer::singleShot(1, this, [this, processInfo]() {
        startProcess(processInfo);
      }); // Restart after 1 second
      return;
    }
  }
}

void Daemon::startProcess(const QPair<QString, QStringList> &processInfo) {
  qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("xcb"));

  const QPointer process = new QProcess(this);

  if (this->m_enableAutoRestart)
    connect(process, &QProcess::errorOccurred, this, &Daemon::onProcessError);

  process->start(processInfo.first, processInfo.second);
  if (process->waitForStarted()) {
    qDebug() << "Process started:" << processInfo.first
             << "PID:" << process->processId();
  } else {
    qDebug() << "Failed to start process:" << processInfo.first
             << process->errorString();
  }
}

JobPrivate::JobPrivate() {}

JobPrivate::~JobPrivate() {}

Job::Job(QObject *parent) : QObject(parent), d_ptr(new JobPrivate) {}

Job::~Job() {
  if (!d_ptr->isFinished) {
    d_ptr->isFinished = true;
    Q_EMIT finished(this);
  }
}

void Job::emitResult() {
  if (d_func()->isFinished) {
    finishJob(true);
  }
}
void Job::finishJob(bool emitResult) {
  Q_D(Job);
  Q_ASSERT(!d->isFinished);
  d->isFinished = true;

  if (d->eventLoop) {
    d->eventLoop->quit();
  }

  Q_EMIT finished(this);

  if (emitResult) {
    Q_EMIT result(this);
  }

  if (isAutoDelete()) {
    deleteLater();
  }
}

bool Job::isAutoDelete() const {
  Q_D(const Job);
  return d->isAutoDelete;
}

void Job::setAutoDelete(bool autodelete) {
  Q_D(Job);
  d->isAutoDelete = autodelete;
}

bool Job::exec() {
  Q_D(Job);
  // Usually this job would delete itself, via deleteLater() just after
  // emitting result() (unless configured otherwise). Since we use an event
  // loop below, that event loop will process the deletion event and we'll
  // have been deleted when exec() returns. This crashes, so temporarily
  // suspend autodeletion and manually do it afterwards.
  const bool wasAutoDelete = isAutoDelete();
  setAutoDelete(false);

  Q_ASSERT(!d->eventLoop);

  QEventLoop loop(this);
  d->eventLoop = &loop;

  start();

  if (!d->isFinished) {
    d->m_startedWithExec = true;
    d->eventLoop->exec(QEventLoop::ExcludeUserInputEvents);
  }
  d->eventLoop = nullptr;

  if (wasAutoDelete) {
    deleteLater();
  }
  return (d->error == NoError);
}
} // namespace LINGMO_SESSION
