// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-FileCopyrightText: 2026 LingmoOS Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "fontconfigwriter.h"

#include <memory>
#include <string>
#include <vector>

#include "base/dbus.h"
#include "base/settings.h"
#include "base/signal.h"

class LoginBackground;
class ScheduledFeature;
class WallpaperManager;

class ThemeManager
{
public:
    ThemeManager(Lingmo::Base::Settings &settings, Lingmo::Base::DBus::Connection *session,
                 Lingmo::Base::DBus::Connection *system);
    ~ThemeManager();

    ThemeManager(const ThemeManager &) = delete;
    ThemeManager &operator=(const ThemeManager &) = delete;

    // org.lingmo.Services.Appearance at /org/lingmo/Services/Appearance.
    bool publish();

    bool isDarkMode();
    // A manual switch holds until the schedule's next turn.
    void setDarkMode(bool darkMode);

    // ScheduledFeature::Schedule, without Custom: dark from sunset to sunrise.
    int darkModeSchedule() const;
    void setDarkModeSchedule(int schedule);

    bool continuousCorners() const;
    void setContinuousCorners(bool value);

    QString systemFont();
    void setSystemFont(const QString &fontFamily);

    QString systemFixedFont();
    void setSystemFixedFont(const QString &fontFamily);

    double systemFontPointSize();
    void setSystemFontPointSize(double fontSize);

    QString wallpaper();
    void setWallpaper(const QString &path);
    QStringList userWallpapers() const;
    void removeUserWallpaper(const QString &path);

    QStringList wallpaperFolders() const;
    void addWallpaperFolder(const QString &path);
    void removeWallpaperFolder(const QString &path);
    QStringList wallpaperFolderImages(const QString &path);

    int backgroundType();
    void setBackgroundType(int type);

    QString backgroundColor();
    void setBackgroundColor(const QString &color);

    int accentColor();
    void setAccentColor(int accentColor);

    QString cursorTheme() const;
    void setCursorTheme(const QString &theme);

    int cursorSize() const;
    void setCursorSize(int size);

    void updateGtk3Config();
    void setFontRendering(bool antialias, const QString &hintStyle);
    void applyCursorSettings();

    QString iconTheme() const;
    void setIconTheme(const QString &iconTheme);

    void updateFontConfig();

    // After any change the interface announces.
    Lingmo::Base::Signal<> changed;

private:
    FontConfig fontConfig();
    template<typename... Args>
    void emitSignal(const QString &name, const Args &...args);
    template<typename T>
    void write(const QString &key, const T &value);

    Lingmo::Base::Settings &m_settings;
    Lingmo::Base::DBus::Connection *m_session;
    std::unique_ptr<WallpaperManager> m_wallpaperManager;
    std::unique_ptr<LoginBackground> m_loginBackground;
    std::unique_ptr<ScheduledFeature> m_darkMode;
    std::unique_ptr<Lingmo::Base::DBus::Object> m_object;
    QList<Lingmo::Base::Subscription> m_subscriptions;

    bool m_continuousCorners = true;
    int m_accentColor = 0;

    QString m_cursorTheme;
    int m_cursorSize = 24;

    QString m_iconTheme;
};

