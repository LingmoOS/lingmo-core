// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-FileCopyrightText: 2026 LingmoOS Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstdint>
#include <string>

#include "base/dbus.h"
#include "base/signal.h"

// One message in the notification centre (org.freedesktop.Notifications).
// show() again replaces what it showed, until the user or close() removed it.
class Notification
{
public:
    enum Urgency : uint8_t {
        Low,
        Normal,
        Critical,
    };

    explicit Notification(Lingmo::Base::DBus::Connection *bus);
    ~Notification();

    Notification(const Notification &) = delete;
    Notification &operator=(const Notification &) = delete;

    // The application it is from.
    void setDesktopEntry(std::string id) { m_desktopEntry = std::move(id); }
    void setIconName(std::string name) { m_iconName = std::move(name); }
    void setSummary(std::string summary) { m_summary = std::move(summary); }
    void setBody(std::string body) { m_body = std::move(body); }
    void setUrgency(Urgency urgency) { m_urgency = urgency; }
    // Notify's expire_timeout; -1 leaves it to the server.
    void setTimeout(int timeout) { m_timeout = timeout; }

    void show();
    void close();

    // The user or the server removed it.
    Lingmo::Base::Signal<> closed;
    // No server took it, as early in a session.
    Lingmo::Base::Signal<> failed;

private:
    void send();
    void sendClose();

    Lingmo::Base::DBus::Connection *m_bus;
    Lingmo::Base::DBus::Proxy m_proxy;
    Lingmo::Base::DBus::Match m_closedMatch;
    Lingmo::Base::DBus::PendingCall m_call;
    std::string m_desktopEntry;
    std::string m_iconName;
    std::string m_summary;
    std::string m_body;
    Urgency m_urgency = Normal;
    int m_timeout = -1;
    uint32_t m_id = 0;
    bool m_pending = false;
    bool m_showAgain = false;
    bool m_closeWhenShown = false;
};

