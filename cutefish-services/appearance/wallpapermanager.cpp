#include "wallpapermanager.h"

#include <algorithm>
#include <filesystem>
#include <set>

#include "base/file.h"
#include "base/hash.h"
#include "base/paths.h"
#include "base/strings.h"

using namespace Lingmo::Base;
using namespace Lingmo::Base::strings;
namespace fs = std::filesystem;

// An absolute path, "." and ".." resolved by name, no trailing separator.
static QString absolutePath(const QString &path)
{
    std::error_code error;
    QString result = fs::absolute(path, error).lexically_normal().string();
    while (result.size() > 1 && result.back() == '/')
        result.pop_back();
    return result;
}

static bool isFile(const QString &path)
{
    std::error_code error;
    return fs::is_regular_file(path, error);
}

static bool isDirectory(const QString &path)
{
    std::error_code error;
    return fs::is_directory(path, error);
}

static bool exists(const QString &path)
{
    std::error_code error;
    return fs::exists(path, error);
}

static void removeRecursively(const QString &path)
{
    std::error_code error;
    fs::remove_all(path, error);
}

static bool contains(const QStringList &list, const QString &value)
{
    return std::find(list.begin(), list.end(), value) != list.end();
}

static void removeAll(QStringList &list, const QString &value)
{
    std::erase(list, value);
}

static QStringList systemWallpaperDirectories()
{
    QStringList directories;
    for (const QString &dataPath : paths::dataDirectories())
        directories.push_back(absolutePath(dataPath) + "/backgrounds/lingmoos");
    return directories;
}

static bool isSystemWallpaper(const QString &path)
{
    const QString wallpaperPath = absolutePath(path);
    for (const QString &directory : systemWallpaperDirectories()) {
        if (startsWith(wallpaperPath, directory + '/'))
            return true;
    }

    return false;
}

static QString userWallpaperDirectory()
{
    return paths::dataHome() + "/backgrounds/lingmo/user";
}

static bool isManagedUserWallpaper(const QString &path)
{
    return startsWith(absolutePath(path), userWallpaperDirectory() + '/');
}

// A folder the user added is mirrored here, one directory per folder: the
// originals may be moved or deleted, and the wallpapers have to survive that.
static QString wallpaperFolderRoot()
{
    return paths::dataHome() + "/backgrounds/lingmo/folders";
}

static QString wallpaperFolderStore(const QString &folder)
{
    const QString folderHash =
            CryptographicHash::hashHex(absolutePath(folder), CryptographicHash::Algorithm::Sha256).substr(0, 16);
    return wallpaperFolderRoot() + '/' + folderHash;
}

static bool isManagedFolderWallpaper(const QString &path)
{
    return startsWith(absolutePath(path), wallpaperFolderRoot() + '/');
}

static QStringList imagesIn(const QString &directory)
{
    static const char *const suffixes[] = {".jpg", ".jpeg", ".png", ".webp", ".bmp", ".tif", ".tiff", ".avif"};

    QStringList result;
    std::error_code error;
    for (fs::recursive_directory_iterator it(directory, error), end; !error && it != end; it.increment(error)) {
        std::error_code typeError;
        if (!it->is_regular_file(typeError))
            continue;
        const QString name = it->path().filename().string();
        const bool image = std::any_of(std::begin(suffixes), std::end(suffixes), [&name](const char *suffix) {
            return name.size() > QStringView(suffix).size()
                    && endsWith(name, suffix, CaseSensitivity::Insensitive);
        });
        if (image)
            result.push_back(it->path().string());
    }

    return result;
}

// The copy's name carries a hash of the source path, so the same source always
// maps to the same copy and a renamed one to a new copy.
static QString mirroredFileName(const QString &source)
{
    const QString sourcePath = absolutePath(source);
    const QString pathHash =
            CryptographicHash::hashHex(sourcePath, CryptographicHash::Algorithm::Sha256).substr(0, 12);
    const QString name = fs::path(sourcePath).filename().string();
    const size_t dot = name.rfind('.');
    const QString baseName = dot == QString::npos ? name : name.substr(0, dot);
    const QString suffix = dot == QString::npos ? QString() : name.substr(dot + 1);
    QString fileName = baseName + '-' + pathHash;
    if (!suffix.empty())
        fileName += '.' + toAsciiLower(suffix);
    return fileName;
}

static QString copyWallpaperInto(const QString &path, const QString &destinationDirectory)
{
    if (!isFile(path))
        return path;

    const QString sourcePath = absolutePath(path);
    if (startsWith(sourcePath, destinationDirectory + '/'))
        return sourcePath;

    std::error_code error;
    fs::create_directories(destinationDirectory, error);
    if (!isDirectory(destinationDirectory))
        return path;

    const QString destinationPath = destinationDirectory + '/' + mirroredFileName(sourcePath);
    if (exists(destinationPath))
        return destinationPath;

    const std::optional<QString> contents = file::read(sourcePath);
    if (!contents)
        return path;

    return file::writeAtomically(destinationPath, *contents) ? destinationPath : path;
}

