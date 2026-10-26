#include "brightnessmanager.h"

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <filesystem>
#include <sys/inotify.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "base/event_loop.h"
#include "base/file.h"
#include "base/strings.h"
#include "dbuscall.h"
#include "log.h"

using namespace std::chrono_literals;
using namespace Lingmo::Base;
using namespace Lingmo::Base::strings;
namespace fs = std::filesystem;

static const QString BacklightDir = "/sys/class/backlight/";
static const QString DrmDir = "/sys/class/drm/";

// Writers that replace the file drop the watch; IN_IGNORED says so.
static constexpr uint32_t kWatchMask = IN_MODIFY | IN_CLOSE_WRITE | IN_ATTRIB | IN_MOVE_SELF | IN_DELETE_SELF;

static QString readSysfs(const QString &path)
{
    return QString(trimmed(file::read(path).value_or(QString())));
}

static int toInt(const QString &text)
{
    return toNumber<int>(text).value_or(0);
}

static QString brightnessFile(const QString &backlight)
{
    return BacklightDir + backlight + "/brightness";
}

// The entries of a directory that are directories or links to them, sorted by name.
static QStringList subdirectories(const QString &directory)
{
    QStringList names;
    std::error_code error;
    for (const fs::directory_entry &entry : fs::directory_iterator(directory, error)) {
        std::error_code typeError;
        if (entry.is_directory(typeError))
            names.push_back(entry.path().filename().string());
    }
    std::sort(names.begin(), names.end());
    return names;
}

// The name of what the link at the path resolves to; empty when it does not.
static QString canonicalName(const QString &path)
{
    std::error_code error;
    const fs::path target = fs::canonical(path, error);
    return error ? QString() : target.filename().string();
}

static bool exists(const QString &path)
{
    std::error_code error;
    return fs::exists(path, error);
}

// Connected DRM connectors, e.g. "card0-eDP-1".
static QStringList connectedConnectors()
{
    QStringList result;
    for (const QString &entry : subdirectories(DrmDir)) {
        if (!startsWith(entry, "card") || entry.find('-', 4) == QString::npos)
            continue;
        if (readSysfs(DrmDir + entry + "/status") == "connected")
            result.push_back(entry);
    }
    return result;
}

static bool isInternalConnector(const QString &connector)
{
    const QStringView type = section(connector, '-', 1, 1);
    return type == "eDP" || type == "LVDS" || type == "DSI";
}

static int typeRank(const QString &type)
{
    if (type == "firmware")
        return 0;
    if (type == "platform")
        return 1;
    return 2;
}

static bool contains(const QStringList &list, const QString &value)
{
    return std::find(list.begin(), list.end(), value) != list.end();
}

BrightnessManager::BrightnessManager(dbus::Connection *session, dbus::Connection *system)
    : m_session(session)
    , m_system(system)
    , m_ddcWorker([this](uint64_t generation, const QList<DdcDisplay> &displays) {
        handleDdcProbed(generation, displays);
    })
{
    m_inotify = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
    if (m_inotify >= 0) {
        m_inotifyWatch = EventLoop::current()->watch(m_inotify, EPOLLIN, [this](uint32_t) {
            readInotify();
        });
    }

    // Monitors need a moment after hotplug before they answer DDC/CI.
    m_rescanTimer.setSingleShot(true);
    m_rescanTimer.setInterval(1500ms);
    m_rescanTimer.onTimeout = [this] {
        rescan();
    };
    if (m_session) {
        dbus::MatchRule rule;
        rule.path = "/org/lingmo/DisplayConfig1";
        rule.interface = "org.lingmo.DisplayConfig1";
        rule.member = "StateChanged";
        m_displayStateChanged = m_session->subscribe(rule, [this](const dbus::Message &) {
            m_rescanTimer.start();
        });
    }

    rescan();
}

