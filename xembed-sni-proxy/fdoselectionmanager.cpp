#include "fdoselectionmanager.h"
#include <QMenu>

FdoSelectionManager::FdoSelectionManager(QObject *parent)
    : QObject(parent)
    , m_trayIcon(new QSystemTrayIcon(this))
{
    // 设置系统托盘图标
    m_trayIcon->setIcon(QIcon(":/icons/tray_icon.png"));

    // 创建上下文菜单
    QMenu *trayIconMenu = new QMenu;
    QAction *quitAction = trayIconMenu->addAction("退出");
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    m_trayIcon->setContextMenu(trayIconMenu);

    // 显示系统托盘图标
    m_trayIcon->show();
}

FdoSelectionManager::~FdoSelectionManager()
{
    // 清理资源
    m_trayIcon->hide();
}