// A wallpaper that ships with the system is already where it belongs.
static QString importUserWallpaper(const QString &path)
{
    if (isSystemWallpaper(path))
        return path;

    return copyWallpaperInto(path, userWallpaperDirectory());
}

WallpaperManager::WallpaperManager(Settings &settings)
    : m_settings(settings)
{
    m_wallpaperPath = m_settings.stringValue("Appearance/Wallpaper", "/usr/share/backgrounds/lingmoos/default.jpg");
    // A folder stays in the list even when its source is gone: what the list
    // shows is the copy taken when the folder was added.
    for (const QString &folder : m_settings.stringListValue("Appearance/WallpaperFolders")) {
        if (folder.empty())
            continue;
        const QString folderPath = absolutePath(folder);
        if (!folderPath.empty() && !contains(m_wallpaperFolders, folderPath))
            m_wallpaperFolders.push_back(folderPath);
    }
    m_settings.setValue("Appearance/WallpaperFolders", m_wallpaperFolders);

    for (const QString &path : m_settings.stringListValue("Appearance/UserWallpapers")) {
        if (!exists(path))
            continue;

        const QString importedPath = importUserWallpaper(path);
        if (isManagedUserWallpaper(importedPath))
            m_userWallpapers.push_back(importedPath);
    }
    if (exists(m_wallpaperPath) && !isSystemWallpaper(m_wallpaperPath)
        && !isManagedFolderWallpaper(m_wallpaperPath)) {
        m_wallpaperPath = importUserWallpaper(m_wallpaperPath);
        m_settings.setValue("Appearance/Wallpaper", m_wallpaperPath);
        if (isManagedUserWallpaper(m_wallpaperPath) && !contains(m_userWallpapers, m_wallpaperPath))
            m_userWallpapers.insert(m_userWallpapers.begin(), m_wallpaperPath);
    }
    QStringList unique;
    for (const QString &path : m_userWallpapers) {
        if (!contains(unique, path))
            unique.push_back(path);
    }
    m_userWallpapers = std::move(unique);
    m_settings.setValue("Appearance/UserWallpapers", m_userWallpapers);
    m_settings.sync();
    m_backgroundType = int(m_settings.intValue("Appearance/BackgroundType", 0));
    m_backgroundColor = m_settings.stringValue("Appearance/BackgroundColor", "#2B8ADA");
}

WallpaperManager::~WallpaperManager() = default;

template<typename T>
void WallpaperManager::write(const QString &key, const T &value)
{
    m_settings.setValue("Appearance/" + key, value);
    m_settings.sync();
}

QString WallpaperManager::wallpaper() const
{
    return m_wallpaperPath;
}

void WallpaperManager::setWallpaper(const QString &path)
{
    const QString wallpaperPath =
            isManagedFolderWallpaper(path) ? absolutePath(path) : importUserWallpaper(path);
    if (exists(wallpaperPath) && isManagedUserWallpaper(wallpaperPath)
        && !contains(m_userWallpapers, wallpaperPath)) {
        m_userWallpapers.insert(m_userWallpapers.begin(), wallpaperPath);
        write("UserWallpapers", m_userWallpapers);
        userWallpapersChanged.emit();
    }

    if (m_wallpaperPath != wallpaperPath) {
        m_wallpaperPath = wallpaperPath;
        write("Wallpaper", wallpaperPath);
        wallpaperChanged.emit(wallpaperPath);
    }
}

QStringList WallpaperManager::userWallpapers() const
{
    return m_userWallpapers;
}

void WallpaperManager::removeUserWallpaper(const QString &path)
{
    if (!isManagedUserWallpaper(path) || !contains(m_userWallpapers, path))
        return;

    std::error_code error;
    if (exists(path) && !fs::remove(path, error))
        return;

    removeAll(m_userWallpapers, path);
    write("UserWallpapers", m_userWallpapers);

    if (path != m_wallpaperPath) {
        userWallpapersChanged.emit();
        return;
    }

    QString replacement;
    for (const QString &candidate : m_userWallpapers) {
        if (exists(candidate)) {
            replacement = candidate;
            break;
        }
    }

    if (replacement.empty()) {
        for (const QString &directory : systemWallpaperDirectories()) {
            const QStringList wallpapers = imagesIn(directory);
            if (!wallpapers.empty()) {
                replacement = wallpapers.front();
                break;
            }
        }
    }

    m_wallpaperPath = replacement;
    write("Wallpaper", replacement);
    wallpaperChanged.emit(replacement);

    if (replacement.empty())
        setBackgroundType(1);

    userWallpapersChanged.emit();
}

