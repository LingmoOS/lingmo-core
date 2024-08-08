#ifndef __DAEMON_HELPER_
#define __DAEMON_HELPER_

#include <QDebug>
#include <QEventLoop>
#include <QList>
#include <QObject>
#include <QPair>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <qobjectdefs.h>

namespace LINGMO_SESSION {
class JobPrivate;

class Daemon : public QObject {
  Q_OBJECT

public:
  /**
   * Start all the passed process using daemon.
   * @param processList Process list to start
   * @param parent
   */
  explicit Daemon(const QList<QPair<QString, QStringList>> &processList,
                  bool _enableAutoStart = true, QObject *parent = nullptr);

public slots:

  /**
   *  Handle the case when the progarm has some errors (i.e. crashed)
   * @param error
   */
  void onProcessError(QProcess::ProcessError error);

private:
  /**
   *  Start a given process using daemon helper
   * @brief startProcess
   * @param processInfo
   */
  void startProcess(const QPair<QString, QStringList> &processInfo);

  QList<QPair<QString, QStringList>> m_processList;

  /**
   * @brief Whether to enable auto reload when process exited.
   */
  bool m_enableAutoRestart;
};

class Job : public QObject {
  Q_OBJECT
public:
  Job(QObject *parent = nullptr);

  ~Job();

  /**
   * Returns whether this job automatically deletes itself once
   * the job is finished.
   *
   * @return whether the job is deleted automatically after
   * finishing.
   */
  bool isAutoDelete() const;

  /**
   * Sets the auto-delete property of the job. If @p autodelete is
   * set to @c false the job will not delete itself once it is finished.
   *
   * The default for any Job is to automatically delete itself, which
   * implies that the job was created on the heap (using <tt>new</tt>).
   * If the job is created on the stack (which isn't the typical use-case
   * for a job) then you must set auto-delete to @c false, otherwise you
   * could get a crash when the job finishes and tries to delete itself.
   *
   * @note If you set auto-delete to @c false then you need to kill the
   * job manually, ideally by calling kill().
   *
   * @param autodelete set to @c false to disable automatic deletion
   * of the job.
   */
  void setAutoDelete(bool autodelete);

  /**
   * Executes the job synchronously.
   *
   * This will start a nested QEventLoop internally. Nested event loop can be
   * dangerous and can have unintended side effects, you should avoid calling
   * exec() whenever you can and use the asynchronous interface of Job instead.
   *
   * Should you indeed call this method, you need to make sure that all callers
   * are reentrant, so that events delivered by the inner event loop don't cause
   * non-reentrant functions to be called, which usually wreaks havoc.
   *
   * Note that the event loop started by this method does not process user input
   * events, which means your user interface will effectively be blocked. Other
   * events like paint or network events are still being processed. The
   * advantage of not processing user input events is that the chance of
   * accidental reentrance is greatly reduced. Still you should avoid calling
   * this function.
   *
   * @return true if the job has been executed without error, false otherwise
   */
  bool exec();

  /**
   * Starts the job asynchronously.
   *
   * When the job is finished, result() is emitted.
   *
   * Warning: Never implement any synchronous workload in this method. This
   * method should just trigger the job startup, not do any work itself. It is
   * expected to be non-blocking.
   *
   * This is the method all subclasses need to implement.
   * It should setup and trigger the workload of the job. It should not do any
   * work itself. This includes all signals and terminating the job, e.g. by
   * emitResult(). The workload, which could be another method of the
   * subclass, is to be triggered using the event loop, e.g. by code like:
   * \code
   * void ExampleJob::start()
   * {
   *  QTimer::singleShot(0, this, &ExampleJob::doWork);
   * }
   * \endcode
   */
  Q_SCRIPTABLE virtual void start() = 0;

  enum {
    /*** Indicates there is no error */
    NoError = 0,
    /*** Indicates the job was killed */
    KilledJobError = 1,
    /*** Subclasses should define error codes starting at this value */
    UserDefinedError = 100,
  };
Q_SIGNALS:
  /**
   * Emitted when the job is finished, in any case. It is used to notify
   * observers that the job is terminated and that progress can be hidden.
   *
   * This signal is guaranteed to be emitted exactly once.
   *
   * This is a private signal, it can't be emitted directly by subclass, use
   * emitResult() instead.
   *
   * In general, to be notified of a job's completion, client code should
   * connect to result() rather than finished(), so that kill(Quietly) is indeed
   * quiet. However if you store a list of jobs and they might get killed
   * silently, then you must connect to this instead of result(), to avoid
   * dangling pointers in your list.
   *
   * @param job the job that emitted this signal
   * @internal
   *
   * @see result
   */
  void finished(Job *job);

  /**
   * Emitted when the job is finished (except when killed with KJob::Quietly).
   *
   * This signal is guaranteed to be emitted at most once.
   *
   * Use error to know if the job was finished with error.
   *
   * This is a private signal, it can't be emitted directly by subclasses of
   * KJob, use emitResult() instead.
   *
   * Please connect to this signal instead of finished.
   *
   * @param job the job that emitted this signal
   *
   * @see kill
   */
  void result(Job *job);

protected :
  /**
   * Utility function to emit the result signal, and end this job.
   * It first notifies the observers to hide the progress for this job using
   * the finished() signal.
   *
   * @note Deletes this job using deleteLater().
   *
   * @see result()
   * @see finished()
   */
  Q_SLOT void emitResult();

  std::unique_ptr<JobPrivate> const d_ptr;

private:
  void finishJob(bool emitResult);

  Q_DECLARE_PRIVATE(Job)
};
} // namespace LINGMO_SESSION
#endif
