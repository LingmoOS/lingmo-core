// SPDX-FileCopyrightText: 2026 Lingmo OS Team
// SPDX-License-Identifier: GPL-3.0-or-later

#include "updatemanager.h"

#include <sys/utsname.h>

#include <chrono>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

#include "base/i18n.h"
#include "base/json.h"
#include "log.h"

using namespace std::chrono_literals;
using namespace Lingmo::Base;
using Lingmo::I18n::tr;
using Lingmo::I18n::format;

static int64_t nowSeconds()
{
    return std::chrono::duration_cast<std::chrono::seconds>(
                   std::chrono::system_clock::now().time_since_epoch())
            .count();
}

static constexpr auto s_channelKey       = "Update/Channel";
static constexpr auto s_lastCheckTimeKey = "Update/LastCheckTime";
static constexpr auto s_apiBase          = "https://lingmo.org/api/v1/update";
static constexpr auto s_releasesUrl      = "https://lingmo.org/content/releases.json";
static constexpr auto s_checkInterval   = 6h;
static constexpr auto s_startDelay      = 30s;

static const char *channelName(UpdateManager::Channel ch)
{
    switch (ch) {
    case UpdateManager::Beta:    return "beta";
    case UpdateManager::Nightly: return "nightly";
    default:                     return "stable";
    }
}

// Parse one line from the apt Status-Fd stream.
//
// When apt-get is run with -o APT::Status-Fd=<fd>, it writes colon-separated
// status lines to that file descriptor. The format is:
//
//   <type>:<package>:<percent>:<message>
//
// where <type> is one of:
//   dlstatus  — a package is being downloaded   (percent 0-100 per package)
//   pmstatus  — dpkg is installing / configuring (percent 0-100 overall)
//
// We report two separate phases to the UI:
//   phase 1 (downloading): dlstatus lines, percent 0-100
//   phase 2 (installing):  pmstatus lines, percent 0-100
//
// Returns false when the line is not a recognised status line.
static bool parseAptStatusFd(const std::string &line, int &outPhase, int &outPercent)
{
    // Expected: "dlstatus:pkg:30.5:Downloading"
    const auto c1 = line.find(':');
    if (c1 == std::string::npos) return false;
    const auto c2 = line.find(':', c1 + 1);
    if (c2 == std::string::npos) return false;
    const auto c3 = line.find(':', c2 + 1);
    if (c3 == std::string::npos) return false;

    const std::string type    = line.substr(0, c1);
    const std::string pctStr  = line.substr(c2 + 1, c3 - c2 - 1);

    double pct = 0;
    try { pct = std::stod(pctStr); } catch (...) { return false; }
    if (!std::isfinite(pct)) return false;
    pct = std::max(0.0, std::min(100.0, pct));

    if (type == "dlstatus") {
        outPhase   = 1;           // downloading
        outPercent = int(pct);
        return true;
    }
    if (type == "pmstatus") {
        outPhase   = 2;           // installing
        outPercent = int(pct);
        return true;
    }
    return false;
}

// ─────────────────────────────────────────────────────────────────────────────

UpdateManager::UpdateManager(Settings &settings, dbus::Connection *session)
    : m_settings(settings)
    , m_session(session)
    , m_arch(machineArchitecture())
    , m_localBuild(readLocalBuild())
{
    loadSettings();

    // First check fires after a short startup delay; subsequent checks repeat
    // on the regular interval.
    m_timer.setSingleShot(true);
    m_timer.setInterval(s_startDelay);
    m_timer.onTimeout = [this] {
        checkForUpdate();
        m_timer.setSingleShot(false);
        m_timer.setInterval(s_checkInterval);
        m_timer.start();
    };
    m_timer.start();
}

UpdateManager::~UpdateManager() = default;

