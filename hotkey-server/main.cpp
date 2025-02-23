// main.cpp
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <vector>

#include <pybind11/embed.h>
namespace py = pybind11;

// Make sure that a scoped_interpreter is created before using pybind11
class GlobalShortcutManager {
public:
    explicit GlobalShortcutManager()
    {
        _evdev = py::module::import("evdev");
        _ecodes = _evdev.attr("ecodes");
        _select =  py::module::import("select");

        // Init devices
        _device_paths = _evdev.attr("list_devices")();
        for (const auto& path : _device_paths) {
            auto _input_device = _evdev.attr("InputDevice")(path);
            
            _device[_input_device.attr("fd")] = _input_device;
        }
    }

    /**
     * 绑定快捷键到回调函数。
     * @param shortcut_id 快捷键的标识符。
     * @param key_combination 构成快捷键的键码列表。
     * @param callback 当快捷键被激活时调用的函数。
     */
    void bind_shortcut(std::uint64_t shortcut_id, std::vector<int> key_combination, std::function<void()> callback)
    {
        // TODO: check if shortcut_id is already bound
        _shortcut_bindings.insert({ shortcut_id, { key_combination, callback, std::make_shared<std::vector<int>>() } });
    }

    /**
     * @brief Checks if all keys for a given shortcut are pressed.
     *
     * @param shortcut_id  The ID of the shortcut to check.
     * @return true
     * @return false
     */
    bool _is_shortcut_activated(const std::uint64_t& shortcut_id)
    {
        try {
            auto shortcut = _shortcut_bindings.at(shortcut_id);

            return (*std::get<2>(shortcut) == std::get<0>(shortcut));
        } catch (const std::out_of_range& e) {
            return false;
        }
    }

    void _handle_event(const py::handle& event, const uint64_t& shortcut_id)
    {
        if (event.attr("type").cast<int>() == _ecodes.attr("EV_KEY").cast<int>()) {
            try {
                auto shortcut = _shortcut_bindings.at(shortcut_id);
                auto shortcut_list = std::get<0>(shortcut);
                if (std::count(shortcut_list.begin(), shortcut_list.end(), event.attr("code").cast<int>()) > 0) {
                    if (event.attr("value").cast<int>() == 1) {
                        std::get<2>(shortcut)->push_back(event.attr("code").cast<int>());

                        if (_is_shortcut_activated(shortcut_id)) {
                            std::get<1>(shortcut)();
                        }
                    } else {
                        // key release
                        std::get<2>(shortcut)->erase(std::remove(std::get<2>(shortcut)->begin(), std::get<2>(shortcut)->end(), event.attr("code").cast<int>()), std::get<2>(shortcut)->end());
                    }
                }
            } catch (const std::out_of_range& e) {
                return;
            }
        }
    }

    void listen_for_events()
    {
        while (true) {
            auto res = _select.attr("select")(_device, py::list(), py::list()).cast<py::tuple>();
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

private:
    py::dict _device;

    std::map<
        std::uint64_t,
        std::tuple<
            std::vector<int>,
            std::function<void()>,
            std::shared_ptr<std::vector<int>>>>
        _shortcut_bindings;

    py::module _evdev;
    py::module _ecodes;
    py::module _select;

    py::object _device_paths;
};

int main()
{
    py::scoped_interpreter python;

    GlobalShortcutManager manager;

    manager.bind_shortcut(1, { 29, 56, 49}, []() { std::cout << "Shortcut activated" << std::endl; });

    try {
        manager.listen_for_events();
    } catch (const std::exception& e) {
        std::cout << "Exitting..." << std::endl;
    }

    return 0;
}
