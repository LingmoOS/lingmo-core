#ifndef FDOSELECTIONMANAGER_H
#define FDOSELECTIONMANAGER_H

#include <QObject>
#include <QAbstractNativeEventFilter>
#include <QHash>
#include <QSystemTrayIcon>

class SNIProxy; // 假设这是处理系统托盘图标的类
class KSelectionOwner; // 假设这是管理X11选择的类

class FdoSelectionManager : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit FdoSelectionManager(QObject *parent = nullptr);
    ~FdoSelectionManager() override;

    // 重写QAbstractNativeEventFilter的函数
    bool nativeEventFilter(const QByteArray &eventType, void *message, long int *result) override;

private slots:
    void onClaimedOwnership();
    void onFailedToClaimOwnership();
    void onLostOwnership();

private:
    void init();
    bool addDamageWatch(xcb_window_t client);
    void dock(xcb_window_t embed_win);
    void undock(xcb_window_t client);
    void setSystemTrayVisual();

    QSystemTrayIcon *m_trayIcon;
    uint8_t m_damageEventBase;
    QHash<xcb_window_t, u_int32_t> m_damageWatches;
    QHash<xcb_window_t, SNIProxy *> m_proxies;
    KSelectionOwner *m_selectionOwner;
};

#endif // FDOSELECTIONMANAGER_H