bool UpdateManager::publish()
{
    if (!m_session)
        return false;

    auto object = std::make_unique<dbus::Object>(m_session, "/org/lingmo/Services/Update",
                                                 "org.lingmo.Services.Update");

    object->addMethod("checkForUpdate", [this] { checkForUpdate(); });
    object->addMethod("startUpgrade",   [this] { startUpgrade(); });
    object->addMethod("setChannel", [this](int32_t ch) { setChannel(ch); },
                      {{"channel"}, {}});

    object->addProperty("state",               [this] { return int32_t(m_state); });
    object->addProperty("channel",             [this] { return int32_t(m_channel); });
    object->addProperty("newVersion",          [this] { return m_newVersion; });
    object->addProperty("newBuild",            [this] { return m_newBuild; });
    object->addProperty("mandatory",           [this] { return m_mandatory; });
    object->addProperty("releaseNotes",        [this] { return m_releaseNotes; });
    object->addProperty("downloadUrl",         [this] { return m_downloadUrl; });
    object->addProperty("releaseDate",         [this] { return m_releaseDate; });
    object->addProperty("upgradableCount",     [this] { return int32_t(m_upgradableCount); });
    object->addProperty("upgradablePackages",  [this] { return m_upgradablePackages; });
    object->addProperty("upgradeProgress",     [this] { return int32_t(m_upgradeProgress); });
    // upgradePhase: 0 = not upgrading, 1 = downloading, 2 = installing.
    object->addProperty("upgradePhase",        [this] { return int32_t(m_upgradePhase); });
    // Unix timestamp of the last completed check (0 if none yet).
    object->addProperty("lastCheckTime",       [this] { return m_lastCheckTime; });
    // Changelog text (Markdown) for the available version; empty when up-to-date.
    object->addProperty("changelogText",       [this] { return m_changelogText; });
    // Error log from the last failed upgrade; empty unless state == Error.
    object->addProperty("errorLog",            [this] { return m_errorLog; });

    object->addSignal("stateChanged",    "i",  {"state"});
    // Fired whenever phase or progress changes.
    // phase: 1 = downloading, 2 = installing.  percent: 0–100.
    object->addSignal("upgradeProgress", "ii", {"phase", "percent"});

    if (!object->publish())
        return false;

    m_object = std::move(object);
    return true;
}

void UpdateManager::checkForUpdate()
{
    if (m_state == Checking || m_state == Upgrading)
        return;

    m_errorLog.clear();
    m_newVersion.clear(); m_newBuild = 0; m_mandatory = false;
    m_releaseNotes.clear(); m_downloadUrl.clear(); m_releaseDate.clear(); m_changelogText.clear();
    m_upgradePhase = 0; m_upgradeProgress = -1;
    m_apiHasUpdate       = false;
    m_aptHasUpdate       = false;
    m_upgradableCount    = 0;
    m_upgradablePackages.clear();

    setState(Checking);
    fetchApi();
}

void UpdateManager::startUpgrade()
{
    if (m_state != SystemUpgrade && m_state != SoftwareUpdate)
        return;

    m_errorLog.clear();

    // Upgrade begins in the download phase; apt may emit dlstatus lines
    // first, then switch to pmstatus for the install phase.
    m_upgradePhase    = 1;
    m_upgradeProgress = 0;
    if (m_object)
        m_object->emitSignal("upgradeProgress", int32_t(m_upgradePhase),
                            int32_t(m_upgradeProgress));

    setState(Upgrading);
    runAptUpgrade();
}

void UpdateManager::setChannel(int channel)
{
    if (m_state == Checking || m_state == Upgrading) return;
    const Channel ch = (channel >= Stable && channel <= Nightly)
                           ? static_cast<Channel>(channel)
                           : Stable;
    if (ch == m_channel)
        return;
    m_channel = ch;
    saveChannel();
    if (m_object)
        m_object->emitPropertiesChanged({"channel"});

    m_timer.start();
    checkForUpdate();
}

// ── stage 1: API ─────────────────────────────────────────────────────────────

void UpdateManager::fetchApi()
{
    m_process = std::make_unique<Process>();
    m_process->setOomProtectionDropped(true);
    m_process->onFinished = [this](int exitCode, bool crashed) {
        const std::string output = m_process->standardOutput();
        const std::string error = m_process->standardError();
        m_process.reset();
        if (crashed || exitCode != 0) {
            LM_WARNING(services::log())
                    << "Update API request failed (curl exited" << exitCode << ")";
            failOperation(format(tr("Update API request failed (curl exit {code}): {error}"), {{"code", std::to_string(exitCode)}, {"error", error}}));
            return;
        }
        parseApiResponse(output);
    };

    if (!m_process->start("curl", {"--max-time", "15", "--location", "--fail", "--silent", "--show-error", apiUrl()})) {
        LM_WARNING(services::log()) << "Cannot start curl for update API check";
        m_process.reset();
        failOperation(tr("Cannot start curl. Install curl and check the update service PATH."));
    }
}

