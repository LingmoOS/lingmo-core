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

#include "hotkey_server.h"

int main(int argc, char *argv[])
{
    HotkeyServer a(argc, argv);

    py::scoped_interpreter python;

    GlobalHotkeyManager manager;

    manager.bind_shortcut(1, { KEY_LEFTCTRL, KEY_LEFTALT, KEY_N}, []() { std::cout << "Shortcut activated" << std::endl; });

    return a.exec();
}
