
#include <map>
#include <string>
n#pragma once
#include <vector>

#include "base/settings.h"
#include "base/signal.h"
#include "base/thread_pool.h"

/**
 * The desktop background: the wallpaper in use, the wallpapers the user
 * brought in, and the folders they added as libraries. ThemeManager owns the
 * D-Bus interface and hands this part of it over here.
 *
 * Every wallpaper the user picks is copied into the session's own data
 * directory, so moving or deleting the original leaves the desktop alone.
 */
class WallpaperManager
{
public:
    explicit WallpaperManager(Lingmo::Base::Settings &settings);
    ~WallpaperManager();

    WallpaperManager(const WallpaperManager &) = delete;
    WallpaperManager &operator=(const WallpaperManager &) = delete;

    QString wallpaper() const;
    void setWallpaper(const QString &path);

    QStringList userWallpapers() const;
    void removeUserWallpaper(const QString &path);

    QStringList wallpaperFolders() const;
    void addWallpaperFolder(const QString &path);
    void removeWallpaperFolder(const QString &path);
    QStringList wallpaperFolderImages(const QString &path);

    int backgroundType() const;
    void setBackgroundType(int type);

    QString backgroundColor() const;
    void setBackgroundColor(const QString &color);

    Lingmo::Base::Signal<QString> wallpaperChanged;
    Lingmo::Base::Signal<> userWallpapersChanged;
    Lingmo::Base::Signal<> wallpaperFoldersChanged;
    Lingmo::Base::Signal<QString> wallpaperFolderImagesChanged;
    Lingmo::Base::Signal<> backgroundTypeChanged;
    Lingmo::Base::Signal<> backgroundColorChanged;

private:
    template<typename T>
    void write(const QString &key, const T &value);
    void syncWallpaperFolder(const QString &folder);
    void onWallpaperFolderSynced(const QString &folder, bool copied);

    Lingmo::Base::Settings &m_settings;

    QString m_wallpaperPath;
    QStringList m_userWallpapers;
    QStringList m_wallpaperFolders;
    // The copies being made, by folder.
    std::map<QString, Lingmo::Base::Job> m_syncingFolders;

    int m_backgroundType = 0;
    QString m_backgroundColor;
};

