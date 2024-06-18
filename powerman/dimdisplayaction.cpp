/***************************************************************************
 *   Copyright (C) 2023-2024 by Reion Wong <reion@lingmoos.com>               *
 *   Copyright (C) 2010 by Dario Freddi <drf@kde.org>                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "dimdisplayaction.h"
#include <QSettings>
#include <QTimer>
#include <QDBusPendingCall>
#include <QProcess>
#include <QDebug>

#include <X11/Xlib.h>
#include <xcb/dpms.h>

DimDisplayAction::DimDisplayAction(QObject *parent)
    : Action(parent)
    , m_iface("com.lingmo.Settings",
              "/Brightness",
              "com.lingmo.Brightness", QDBusConnection::sessionBus())
{
    auto isPlatformX11 = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
    if (isPlatformX11) {
        auto *x11App = qApp->nativeInterface<QNativeInterface::QX11Application>();

        // 获取Display类型的显示指针
        auto *displayID = x11App->display();

        // 从Display转换为xcb_connection_t类型的连接
        auto *connection = XGetXCBConnection(displayID);

        xcb_dpms_set_timeouts(connection, 0, 0, 0);

        XSetScreenSaver(displayID, 0, 0, 0, 0);
    }
}

void DimDisplayAction::onWakeupFromIdle()
{   
    if (!m_dimmed) {
        return;
    }

    if (m_oldScreenBrightness < 0)
        m_oldScreenBrightness = 1;

    // An active inhibition may not let us restore the brightness.
    // We should wait a bit screen to wake-up from sleep
    QTimer::singleShot(0, this, [this]() {
        m_iface.asyncCall("setValue", QVariant::fromValue(m_oldScreenBrightness));
    });

    m_dimmed = false;
}

void DimDisplayAction::onIdleTimeout(int msec)
{
    int sec = msec / 1000;

    if (m_iface.property("brightness").toInt() == 0)
        return;

    if (sec == m_dimOnIdleTime) {
        m_iface.asyncCall("setValue", QVariant::fromValue(0));

        // Sleep
        if (m_sleep) {
            QDBusInterface iface("com.lingmo.Session",
                                 "/Session",
                                 "com.lingmo.Session", QDBusConnection::sessionBus());

            if (iface.isValid()) {
                iface.call("suspend");
            }
        }

        if (m_lock) {
            QProcess::startDetached("lingmo-screenlocker", QStringList());
        }

    } else if (sec == (m_dimOnIdleTime * 3 / 4)) {
        const int newBrightness = qRound(m_oldScreenBrightness / 8.0);
        m_iface.asyncCall("setValue", QVariant::fromValue(newBrightness));
    } else if (sec == (m_dimOnIdleTime * 1 / 2)) {
        m_oldScreenBrightness = m_iface.property("brightness").toInt();

        const int newBrightness = qRound(m_oldScreenBrightness / 2.0);

        m_iface.asyncCall("setValue", QVariant::fromValue(newBrightness));
    }

    m_dimmed = true;
}

void DimDisplayAction::setTimeout(int timeout)
{
    unregisterIdleTimeout();

    if (timeout < 0) {
        m_dimOnIdleTime = timeout;
        return;
    }

    m_dimOnIdleTime = timeout;
    registerIdleTimeout(m_dimOnIdleTime * 3 / 4);
    registerIdleTimeout(m_dimOnIdleTime / 2);
    registerIdleTimeout(m_dimOnIdleTime);
}

void DimDisplayAction::setSleep(bool sleep)
{
    m_sleep = sleep;
}

void DimDisplayAction::setLock(bool lock)
{
    m_lock = lock;
}
