/*
 * SPDX-FileCopyrightText: 2025 Elysia <elysia@lingmo.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef HOTKEY_MANAGER_H
#define HOTKEY_MANAGER_H
// STL
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <iostream>

// libinput related
#include <dirent.h> // 用于目录操作
#include <fcntl.h>
#include <unistd.h>

#include <libinput.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/input-event-codes.h>

// Qt
#include <QObject>
#include <QThreadPool>

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
    };

    explicit GlobalHotkeyManager(QObject* parent = nullptr);

    ~GlobalHotkeyManager() override;

    /**
     * 绑定快捷键到回调函数。
     * @param shortcut_id 快捷键的标识符。
     * @param keyCombination 构成快捷键的键码列表。
     * @param callback 当快捷键被激活时调用的函数。
     */
    void bindShortcut(const string& shortcutId, const unordered_set<int>& keyCombination, function<void()> callback);

    /**
     * @brief Start to listen for events from the libinput library.
     *
     */
    void listenForEvents();

    /**
     * @brief Stop listening for events from the libinput library. This instance is NOT usable after this function is called.
     * 
     */
    void stopListeningForEvents();

private:
    libinput* libinput_;
    // registerd shortcuts
    unordered_map<string, ShortcutBinding> shortcuts_;
    unordered_set<string> activeShortcuts_;

    // Qt related
    std::shared_ptr<QThreadPool> _thread_pool;

    // Whether the event listener should exit
    volatile bool _should_exit = false;
    // Whether the event listener is currently running
    volatile bool _is_listening = false;

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
};

#endif // HOTKEY_MANAGER_H