BrightnessManager::~BrightnessManager()
{
    if (m_inotifyWatch)
        EventLoop::current()->unwatch(m_inotifyWatch);
    if (m_inotify >= 0)
        ::close(m_inotify);
}

bool BrightnessManager::publish()
{
    if (!m_session)
        return false;
    m_object = std::make_unique<dbus::Object>(m_session, "/org/lingmo/Services/Brightness",
                                              "org.lingmo.Services.Brightness");
    dbus::Object &object = *m_object;
    object.addMethod("setValue", [this](int32_t value) { setValue(value); }, {{"value"}, {}});
    object.addMethod("displays", [this] { return displays(); });
    object.addMethod("displayBrightness",
                     [this](const QString &display) { return int32_t(displayBrightness(display)); },
                     {{"display"}, {"value"}});
    object.addMethod("setDisplayBrightness",
                     [this](const QString &display, int32_t value) { setDisplayBrightness(display, value); },
                     {{"display", "value"}, {}});
    object.addProperty("maxBrightness", [this] { return int32_t(maxBrightness()); });
    object.addProperty("brightness", [this] { return int32_t(brightness()); });
    object.addProperty("brightnessEnabled", [this] { return brightnessEnabled(); });
    object.addSignal("brightnessChanged", "");
    object.addSignal("displaysChanged", "");
    object.addSignal("displayBrightnessChanged", "si", {"display", "value"});
    return object.publish();
}

int BrightnessManager::maxBrightness()
{
    return m_displays.empty() ? -1 : m_displays.front().maximum;
}

int BrightnessManager::brightness()
{
    return m_displays.empty() ? -1 : percent(m_displays.front());
}

bool BrightnessManager::brightnessEnabled()
{
    return !m_displays.empty();
}

void BrightnessManager::setValue(int value)
{
    if (!m_displays.empty())
        setDisplayBrightness(m_displays.front().connector, value);
}

QStringList BrightnessManager::displays() const
{
    QStringList result;
    for (const Display &display : m_displays)
        result.push_back(display.connector);
    return result;
}

int BrightnessManager::displayBrightness(const QString &display) const
{
    const Display *entry = find(display);
    return entry ? percent(*entry) : -1;
}

void BrightnessManager::setDisplayBrightness(const QString &connector, int value)
{
    Display *display = find(connector);
    if (!display)
        return;

    int raw = int(std::lround(std::clamp(value, 0, 100) * display->maximum / 100.0));
    // Raw 0 switches some backlights off entirely.
    if (display->kind == Kind::Backlight)
        raw = std::max(1, raw);
    if (raw == display->raw)
        return;

    const int oldPercent = percent(*display);
    display->raw = raw;

    if (display->kind == Kind::Backlight) {
        if (m_system) {
            const dbus::Proxy session(m_system, "org.freedesktop.login1",
                                      "/org/freedesktop/login1/session/auto",
                                      "org.freedesktop.login1.Session");
            services::callDetached(
                    session, "SetBrightness",
                    [](const dbus::Reply &reply) {
                        if (reply.isError())
                            LM_WARNING(services::log())
                                    << "Setting backlight brightness failed:" << reply.error().message;
                    },
                    QString("backlight"), display->device, uint32_t(raw));
        }
    } else {
        m_ddcWorker.requestValue(display->device, raw);
    }

    notifyIfChanged(*display, oldPercent);
}

void BrightnessManager::rescan()
{
    ++m_generation;
    const QStringList connected = connectedConnectors();
    const QList<Display> backlights = scanBacklights(connected);

    QList<DdcDisplay> candidates = ddcCandidates(connected, backlights);

    // Keep known DDC displays until the probe answers so sliders do not flicker.
    QList<Display> displays = backlights;
    for (const Display &display : m_displays) {
        if (display.kind != Kind::Ddc)
            continue;
        for (const DdcDisplay &candidate : candidates) {
            if (candidate.connector == display.connector && candidate.device == display.device)
                displays.push_back(display);
        }
    }
    publish(std::move(displays));

    if (!candidates.empty())
        m_ddcWorker.probe(m_generation, std::move(candidates));
}