void UpdateManager::parseApiResponse(const std::string &text)
{
    const auto root = json::Value::parse(text);
    if (!root || !root->operator[]("success").toBool()) {
        LM_WARNING(services::log()) << "Update API returned an unexpected response";
        failOperation(tr("Update API returned an invalid response (expected success=true)."));
        return;
    }

    const auto &latest = root->operator[]("latest");
    if (!latest.isNull()) {
        const int64_t remoteBuild = latest["build"].toInt();
        if (remoteBuild > m_localBuild) {
            m_apiHasUpdate = true;
            m_newVersion   = latest["version"].toString();
            m_newBuild     = remoteBuild;
            m_mandatory    = latest["mandatory"].toBool();
            m_releaseNotes = "https://lingmo.org" + latest["releaseNotes"].toString();
            m_downloadUrl  = latest["download"]["iso"].toString();
            m_releaseDate  = latest["date"].toString();
        }
    }

    // When a new version is available, fetch its changelog before moving to
    // the apt stage so the UI can show release notes right away.
    if (m_apiHasUpdate) {
        fetchChangelog();
    } else {
        runAptUpdate();
    }
}

// ── stage 1b: fetch changelog ────────────────────────────────────────────────

void UpdateManager::fetchChangelog()
{
    // releases.json holds an array of {version, date, title, content, type}.
    // We match on newVersion to get the Markdown changelog for the UI card.
    m_process = std::make_unique<Process>();
    m_process->setOomProtectionDropped(true);
    m_process->onFinished = [this](int exitCode, bool crashed) {
        const std::string output = m_process->standardOutput();
        m_process.reset();
        // A changelog fetch failure is non-fatal; just proceed without text.
        if (!crashed && exitCode == 0)
            parseChangelog(output);
        runAptUpdate();
    };

    if (!m_process->start("curl", {"--max-time", "10", "--location", "--fail", "--silent", "--show-error", s_releasesUrl})) {
        LM_WARNING(services::log()) << "Cannot start curl for changelog fetch";
        m_process.reset();
        runAptUpdate();
    }
}

void UpdateManager::parseChangelog(const std::string &text)
{
    const auto root = json::Value::parse(text);
    if (!root || !root->isArray())
        return;

    for (const auto &entry : root->toArray()) {
        if (entry["version"].toString() == m_newVersion) {
            m_changelogText = entry["content"].toString();
            m_releaseDate   = entry["date"].toString();  // authoritative date
            return;
        }
    }
}

// ── stage 2a: apt-get update ─────────────────────────────────────────────────

void UpdateManager::runAptUpdate()
{
    // Refresh the package lists from the mirrors. Needs root via pkexec.
    // -qq suppresses all output except errors.
    m_process = std::make_unique<Process>();
    m_process->setOomProtectionDropped(true);
    m_process->setEnvironment({"LANG=C", "LANGUAGE=C", "LC_ALL=C",
                               "DEBIAN_FRONTEND=noninteractive"});
    m_process->onFinished = [this](int exitCode, bool crashed) {
        const std::string error = m_process->standardError();
        m_process.reset();
        if (crashed || exitCode != 0) {
            failOperation(format(tr("Refreshing package lists failed (pkexec/apt-get exit {code}): {error}"), {{"code", std::to_string(exitCode)}, {"error", error}}));
            return;
        }
        runAptListUpgradable();
    };

    if (!m_process->start("pkexec", {"/usr/bin/apt-get", "update", "-qq", "-o", "APT::Update::Error-Mode=any"})) {
        m_process.reset();
        failOperation(tr("Cannot start pkexec to refresh package lists. Check polkit installation."));
    }
}

// ── stage 2b: apt list --upgradable ─────────────────────────────────────────

