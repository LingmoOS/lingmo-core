/*
 * SPDX-FileCopyrightText: 2025 Elysia <elysia@lingmo.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

// STL
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <set>

// Python
#include <pybind11/embed.h>
namespace py = pybind11;

// Qt
#include <QObject>

class GlobalHotkeyManager : public QObject {
    Q_OBJECT
public:
    explicit GlobalHotkeyManager(QObject* parent = nullptr);

    /**
     * 绑定快捷键到回调函数。
     * @param shortcut_id 快捷键的标识符。
     * @param key_combination 构成快捷键的键码列表。
     * @param callback 当快捷键被激活时调用的函数。
     */
    void bind_shortcut(std::uint64_t shortcut_id,
        std::set<int> key_combination,
        std::function<void()> callback);

    /**
     * @brief Checks if all keys for a given shortcut are pressed.
     *
     * @param shortcut_id  The ID of the shortcut to check.
     * @return true
     * @return false
     */
    bool _is_shortcut_activated(const std::uint64_t& shortcut_id);

    /**
     * @brief Handles an event from the evdev library.
     *
     * @param event InputEvent from evdev
     * @param shortcut_id unique identifier of the shortcut registerd
     */
    void _handle_event(const pybind11::handle& event, const uint64_t& shortcut_id);

    /**
     * @brief Start to listen for events from the evdev library.
     *
     */
    void listen_for_events();

private:
    // Store shortcut bindings
    // Format: {shortcut_id: (key_combination, callback, key_combination)}
    std::map<std::uint64_t, std::tuple<std::set<int>, std::function<void()>, std::shared_ptr<std::set<int>>>>
        _shortcut_bindings;

    pybind11::dict _device;

    pybind11::module _evdev;
    pybind11::module _select;
};