QList<BrightnessManager::Display>
BrightnessManager::scanBacklights(const QStringList &connected) const
{
    QStringList internal;
    for (const QString &connector : connected) {
        if (isInternalConnector(connector))
            internal.push_back(connector);
    }

    // Firmware and platform interfaces carry no connector link; they drive the internal panel.
    std::map<QString, std::pair<int, Display>> best;
    for (const QString &name : subdirectories(BacklightDir)) {
        const QString path = BacklightDir + name;
        Display display;
        display.kind = Kind::Backlight;
        display.device = name;
        display.maximum = toInt(readSysfs(path + "/max_brightness"));
        if (display.maximum <= 0)
            continue;

        const QString parent = canonicalName(path + "/device");
        const bool attached = !parent.empty() && exists(DrmDir + parent) && startsWith(parent, "card");
        if (attached)
            display.connector = parent;
        else if (!internal.empty())
            display.connector = internal.front();
        if (display.connector.empty() || !contains(connected, display.connector))
            continue;

        const int rank = typeRank(readSysfs(path + "/type")) * 2 + (attached ? 0 : 1);
        auto it = best.find(display.connector);
        if (it == best.end() || rank < it->second.first) {
            readBacklight(display);
            best.insert_or_assign(display.connector, std::make_pair(rank, display));
        }
    }

    QList<Display> result;
    for (const QString &connector : connected) {
        if (const auto it = best.find(connector); it != best.end())
            result.push_back(it->second.second);
    }
    return result;
}

QList<DdcDisplay> BrightnessManager::ddcCandidates(const QStringList &connected,
                                                         const QList<Display> &backlights) const
{
    QList<DdcDisplay> result;
    for (const QString &connector : connected) {
        if (isInternalConnector(connector))
            continue;
        const bool hasBacklight = std::any_of(backlights.cbegin(), backlights.cend(),
                                              [&](const Display &display) {
                                                  return display.connector == connector;
                                              });
        if (hasBacklight)
            continue;

        // HDMI/DVI/VGA link their DDC adapter; DP exposes an AUX-backed i2c child.
        const QString path = DrmDir + connector;
        QString bus = canonicalName(path + "/ddc");
        if (!startsWith(bus, "i2c-")) {
            bus.clear();
            for (const QString &child : subdirectories(path)) {
                if (startsWith(child, "i2c-")) {
                    bus = child;
                    break;
                }
            }
        }
        if (bus.empty() || !exists("/dev/" + bus))
            continue;

        DdcDisplay candidate;
        candidate.connector = connector;
        candidate.device = "/dev/" + bus;
        result.push_back(candidate);
    }
    return result;
}

void BrightnessManager::handleDdcProbed(uint64_t generation, const QList<DdcDisplay> &probed)
{
    if (generation != m_generation)
        return;

    // Backlight values may have changed since the scan; m_displays holds the latest.
    QList<Display> displays;
    for (const Display &display : m_displays) {
        if (display.kind == Kind::Backlight)
            displays.push_back(display);
    }
    for (const DdcDisplay &entry : probed) {
        Display display;
        display.connector = entry.connector;
        display.kind = Kind::Ddc;
        display.device = entry.device;
        display.maximum = entry.maximum;
        display.raw = entry.value;
        // A slider write may still be queued; its value is newer than the probe.
        if (const Display *known = find(entry.connector); known && known->device == entry.device)
            display.raw = known->raw;
        displays.push_back(display);
    }
    publish(std::move(displays));
}