QStringList WallpaperManager::wallpaperFolders() const
{
    return m_wallpaperFolders;
}

void WallpaperManager::addWallpaperFolder(const QString &path)
{
    const QString folderPath = absolutePath(path);
    if (!isDirectory(folderPath) || contains(m_wallpaperFolders, folderPath))
        return;

    m_wallpaperFolders.push_back(folderPath);
    write("WallpaperFolders", m_wallpaperFolders);
    syncWallpaperFolder(folderPath);
    wallpaperFoldersChanged.emit();
}

void WallpaperManager::removeWallpaperFolder(const QString &path)
{
    const QString folderPath = absolutePath(path);
    if (!contains(m_wallpaperFolders, folderPath))
        return;

    // The folder's copies go with it, so a wallpaper still in use is kept by
    // moving it over to the user's own wallpapers first.
    const QString store = wallpaperFolderStore(folderPath);
    if (startsWith(m_wallpaperPath, store + '/')) {
        const QString importedPath = importUserWallpaper(m_wallpaperPath);
        if (isManagedUserWallpaper(importedPath)) {
            m_wallpaperPath = importedPath;
            write("Wallpaper", importedPath);
            if (!contains(m_userWallpapers, importedPath)) {
                m_userWallpapers.insert(m_userWallpapers.begin(), importedPath);
                write("UserWallpapers", m_userWallpapers);
                userWallpapersChanged.emit();
            }
            wallpaperChanged.emit(importedPath);
        }
    }

    removeRecursively(store);
    removeAll(m_wallpaperFolders, folderPath);
    write("WallpaperFolders", m_wallpaperFolders);
    wallpaperFoldersChanged.emit();
}

// The copies are what the folder shows; the source is read again on every
// query so that images added to it later turn up here too. Copying a folder
// takes long enough to be worth a thread: the caller is told to ask again
// once the copies are on disk.
void WallpaperManager::syncWallpaperFolder(const QString &folder)
{
    if (m_syncingFolders.contains(folder))
        return;

    const QString wallpaperInUse = m_wallpaperPath;
    m_syncingFolders[folder] = runInThread(
            [folder, wallpaperInUse] {
                const QString store = wallpaperFolderStore(folder);
                QStringList before = imagesIn(store);
                std::sort(before.begin(), before.end());
                std::set<QString> expected;
                for (const QString &source : imagesIn(folder)) {
                    copyWallpaperInto(source, store);
                    expected.insert(mirroredFileName(source));
                }
                // A copy whose source was renamed, replaced or removed has no use, unless it is the
                // wallpaper in use; a source folder that is gone as a whole keeps its copies.
                if (isDirectory(folder)) {
                    for (const QString &copy : imagesIn(store)) {
                        std::error_code error;
                        if (copy != wallpaperInUse && !expected.contains(fs::path(copy).filename().string()))
                            fs::remove(copy, error);
                    }
                }
                QStringList after = imagesIn(store);
                std::sort(after.begin(), after.end());
                return after != before;
            },
            [this, folder](bool copied) {
                onWallpaperFolderSynced(folder, copied);
            });
}

void WallpaperManager::onWallpaperFolderSynced(const QString &folder, bool copied)
{
    m_syncingFolders.erase(folder);

    // The folder was removed while its copies were still being made.
    if (!contains(m_wallpaperFolders, folder)) {
        removeRecursively(wallpaperFolderStore(folder));
        return;
    }

    if (copied)
        wallpaperFolderImagesChanged.emit(folder);
}

QStringList WallpaperManager::wallpaperFolderImages(const QString &path)
{
    const QString folderPath = absolutePath(path);
    if (!contains(m_wallpaperFolders, folderPath))
        return {};

    syncWallpaperFolder(folderPath);
    return imagesIn(wallpaperFolderStore(folderPath));
}

int WallpaperManager::backgroundType() const
{
    return m_backgroundType;
}

void WallpaperManager::setBackgroundType(int type)
{
    if (m_backgroundType != type) {
        m_backgroundType = type;
        write("BackgroundType", m_backgroundType);
        backgroundTypeChanged.emit();
    }
}

QString WallpaperManager::backgroundColor() const
{
    return m_backgroundColor;
}

void WallpaperManager::setBackgroundColor(const QString &color)
{
    if (m_backgroundColor != color) {
        m_backgroundColor = color;
        write("BackgroundColor", m_backgroundColor);
        backgroundColorChanged.emit();
    }
}
