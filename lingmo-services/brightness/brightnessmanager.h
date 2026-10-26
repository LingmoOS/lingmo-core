
#include "ddcworker.h"

#pragma once
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "base/dbus.h"
#include "base/timer.h"

class BrightnessManager
{
public:
    explicit BrightnessManager(Lingmo::Base::DBus::Connection *session,
                               Lingmo::Base::DBus::Connection *system);
    ~BrightnessManager();

    BrightnessManager(const BrightnessManager &) = delete;
    BrightnessManager &operator=(const BrightnessManager &) = delete;

    // org.lingmo.Services.Brightness at /org/lingmo/Services/Brightness.
    bool publish();

    // Legacy single-display API; acts on the internal panel, else the first adjustable display.
    int maxBrightness();
    int brightness();
    bool brightnessEnabled();
    void setValue(int value);

    // Displays are sysfs DRM connector names, e.g. "card0-eDP-1". Values are percentages.
    QStringList displays() const;
    int displayBrightness(const QString &display) const;
    void setDisplayBrightness(const QString &display, int value);

private:
    enum class Kind {
        Backlight,
        Ddc
    };

    struct Display {
        QString connector;
        Kind kind = Kind::Backlight;
        QString device; // backlight name or /dev/i2c-N
        int maximum = 0;
        int raw = 0;
    };

    void rescan();
    QList<Display> scanBacklights(const QStringList &connected) const;
    QList<DdcDisplay> ddcCandidates(const QStringList &connected,
                                          const QList<Display> &backlights) const;
    void publish(QList<Display> displays);
    void readBacklight(Display &display) const;
    Display *find(const QString &display);
    const Display *find(const QString &display) const;
    int percent(const Display &display) const;
    void notifyIfChanged(const Display &display, int oldPercent);
    void emitBrightnessChanged();
    void emitDisplayBrightnessChanged(const QString &display, int value);

    // The brightness files of the backlights in use, through inotify.
    void watchFiles();
    void readInotify();
    void handleFileChanged(const QString &path);
    void handleDdcProbed(uint64_t generation, const QList<DdcDisplay> &displays);

    Lingmo::Base::DBus::Connection *m_session;
    Lingmo::Base::DBus::Connection *m_system;
    std::unique_ptr<Lingmo::Base::DBus::Object> m_object;
    Lingmo::Base::DBus::Match m_displayStateChanged;
    int m_inotify = -1;
    int m_inotifyWatch = 0;
    // Watch descriptors by file.
    std::map<QString, int> m_watches;
    Lingmo::Base::Timer m_rescanTimer;
    DdcWorker m_ddcWorker;
    uint64_t m_generation = 0;
    QList<Display> m_displays;
};

#endif
