#ifndef JOB_PRIVATE_HPP
#define JOB_PRIVATE_HPP

#include "daemon-helper.h"
#include <QEventLoopLocker>
#include <QMap>

#include <array>

using namespace LINGMO_SESSION;

class QTimer;
class QEventLoop;

// This is a private class, but it's exported for
// KIO::Job's usage. Other Job classes in kdelibs may
// use it too.
namespace LINGMO_SESSION {
class JobPrivate {
public:
  JobPrivate();
  virtual ~JobPrivate();

  Job *q_ptr = nullptr;

  QEventLoop *eventLoop = nullptr;
  // eventLoopLocker prevents QCoreApplication from exiting when the last
  // window is closed until the job has finished running
  QEventLoopLocker eventLoopLocker;
  bool suspended = false;
  bool isAutoDelete = true;

  bool isFinished = false;
  bool m_startedWithExec = false;

  int error = Job::NoError;

  Q_DECLARE_PUBLIC(Job)
};
} // namespace LINGMO_SESSION
#endif