void UpdateManager::runAptListUpgradable()
{
    // Enumerate packages that have a newer version available.
    // Output format (one line per package):
    //   bash/bookworm 5.2.15 amd64 [upgradable from: 5.2.14]
    m_process = std::make_unique<Process>();
    m_process->setOomProtectionDropped(true);
    m_process->setEnvironment({"LANG=C", "LANGUAGE=C", "LC_ALL=C"});
    m_process->onFinished = [this](int exitCode, bool crashed) {
        const std::string output = m_process->standardOutput();
        const std::string error = m_process->standardError();
        m_process.reset();
        if (crashed || exitCode != 0) {
            failOperation(format(tr("Listing upgradable packages failed (apt exit {code}): {error}"), {{"code", std::to_string(exitCode)}, {"error", error}}));
            return;
        }
        parseUpgradableList(output);
    };

    if (!m_process->start("apt", {"list", "--upgradable"})) {
        m_process.reset();
        failOperation(tr("Cannot start apt to list upgradable packages."));
    }
}

void UpdateManager::parseUpgradableList(const std::string &text)
{
    // Each real package line contains "[upgradable from:".
    std::istringstream ss(text);
    std::string line;
    std::vector<std::string> names;

    while (std::getline(ss, line)) {
        if (line.find("[upgradable from:") == std::string::npos)
            continue;
        // Package name is the part before the first '/'.
        const auto slash = line.find('/');
        if (slash != std::string::npos)
            names.push_back(line.substr(0, slash));
    }

    m_upgradableCount = int(names.size());
    m_aptHasUpdate    = m_upgradableCount > 0;

    // Build a space-separated list for the UI.
    m_upgradablePackages.clear();
    for (const auto &n : names) {
        if (!m_upgradablePackages.empty())
            m_upgradablePackages += ' ';
        m_upgradablePackages += n;
    }

    // Publish completion metadata before announcing the resulting state.
    m_lastCheckTime = nowSeconds();
    saveLastCheckTime();

    // Apply the decision matrix.
    if (m_apiHasUpdate && m_aptHasUpdate) {
        setState(SystemUpgrade);
    } else if (!m_apiHasUpdate && m_aptHasUpdate) {
        // Regular update: show current version, generic changelog string.
        m_newVersion    = readCurrentVersion();
        m_changelogText = tr("Regular software update");
        m_releaseDate.clear();
        setState(SoftwareUpdate);
    } else {
        setState(UpToDate);
    }

    emitInfoChanged();
}

// ── upgrade ───────────────────────────────────────────────────────────────────

void UpdateManager::runAptUpgrade()
{
    // Run a real apt-get upgrade with root via pkexec.
    //
    // Progress reporting uses the APT Status-Fd protocol:
    //   -o APT::Status-Fd=1  — write status lines to stdout (fd 1)
    //   -o Dpkg::Progress=1  — enable dpkg progress reporting
    //
    // apt writes one line per event:
    //   dlstatus:<pkg>:<0-100>:<msg>   download phase
    //   pmstatus:<pkg>:<0-100>:<msg>   install/configure phase
    //
    // We map download→0-50 and install→50-100 so the UI sees a
    // continuous 0-100 range across both phases.
    m_process = std::make_unique<Process>();
    m_process->setOomProtectionDropped(true);
    m_process->setEnvironment({"LANG=C", "LANGUAGE=C", "LC_ALL=C",
                               "DEBIAN_FRONTEND=noninteractive"});

    // Accumulate partial lines across chunks.
    auto lineBuf = std::make_shared<std::string>();
    m_process->onStandardOutput = [this, lineBuf](std::string_view chunk) {
        lineBuf->append(chunk);
        std::string::size_type start = 0;
        std::string::size_type nl;
        while ((nl = lineBuf->find('\n', start)) != std::string::npos) {
            const std::string line = lineBuf->substr(start, nl - start);
            start = nl + 1;
            int phase = 0, percent = 0;
            if (parseAptStatusFd(line, phase, percent)) {
                if (phase != m_upgradePhase || percent != m_upgradeProgress) {
                    m_upgradePhase    = phase;
                    m_upgradeProgress = percent;
                    if (m_object)
                        m_object->emitSignal("upgradeProgress",
                                             int32_t(phase), int32_t(percent));
                }
            }
        }
        lineBuf->erase(0, start);
    };

    m_process->onFinished = [this](int exitCode, bool crashed) {
        // Collect stderr for the error log before resetting the process.
        const std::string errOutput = m_process->standardError();
        m_process.reset();
        if (crashed || exitCode != 0) {
            LM_WARNING(services::log())
                    << "apt-get upgrade failed (exit" << exitCode << ")";
            m_errorLog        = errOutput.empty()
                                    ? format(tr("apt-get upgrade exited with code {code}"), {{"code", std::to_string(exitCode)}})
                                    : errOutput;
            m_upgradePhase    = 0;
            m_upgradeProgress = -1;
            if (m_object)
                m_object->emitSignal("upgradeProgress",
                                     int32_t(m_upgradePhase), int32_t(m_upgradeProgress));
            failOperation(m_errorLog);
        } else {
            m_errorLog.clear();
            // Finish the install phase at 100%.
            m_upgradePhase    = 2;
            m_upgradeProgress = 100;
            if (m_object)
                m_object->emitSignal("upgradeProgress",
                                     int32_t(m_upgradePhase), int32_t(m_upgradeProgress));
            m_upgradePhase    = 0;
            m_upgradeProgress = -1;
            setState(UpToDate);
        }
    };

    const bool started = m_process->start(
            "pkexec",
            {"/usr/bin/apt-get", "upgrade", "-y",
             "-o", "APT::Status-Fd=1",
             "-o", "Dpkg::Progress=1"});

    if (!started) {
        LM_WARNING(services::log()) << "Cannot start pkexec apt-get upgrade";
        m_process.reset();
        failOperation(tr("Cannot start pkexec to install updates. Check polkit installation."));
    }
}

