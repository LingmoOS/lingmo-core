/*
 * SPDX-FileCopyrightText: 2025 Elysia <elysia@lingmo.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "hotkey_manager.h"
#include "keycode_helper.h"

#include <thread>
#include <iostream>

#include <libinput.h>

#include <QCoreApplication>
#include <qnamespace.h>

GlobalHotkeyManager::GlobalHotkeyManager(QObject* parent)
    : QObject(parent)
    , _thread_pool { new QThreadPool(this) }
{
    initLibinput();
}

GlobalHotkeyManager::~GlobalHotkeyManager()
{
    // Wait for all threads to finish
    stopListeningForEvents(); // Ensure thread is stopped before destruction
}

void GlobalHotkeyManager::initLibinput()
{
    // Define the interface as const
    const struct libinput_interface libinput_interface = {
        [](const char* path, int flags, void* user_data) -> int {
            return open(path, flags);
        },
        [](int fd, void* user_data) -> void {
            close(fd);
        }
    };

    // Create libinput context
    libinput_ = libinput_path_create_context(&libinput_interface, nullptr);
    if (!libinput_) {
        cerr << "Failed to create libinput context." << endl;
        exit(1);
    }

    // Automatically add keyboard devices
    addKeyboardDevices();

    // Clear previous state.
    _should_exit = false;
    _should_restart = false;
}

bool GlobalHotkeyManager::bindShortcut(const string& shortcutId, const unordered_set<int>& keyCombination, function<void()> callback, const QString& description)
{
    _shortcuts_mutex.lock();
    // Check for duplicate shortcutId
    if (shortcuts_.find(shortcutId) != shortcuts_.end()) {
        cerr << "Duplicate shortcutId: " << shortcutId << endl;
        _shortcuts_mutex.unlock();
        return false;
    } else {
        // Check for duplicate keyCombination
        for (const auto& [existingShortcutId, existingShortcut] : shortcuts_) {
            if (existingShortcut.keys == keyCombination) {
                cerr << "Duplicate keyCombination: Pending shortcutId: " << shortcutId << " conflicts with shortcutId: " << existingShortcutId << endl;
                _shortcuts_mutex.unlock();
                return false;
            }
        }
    }
    shortcuts_[shortcutId] = { keyCombination, callback, {}, description };
    _shortcuts_mutex.unlock();

    return true;
}

bool GlobalHotkeyManager::bindShortcut(const string& shortcutId, const Lingmo::HotKey::NativeShortcut& keyCombination, function<void()> callback, const QString& description)
{
    unordered_set<int> keys = unordered_set<int> { keyCombination.key };
    keys.insert(keyCombination.modifier.begin(), keyCombination.modifier.end());
    return bindShortcut(shortcutId, keys, callback, description);
}

void GlobalHotkeyManager::handleKeyEvent(struct libinput_event_keyboard* keyboardEvent)
{
    int keyCode = libinput_event_keyboard_get_key(keyboardEvent);
    int keyState = libinput_event_keyboard_get_key_state(keyboardEvent);
    _shortcuts_mutex.lock();
    for (auto& [shortcutId, shortcut] : shortcuts_) {
        if (find(shortcut.keys.begin(), shortcut.keys.end(), keyCode) != shortcut.keys.end()) {
            if (keyState == LIBINPUT_KEY_STATE_PRESSED) {
                shortcut.pressedKeys.insert(keyCode);
                if (shortcut.pressedKeys == shortcut.keys) {
                    activeShortcuts_.insert(shortcutId);
                    // Execute callback in the main thread
                    QMetaObject::invokeMethod(this, shortcut.callback, Qt::QueuedConnection);
                }
            } else if (keyState == LIBINPUT_KEY_STATE_RELEASED) {
                shortcut.pressedKeys.erase(keyCode);
                activeShortcuts_.erase(shortcutId);
            }
        }
    }
    _shortcuts_mutex.unlock();
}

void GlobalHotkeyManager::recreateLibinputContext()
{
    devices_list_.clear();
    _is_listening = false;

    this->stopListeningForEvents();
    this->initLibinput();
    libinput_ref(libinput_);

    _is_listening = true;
}

void GlobalHotkeyManager::listenForEvents()
{
    _is_listening = true;
    // Add a ref count to the libinput context
    // this will be released in deconstructor
    libinput_ref(libinput_);
    while (true) {
        if (_should_exit) {
            _is_listening = false;
            return;
        }

        if (_should_restart || checkKeyboardRemove()) {
            // Need update keyboard device
            // restart libinput context in order to do so
            _should_check_keyboard = false;
            _should_restart_mutex.lock();
            recreateLibinputContext();
            _should_restart = false;
            _should_check_keyboard = true;
            _should_restart_mutex.unlock();
            continue;
        }

        struct libinput_event* event = libinput_get_event(libinput_);

        if (event == nullptr) {
            libinput_dispatch(libinput_);
            continue;
        }

        if (libinput_event_get_type(event) == LIBINPUT_EVENT_KEYBOARD_KEY) {
            handleKeyEvent(libinput_event_get_keyboard_event(event));
        }

        libinput_event_destroy(event);
        libinput_dispatch(libinput_);
    }
}

void GlobalHotkeyManager::addKeyboardDevices()
{
    // Use a standalone libinput context for this task
    // otherwise the main libinput context will not be available
    // after calling unref()
    struct libinput* li;
    const static struct libinput_interface interface = {
        [](const char* path, int flags, void* user_data) -> int {
            return open(path, flags);
        },
        [](int fd, void* user_data) -> void {
            close(fd);
        }
    };

    li = libinput_path_create_context(&interface, NULL);

    libinput_ref(li);

    // 尝试打开 /dev/input/ 目录，该目录通常包含输入设备文件
    DIR* dir = opendir("/dev/input/");
    if (!dir) {
        cerr << "Failed to open /dev/input/ directory." << endl;
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        string name = entry->d_name;
        if (name.substr(0, 5) == "event") { // Check if it's an event device
            string devicePath = "/dev/input/" + name;
            struct libinput_device* device = libinput_path_add_device(li, devicePath.c_str());
            if (device && libinput_device_has_capability(device, LIBINPUT_DEVICE_CAP_KEYBOARD)) {
                cout << "Added device: " << devicePath << endl;
                libinput_path_add_device(libinput_, devicePath.c_str());
                devices_list_.push_back({ devicePath, libinput_device_ref(device) });
            } else {
                cout << "Cannot check device: " << devicePath << ". Do you have permission?" << endl;
            }
        }
    }

    libinput_unref(li);

    closedir(dir);
}

bool GlobalHotkeyManager::checkKeyboardRemove()
{
    // check and Remove/add keyboard devices from the libinput context.
    // This is necessary because the libinput context does not automatically
    // detect devices that are added or removed.
    for (const auto& [devicePath, device] : devices_list_) {
        if (access(devicePath.c_str(), R_OK) != 0) {
            return true;
        }
    }
    return false;
}

void GlobalHotkeyManager::checkKeyboardAdd()
{
    while (true) {
        if(!_should_check_keyboard)
            continue;

        _should_restart_mutex.lock();

        // Use a standalone libinput context for this task
        // otherwise the main libinput context will not be available
        // after calling unref()
        struct libinput* li;
        const static struct libinput_interface interface = {
            [](const char* path, int flags, void* user_data) -> int {
                return open(path, flags);
            },
            [](int fd, void* user_data) -> void {
                close(fd);
            }
        };

        li = libinput_path_create_context(&interface, NULL);

        libinput_ref(li);

        // 尝试打开 /dev/input/ 目录，该目录通常包含输入设备文件
        DIR* dir = opendir("/dev/input/");
        if (!dir) {
            cerr << "Failed to open /dev/input/ directory." << endl;
            exit(1);
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            string name = entry->d_name;
            if (name.substr(0, 5) == "event") { // Check if it's an event device
                string devicePath = "/dev/input/" + name;
                struct libinput_device* device = libinput_path_add_device(li, devicePath.c_str());
                if (device && libinput_device_has_capability(device, LIBINPUT_DEVICE_CAP_KEYBOARD)) {
                    // Test if the device is in the list
                    bool found = false;
                    for (const auto& [path, dev] : devices_list_) {
                        if (devicePath == path) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        _should_restart = true;
                    }
                }
            }
        }

        libinput_unref(li);

        closedir(dir);

        _should_restart_mutex.unlock();

        // Sleep 1s
        // std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void GlobalHotkeyManager::startListeningForEvents()
{
    _should_exit = false;
    _should_check_keyboard = true;
    _worker_thread = std::thread(&GlobalHotkeyManager::listenForEvents, this);
    _device_checker_thread = std::thread(&GlobalHotkeyManager::checkKeyboardAdd, this);
}

void GlobalHotkeyManager::stopListeningForEvents()
{
    cout << "Stopping listening for events..." << endl;
    if (_is_listening) {
        _should_exit = true;

        while (true) {
            if (!_is_listening) {
                break;
            }
        }

        // Join the worker thread
        if (_worker_thread.joinable()) {
            _worker_thread.join();
        }
    }

    _thread_pool->waitForDone(0);
}
