#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <QAbstractNativeEventFilter>
#include <QObject>
#include <QProcess>
#include <QEventLoop>
#include <QMap>

class Application;
class ProcessManager : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit ProcessManager(Application *app, QObject *parent = nullptr);
    ~ProcessManager();

    void start();
    void logout();

    void startWindowManager();
    void startDesktopProcess();
    void startDaemonProcess();
    void loadAutoStartProcess();

    bool nativeEventFilter(const QByteArray & eventType, void * message, long * result) override;

private:
    Application *m_app;
    QMap<QString, QProcess *> m_systemProcess;
    QMap<QString, QProcess *> m_autoStartProcess;

    bool m_wmStarted;
    QEventLoop *m_waitLoop;
};

#endif // PROCESSMANAGER_H
