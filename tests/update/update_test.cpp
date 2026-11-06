// SPDX-FileCopyrightText: 2026 LingmoOS Contributors
// SPDX-License-Identifier: GPL-3.0-or-later
#include "updatemanager.h"
#include "lingmo/update/update_status.h"
#include "base/event_loop.h"
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <poll.h>
#include <sys/stat.h>
#include <unistd.h>
using namespace Lingmo;
using namespace std::chrono_literals;
void require(bool ok, const char* message) { if (!ok) { std::cerr << message << '\n'; std::exit(1); } }
void wait(base::EventLoop& loop, const std::function<bool()>& ready) {
    auto end = std::chrono::steady_clock::now() + 5s;
    while (!ready() && std::chrono::steady_clock::now() < end) { pollfd fd{loop.fd(), POLLIN, 0}; ::poll(&fd, 1, 5); loop.dispatch(); }
    require(ready(), "Timed out waiting for update state");
}
void write(const std::string& path, const std::string& text, bool executable = false) {
    std::ofstream file(path); file << text; file.close(); if (executable) ::chmod(path.c_str(), 0700);
}
int main() {
    std::string temp = "/tmp/lingmo-update-test-XXXXXX"; require(::mkdtemp(temp.data()), "temporary directory");
    struct Cleanup { std::string path; ~Cleanup() { std::filesystem::remove_all(path); } } cleanup{temp};
    // PATH contains only test doubles. No real curl, apt or pkexec can execute.
    ::setenv("PATH", temp.c_str(), 1);
    const auto mode = [&](const std::string& value) { write(temp + "/mode", value + "\n"); };
    const std::string prefix = "#!/bin/sh\nread mode < '" + temp + "/mode'\n";
    const auto curlScript = [&] {
        write(temp + "/curl", prefix +
            "case \"$mode\" in\n"
            " api-fail) echo 'TLS connection failed' >&2; exit 60;;\n"
            " invalid-api) echo '<html>unavailable</html>'; exit 0;;\n"
            " changelog-start-fail) /bin/rm -- '" + temp + "/curl'; echo '{\"success\":true,\"latest\":{\"build\":999999999999,\"version\":\"test\"}}'; exit 0;;\n"
            "esac\necho '{\"success\":true,\"latest\":false}'\n", true);
    };
    write(temp + "/pkexec", prefix +
        "if [ \"$2\" = update ]; then\n"
        " if [ \"$mode\" = refresh-fail ]; then echo 'Authorization denied' >&2; exit 126; fi\n"
        " /bin/sleep 0.05; exit 0\nfi\n"
        "if [ \"$mode\" = upgrade-fail ]; then echo 'Package lock unavailable' >&2; exit 100; fi\n"
        "echo 'dlstatus:pkg:40:Downloading'\necho 'pmstatus:pkg:100:Installed'\n", true);
    write(temp + "/apt", prefix +
        "if [ \"$mode\" = list-fail ]; then echo 'Package cache unreadable' >&2; exit 100; fi\n"
        "echo 'sample/stable 2.0 amd64 [upgradable from: 1.0]'\n", true);
    base::EventLoop loop;
    auto* bus = base::dbus::Connection::session(&loop); require(bus, "session bus");
    update::UpdateStatus status; // Before the service exists: no synchronous self-call.
    base::Settings settings(temp + "/settings");
    auto manager = std::make_unique<UpdateManager>(settings, bus);
    require(manager->publish() && bus->requestName("org.lingmo.Services"), "publish update service");
    wait(loop, [&] { return status.isAvailable(); });
    auto check = [&](const std::string& scenario, update::State expected) {
        mode(scenario); curlScript();
        bool observedChecking = false;
        auto changed = status.changed.connect([&] { if (status.state() == update::State::Checking) observedChecking = true; });
        status.checkForUpdate(); require(status.isRequestPending(), "command acknowledgement pending");
        wait(loop, [&] { return observedChecking && !status.isRequestPending() && status.state() == expected; });
    };
    check("api-fail", update::State::Error);
    require(status.errorLog().find("TLS connection failed") != std::string::npos, "curl diagnostics visible");
    check("success", update::State::SoftwareUpdate);
    require(status.errorLog().empty() && status.upgradableCount() == 1, "retry clears error and finds update");
    check("refresh-fail", update::State::Error);
    require(status.errorLog().find("Authorization denied") != std::string::npos, "APT refresh failure visible");
    check("list-fail", update::State::Error);
    require(status.errorLog().find("Package cache unreadable") != std::string::npos, "nonzero apt exit not up-to-date");
    check("invalid-api", update::State::Error);
    require(!status.errorLog().empty(), "invalid API response has details");
    check("changelog-start-fail", update::State::SystemUpgrade);
    require(status.upgradableCount() == 1, "missing changelog process does not kill APT");
    mode("upgrade-fail"); status.startUpgrade();
    wait(loop, [&] { return status.state() == update::State::Error && !status.isRequestPending(); });
    require(status.errorLog().find("Package lock unavailable") != std::string::npos, "upgrade stderr visible");
    check("success", update::State::SoftwareUpdate);
    status.startUpgrade(); wait(loop, [&] { return status.state() == update::State::UpToDate && !status.isRequestPending(); });
    require(status.errorLog().empty(), "upgrade retry succeeds");
    manager.reset();
    // Service name remains, but method calls now fail: surface the D-Bus error.
    status.checkForUpdate(); wait(loop, [&] { return !status.isRequestPending(); });
    require(!status.requestError().empty(), "D-Bus method errors are not silent");
    bus->releaseName("org.lingmo.Services"); wait(loop, [&] { return !status.isAvailable(); });
    std::cout << "Update retry integration passed\n";
}