void BrightnessManager::publish(QList<Display> displays)
{
    const QStringList oldConnectors = this->displays();
    const int oldDefault = brightness();
    const QList<Display> old = m_displays;
    m_displays = std::move(displays);

    watchFiles();

    if (this->displays() != oldConnectors && m_object)
        m_object->emitSignal("displaysChanged");

    for (const Display &display : m_displays) {
        const auto it = std::find_if(old.cbegin(), old.cend(), [&](const Display &entry) {
            return entry.connector == display.connector;
        });
        if (it == old.cend() || percent(*it) != percent(display))
            emitDisplayBrightnessChanged(display.connector, percent(display));
    }

    if (brightness() != oldDefault)
        emitBrightnessChanged();
}

void BrightnessManager::watchFiles()
{
    if (m_inotify < 0)
        return;

    std::map<QString, int> watches;
    for (const Display &display : m_displays) {
        if (display.kind != Kind::Backlight)
            continue;
        const QString path = brightnessFile(display.device);
        if (const auto it = m_watches.find(path); it != m_watches.end()) {
            watches.insert(*it);
            m_watches.erase(it);
            continue;
        }
        const int watch = inotify_add_watch(m_inotify, path.c_str(), kWatchMask);
        if (watch >= 0)
            watches.emplace(path, watch);
    }
    for (const auto &[path, watch] : m_watches)
        inotify_rm_watch(m_inotify, watch);
    m_watches = std::move(watches);
}

void BrightnessManager::readInotify()
{
    alignas(inotify_event) char buffer[4096];
    QStringList changed;
    for (;;) {
        const ssize_t length = ::read(m_inotify, buffer, sizeof(buffer));
        if (length <= 0)
            break;
        for (ssize_t offset = 0; offset < length;) {
            const auto *event = reinterpret_cast<const inotify_event *>(buffer + offset);
            offset += ssize_t(sizeof(inotify_event) + event->len);
            for (auto it = m_watches.begin(); it != m_watches.end(); ++it) {
                if (it->second != event->wd)
                    continue;
                // Some writers replace the file, which drops it from the watch.
                if (event->mask & IN_IGNORED) {
                    const int watch = inotify_add_watch(m_inotify, it->first.c_str(), kWatchMask);
                    if (watch >= 0)
                        it->second = watch;
                }
                if (std::find(changed.begin(), changed.end(), it->first) == changed.end())
                    changed.push_back(it->first);
                break;
            }
        }
    }
    for (const QString &path : changed)
        handleFileChanged(path);
}

void BrightnessManager::readBacklight(Display &display) const
{
    display.raw = toInt(readSysfs(brightnessFile(display.device)));
}

BrightnessManager::Display *BrightnessManager::find(const QString &display)
{
    for (Display &entry : m_displays) {
        if (entry.connector == display)
            return &entry;
    }
    return nullptr;
}

const BrightnessManager::Display *BrightnessManager::find(const QString &display) const
{
    return const_cast<BrightnessManager *>(this)->find(display);
}

int BrightnessManager::percent(const Display &display) const
{
    return display.maximum > 0 ? int(std::lround(display.raw * 100.0 / display.maximum)) : -1;
}

void BrightnessManager::notifyIfChanged(const Display &display, int oldPercent)
{
    const int newPercent = percent(display);
    if (newPercent == oldPercent)
        return;

    emitDisplayBrightnessChanged(display.connector, newPercent);
    if (&display == &m_displays.front())
        emitBrightnessChanged();
}

void BrightnessManager::emitBrightnessChanged()
{
    if (m_object)
        m_object->emitSignal("brightnessChanged");
}

void BrightnessManager::emitDisplayBrightnessChanged(const QString &display, int value)
{
    if (m_object)
        m_object->emitSignal("displayBrightnessChanged", display, int32_t(value));
}

void BrightnessManager::handleFileChanged(const QString &path)
{
    for (Display &display : m_displays) {
        if (display.kind != Kind::Backlight || brightnessFile(display.device) != path)
            continue;

        const int oldPercent = percent(display);
        readBacklight(display);
        notifyIfChanged(display, oldPercent);
        return;
    }
}