// ── helpers ───────────────────────────────────────────────────────────────────

void UpdateManager::loadSettings()
{
    const int ch = int(m_settings.intValue(s_channelKey, int64_t(Stable)));
    m_channel = (ch >= Stable && ch <= Nightly) ? static_cast<Channel>(ch) : Stable;
    m_lastCheckTime = m_settings.intValue(s_lastCheckTimeKey, 0);
}

void UpdateManager::saveChannel()
{
    m_settings.setValue(s_channelKey, int64_t(m_channel));
    m_settings.sync();
}

void UpdateManager::saveLastCheckTime()
{
    m_settings.setValue(s_lastCheckTimeKey, m_lastCheckTime);
    m_settings.sync();
}

std::string UpdateManager::machineArchitecture()
{
    struct utsname buf {};
    if (::uname(&buf) != 0)
        return "x86_64";
    const std::string machine = buf.machine;
    if (machine == "aarch64")     return "arm64";
    if (machine == "loongarch64") return "loongarch";
    return "x86_64";
}

int64_t UpdateManager::readLocalBuild()
{
    // /etc/lingmo-release written by the installer:
    //   VERSION=2.0.26
    //   BUILD=2026070101
    std::ifstream file("/etc/lingmo-release");
    if (!file.is_open())
        return 0;
    std::string line;
    while (std::getline(file, line)) {
        if (line.rfind("BUILD=", 0) == 0) {
            try { return std::stoll(line.substr(6)); } catch (...) { return 0; }
        }
    }
    return 0;
}

std::string UpdateManager::readCurrentVersion()
{
    std::ifstream file("/etc/lingmo-release");
    if (!file.is_open())
        return {};
    std::string line;
    while (std::getline(file, line)) {
        if (line.rfind("VERSION=", 0) == 0)
            return line.substr(8);
    }
    return {};
}

std::string UpdateManager::apiUrl() const
{
    return std::string(s_apiBase)
         + "?channel=" + channelName(m_channel)
         + "&arch="    + m_arch;
}

void UpdateManager::failOperation(const std::string &message)
{
    m_errorLog = message;
    m_upgradePhase = 0;
    m_upgradeProgress = -1;
    LM_WARNING(services::log()) << m_errorLog;
    setState(Error);
    emitInfoChanged();
}

void UpdateManager::setState(State state)
{
    if (m_state == state)
        return;
    m_state = state;
    if (m_object)
        m_object->emitSignal("stateChanged", int32_t(state));
}

void UpdateManager::emitInfoChanged()
{
    if (!m_object)
        return;
    m_object->emitPropertiesChanged({"newVersion", "newBuild", "mandatory",
                                     "releaseNotes", "downloadUrl", "releaseDate",
                                     "changelogText", "upgradableCount",
                                     "upgradablePackages", "lastCheckTime",
                                     "errorLog"});
}
