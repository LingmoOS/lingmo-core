#include "hotkey_manager.h"

// For ecodes defines
#include <linux/input-event-codes.h>

GlobalHotkeyManager::GlobalHotkeyManager(QObject* parent)
    : QObject(parent)
{
    _evdev = py::module::import("evdev");
    _select = py::module::import("select");

    // Init devices
    auto _device_paths = _evdev.attr("list_devices")();
    for (const auto& path : _device_paths) {
        auto _input_device = _evdev.attr("InputDevice")(path);

        _device[_input_device.attr("fd")] = _input_device;
    }
}

void GlobalHotkeyManager::bind_shortcut(std::uint64_t shortcut_id,
    std::set<int> key_combination,
    std::function<void()> callback)
{
    // TODO: check if shortcut_id is already bound
    _shortcut_bindings.insert(
        { shortcut_id,
            { key_combination, callback, std::make_shared<std::set<int>>() } });
}

bool GlobalHotkeyManager::_is_shortcut_activated(const std::uint64_t& shortcut_id)
{
    try {
        auto shortcut = _shortcut_bindings.at(shortcut_id);

        return (*std::get<2>(shortcut) == std::get<0>(shortcut));
    } catch (const std::out_of_range& e) {
        return false;
    }
}

void GlobalHotkeyManager::_handle_event(const py::handle& event, const uint64_t& shortcut_id)
{
    if (event.attr("type").cast<int>() == static_cast<int>(EV_KEY)) {
        try {
            auto shortcut = _shortcut_bindings.at(shortcut_id);
            auto shortcut_list = std::get<0>(shortcut);
            if (std::count(shortcut_list.begin(), shortcut_list.end(),
                    event.attr("code").cast<int>())
                > 0) {
                if (event.attr("value").cast<int>() == 1) {
                    std::get<2>(shortcut)->insert(event.attr("code").cast<int>());

                    if (_is_shortcut_activated(shortcut_id)) {
                        std::get<1>(shortcut)();
                    }
                } else {
                    // key release
                    std::get<2>(shortcut)->erase(event.attr("code").cast<int>());
                }
            }
        } catch (const std::out_of_range& e) {
            return;
        }
    }
}

void GlobalHotkeyManager::listen_for_events()
{
    while (true) {
        auto res = _select.attr("select")(_device, py::list(), py::list())
                       .cast<py::tuple>();
        auto rdev = res[0];
        for (const auto& device : rdev) {
            for (const auto& event : _device[device].attr("read")()) {
                for (auto& shortcut : _shortcut_bindings) {
                    _handle_event(event, shortcut.first);
                }
            }
        }
    }
}
