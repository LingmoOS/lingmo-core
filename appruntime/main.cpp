// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-FileCopyrightText: 2026 LingmoOS Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "appruntime.h"

#include "base/dbus.h"
#include "base/event_loop.h"
#include "base/logging.h"

#include <systemd/sd-bus.h>
#include <cstring>

#include <QString>
#include <QStringList>

namespace dbus = Lingmo::Base::DBus;

static constexpr char kService[] = "org.lingmo.AppRuntime";
static constexpr char kPath[]    = "/AppRuntime";

// launchApplication and launchCommand also come without an activation token.
// A method table holds one signature per name, so these shorter forms are
// answered here, before the table sees them.
static int handleShortForms(sd_bus_message *message, void *userdata, sd_bus_error *)
{
    if (!sd_bus_message_is_method_call(message, kService, nullptr))
        return 0;

    auto *runtime = static_cast<AppRuntime *>(userdata);
    const dbus::Message call(message);
    const char *member = sd_bus_message_get_member(message);
    uint32_t pid = 0;

    if (std::strcmp(member, "launchApplication") == 0 && call.signature() == "sas") {
        QString appId;
        QStringList arguments;
        if (!call.read(appId, arguments))
            return 0;
        pid = runtime->launchApplication(appId, arguments);
    } else if (std::strcmp(member, "launchCommand") == 0 && call.signature() == "sass") {
        QString appId;
        QStringList command;
        QString workingDirectory;
        if (!call.read(appId, command, workingDirectory))
            return 0;
        pid = runtime->launchCommand(appId, command, workingDirectory);
    } else {
        return 0;
    }

    sd_bus_reply_method_return(message, "u", pid);
    return 1;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    Lingmo::Base::EventLoop loop;
    static const Lingmo::Base::LogCategory log("lingmo.appruntime");

    dbus::Connection *bus = dbus::Connection::session(&loop);
    if (!bus) {
        CF_WARNING(log) << "Cannot connect to the session bus";
        return 1;
    }

    AppRuntime runtime;

    dbus::Object object(bus, kPath, kService);
    object.addMethod(
        "launchApplication",
        [&runtime](const QString &appId, const QStringList &arguments,
                   const QString &activationToken) {
            return runtime.launchApplication(appId, arguments, activationToken);
        },
        {{"appId", "arguments", "activationToken"}, {"pid"}});
    object.addMethod(
        "launchCommand",
        [&runtime](const QString &appId, const QStringList &command,
                   const QString &workingDirectory, const QString &activationToken) {
            return runtime.launchCommand(appId, command, workingDirectory, activationToken);
        },
        {{"appId", "command", "workingDirectory", "activationToken"}, {"pid"}});
    object.addMethod(
        "quitApplication",
        [&runtime](const QString &appId) { return runtime.quitApplication(appId); },
        {{"appId"}, {"result"}});
    object.addMethod("quitAll", [&runtime] { return runtime.quitAll(); }, {{}, {"result"}});
    object.addMethod(
        "quitByPid",
        [&runtime](uint32_t pid) { return runtime.quitByPid(pid); },
        {{"pid"}, {"result"}});
    object.addMethod(
        "forceQuitByPid",
        [&runtime](uint32_t pid) { return runtime.forceQuitByPid(pid); },
        {{"pid"}, {"result"}});
    object.addMethod(
        "isRunning",
        [&runtime](const QString &appId) { return runtime.isRunning(appId); },
        {{"appId"}, {"result"}});
    object.addMethod(
        "runningApplications",
        [&runtime] { return runtime.runningApplications(); },
        {{}, {"appIds"}});
    object.addMethod(
        "pidsForApplication",
        [&runtime](const QString &appId) { return runtime.pidsForApplication(appId); },
        {{"appId"}, {"pids"}});
    object.addSignal("applicationLaunched", "su", {"appId", "pid"});
    object.addSignal("applicationQuit",     "su", {"appId", "pid"});
    object.publish();

    sd_bus_slot *shortForms = nullptr;
    sd_bus_add_object(bus->bus(), &shortForms, kPath, handleShortForms, &runtime);
    const dbus::Slot shortFormsSlot(shortForms);

    const auto launched = runtime.applicationLaunched.connect(
        [&object](const QString &appId, uint32_t pid) {
            object.emitSignal("applicationLaunched", appId, pid);
        });
    const auto quit = runtime.applicationQuit.connect(
        [&object](const QString &appId, uint32_t pid) {
            object.emitSignal("applicationQuit", appId, pid);
        });

    if (!bus->requestName(kService)) {
        CF_WARNING(log) << "Another application runtime is already running";
        return 1;
    }

    return loop.run();
}
