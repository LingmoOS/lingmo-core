#ifndef FDOSELECTIONMANAGER_H
#define FDOSELECTIONMANAGER_H

#include <QObject>
#include <QSystemTrayIcon>

class FdoSelectionManager : public QObject
{
    Q_OBJECT

public:
    explicit FdoSelectionManager(QObject *parent = nullptr);
    ~FdoSelectionManager();

private:
    QSystemTrayIcon *m_trayIcon;
};

#endif // FDOSELECTIONMANAGER_H
