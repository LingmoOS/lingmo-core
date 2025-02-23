// main.cpp
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <vector>

#include <pybind11/embed.h>
namespace py = pybind11;

#include "hotkey_manager.h"

// For evdev
#include <linux/input-event-codes.h>

int main()
{
    py::scoped_interpreter python;

    GlobalHotkeyManager manager;

    manager.bind_shortcut(1, { KEY_LEFTCTRL, KEY_LEFTALT, KEY_N}, []() { std::cout << "Shortcut activated" << std::endl; });

    try {
        manager.listen_for_events();
    } catch (const std::exception& e) {
        std::cout << "Exitting..." << std::endl;
    }

    return 0;
}
