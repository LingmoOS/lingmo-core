/*
 * SPDX-FileCopyrightText: 2025 Elysia <elysia@lingmo.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef HOTKEY_MANAGER_H
#define HOTKEY_MANAGER_H
// STL
#include <QMutex>
#include <algorithm>
#include <cstddef>
#include <functional>
#include <iostream>
#include <memory>
#include <qcontainerfwd.h>
#include <qlist.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

// libinput related
#include <dirent.h> // 用于目录操作
#include <fcntl.h>
#include <unistd.h>

#include <libinput.h>
#include <linux/input-event-codes.h>
#include <sys/stat.h>
#include <sys/types.h>

// Qt
#include <QMutex>
#include <QObject>
#include <QString>
#include <QThreadPool>

namespace Lingmo::HotKey {
class NativeShortcut;
}

using namespace std;

class GlobalHotkeyManager : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Custom data type to store shortcut binding information.
     *
     * @property keys: A set of key codes that make up the shortcut.
     * @property callback: A function to be called when the shortcut is activated.
     * @property pressedKeys: (internal) A set to store key codes that have been pressed for the current shortcut. C
     */
    struct ShortcutBinding {
        unordered_set<int> keys;
        function<void()> callback;
        unordered_set<int> pressedKeys;
        QString description;
    };

    explicit GlobalHotkeyManager(QObject* parent = nullptr);

    ~GlobalHotkeyManager() override;

    /**
     * 绑定快捷键到回调函数。
     * @param shortcut_id 快捷键的标识符。
     * @param keyCombination 构成快捷键的键码列表。
     * @param callback 当快捷键被激活时调用的函数。
     * @param description 快捷键的描述信息。
     */
    void bindShortcut(const string& shortcutId, const unordered_set<int>& keyCombination, function<void()> callback, const QString& description = "");
    
    void bindShortcut(const string& shortcutId, const Lingmo::HotKey::NativeShortcut& keyCombination, function<void()> callback, const QString& description = "");

    /**
     * @brief Start to listen for events from the libinput library.
     *
     */
    void listenForEvents();

    /**
     * @brief Start listenForEvents() in a new thread.
     *
     */
    void startListeningForEvents();

    /**
     * @brief Stop listening for events from the libinput library. This instance is NOT usable after this function is called.
     *
     */
    void stopListeningForEvents();

private:
    struct libinput* libinput_;
    // registerd shortcuts
    unordered_map<string, ShortcutBinding> shortcuts_;
    unordered_set<string> activeShortcuts_;
    
    QList<QPair<std::string, struct libinput_device*>> devices_list_; // 存储设备路径的列表

    // Qt related
    std::shared_ptr<QThreadPool> _thread_pool;

    // Mutex to protect the shortcuts_ map
    QMutex _shortcuts_mutex;

    QMutex _should_restart_mutex;

    // Whether the event listener should exit
    volatile bool _should_exit = false;
    // Whether the event listener is currently running
    volatile bool _is_listening = false;
    // Only can be modified by checkKeyboardDevice()
    volatile bool _should_restart = false;

    volatile bool _should_check_keyboard = true;

    // Used to store the thread that is listening for events
    std::thread _worker_thread;

    std::thread _device_checker_thread;

    /**
     * @brief Initialize the libinput context, it will search for devices automatically.
     * 
     */
    void initLibinput();

    /**
     * @brief Restart the libinput context and the event listener.
     * 
     */
    void restartLibinputAndListening();

    /**
     * @brief Handles an event from the evdev library.
     *
     * @param keyboardEvent libinput_event_keyboard from libinput
     */
    void handleKeyEvent(struct libinput_event_keyboard* keyboardEvent);

    /**
     * @brief Scan and Add keyboard devices to the libinput context.
     *
     */
    void addKeyboardDevices();

    /**
     * @brief check and Remove/add keyboard devices from the libinput context.
     */
    void checkKeyboardAdd();
    
    bool checkKeyboardRemove();
};

#endif // HOTKEY_MANAGER_H
