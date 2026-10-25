// SPDX-FileCopyrightText: 2026 LingmoOS Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "applicationscope.h"

#include "base/dbus.h"
#include "base/logging.h"
#include "common/detached_call.h"

#include <cstdio>
#include <tuple>
#include <vector>

#include <signal.h>

#include <QString>

namespace dbus = Lingmo::Base::DBus;

static const Lingmo::Base::LogCategory kLog("lingmo.appruntime");

using UnitProperty = std::tuple<QString, dbus::Variant>;
using AuxiliaryUnit = std::tuple<QString, std::vector<UnitProperty>>;

// systemd unit name escaping: [a-zA-Z0-9:_.] pass, a leading '.' and
// everything else become \xNN of the UTF-8 byte.
static QString escape(const QString &text)
{
    QString result;
    const QByteArray utf8 = text.toUtf8();
    for (int i = 0; i < utf8.size(); ++i) {
        const unsigned char c = static_cast<unsigned char>(utf8[i]);
        const bool plain = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
            || (c >= '0' && c <= '9') || c == ':' || c == '_'
            || (c == '.' && i > 0);
        if (plain) {
            result += QLatin1Char(char(c));
        } else {
            char escaped[5];
            std::snprintf(escaped, sizeof(escaped), "\\x%02x", c);
            result += QLatin1String(escaped);
        }
    }
    return result;
}

static dbus::Message managerCall(dbus::Connection *bus, const char *method)
{
    return bus->newMethodCall("org.freedesktop.systemd1",
                              "/org/freedesktop/systemd1",
                              "org.freedesktop.systemd1.Manager", method);
}

QString ApplicationScope::unitName(const QString &appId, uint32_t pid)
{
    return QStringLiteral("app-lingmo-") + escape(appId)
         + QLatin1Char('-') + QString::number(pid)
         + QStringLiteral(".scope");
}

void ApplicationScope::adopt(const QString &appId, uint32_t pid)
{
    if (appId.isEmpty() || pid == 0)
        return;
    dbus::Connection *bus = dbus::Connection::session();
    if (!bus)
        return;

    const std::vector<UnitProperty> properties{
        {QStringLiteral("PIDs"),        dbus::Variant(std::vector<uint32_t>{pid})},
        {QStringLiteral("CollectMode"), dbus::Variant(QStringLiteral("inactive-or-failed"))},
    };

    dbus::Message call = managerCall(bus, "StartTransientUnit");
    call.append(unitName(appId, pid), QStringLiteral("fail"),
                properties, std::vector<AuxiliaryUnit>());

    Lingmo::Core::callDetached(bus, call, [appId, pid](const dbus::Reply &reply) {
        if (reply.isError())
            CF_WARNING(kLog) << "AppRuntime: no systemd scope for"
                             << appId << pid << reply.error().message;
    });
}

void ApplicationScope::killAll(const QString &unitName)
{
    dbus::Connection *bus = dbus::Connection::session();
    if (!bus)
        return;
    dbus::Message call = managerCall(bus, "KillUnit");
    call.append(unitName, QStringLiteral("all"), int32_t(SIGKILL));
    bus->send(call);
}
