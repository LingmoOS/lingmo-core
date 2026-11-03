// SPDX-FileCopyrightText: 2026 Lingmo OS Team
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "base/dbus.h"
#include "base/process.h"
#include "base/settings.h"
#include "base/timer.h"

// Checks for system updates in two stages:
//
//   1. The lingmo.org API (GET /api/v1/update) — tells whether a new OS
//      image is available and carries its version, build number, release
//      notes URL and ISO download link.
//   2. apt — first "apt-get update" to refresh package lists, then
//      "apt list --upgradable" to enumerate what is pending.
//
// The two results are combined into one of three outcomes:
//
//   SystemUpgrade  — API says new version AND apt has upgrades.
//   SoftwareUpdate — API says up-to-date but apt has upgrades.
//   UpToDate       — Neither source has anything to install.
//
// The result is published on the session bus as org.lingmo.Services.Update
// at /org/lingmo/Services/Update. A D-Bus signal is sent whenever the
// state or the upgrade progress changes.
class UpdateManager
{
public:
    // The overall state of the update subsystem.
    enum State {
        Idle           = 0,  // No check has run yet since startup.
        Checking       = 1,  // A check is in progress.
        SystemUpgrade  = 2,  // New OS version available (API + apt both positive).
        SoftwareUpdate = 3,  // Regular package upgrades only (apt positive, API negative).
        UpToDate       = 4,  // Nothing to install.
        Upgrading      = 5,  // apt-get upgrade is running.
        Error          = 6,
    };

    // The release channels the API serves.
    enum Channel { Stable = 0, Beta = 1, Nightly = 2 };

    UpdateManager(Lingmo::Base::Settings &settings, Lingmo::Base::DBus::Connection *session);
    ~UpdateManager();

    UpdateManager(const UpdateManager &) = delete;
    UpdateManager &operator=(const UpdateManager &) = delete;

    // org.lingmo.Services.Update at /org/lingmo/Services/Update.
    bool publish();

    // Triggers a full two-stage check; ignored while one is already running
    // or while an upgrade is in progress.
    void checkForUpdate();

    // Starts apt-get upgrade; ignored unless state is SystemUpgrade or
    // SoftwareUpdate. Progress is reported via the upgradeProgress signal.
    void startUpgrade();

    // Changes the release channel and re-runs the check.
    void setChannel(int channel);

private:
    // --- stage 1: API check ---
    void fetchApi();
    void parseApiResponse(const std::string &text);

    // --- stage 2: apt check (two sub-steps) ---
    // 2a. pkexec apt-get update  — refreshes package lists from the mirrors.
    void runAptUpdate();
    // 2b. apt list --upgradable  — enumerates packages to upgrade.
    void runAptListUpgradable();
    void parseUpgradableList(const std::string &text);

    // --- upgrade ---
    void runAptUpgrade();

    // --- helpers ---
    void loadSettings();
    void saveChannel();
    static std::string machineArchitecture();
    static int64_t readLocalBuild();
    static std::string readCurrentVersion();
    std::string apiUrl() const;
    void setState(State state);
    void emitInfoChanged();

    Lingmo::Base::Settings &m_settings;
    Lingmo::Base::DBus::Connection *m_session;
    std::unique_ptr<Lingmo::Base::DBus::Object> m_object;
    std::unique_ptr<Lingmo::Base::Process> m_process;
    Lingmo::Base::Timer m_timer;

    Channel m_channel  = Stable;
    State   m_state    = Idle;
    std::string m_arch;
    int64_t m_localBuild = 0;

    // Set by the API stage.
    bool        m_apiHasUpdate = false;
    std::string m_newVersion;
    int64_t     m_newBuild     = 0;
    bool        m_mandatory    = false;
    std::string m_releaseNotes;  // full URL
    std::string m_downloadUrl;
    std::string m_releaseDate;

    // Set by the apt stage.
    bool m_aptHasUpdate    = false;
    int  m_upgradableCount = 0;   // number of packages apt would upgrade
    // The package names, space-separated, for the UI to display if desired.
    std::string m_upgradablePackages;

    // Upgrade phase and progress.
    // Phase: 0 = not upgrading, 1 = downloading, 2 = installing.
    // Progress: 0–100 within the current phase, -1 when not upgrading.
    int m_upgradePhase    = 0;
    int m_upgradeProgress = -1;
};
