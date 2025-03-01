// main.cpp
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <qcoreapplication.h>
#include <vector>

#include "hotkey_manager.h"

#include <QCoreApplication>
#include <QTimer>

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    auto hotkeyManager = std::make_shared<GlobalHotkeyManager>(&a);

    // Bind a testing shortcut
    // Example binding Ctrl + Alt + N to the exampleCallback function
    unordered_set<int> exampleShortcutKeys = { KEY_LEFTCTRL, KEY_LEFTALT, KEY_N }; // KEY_N, KEY_LEFTALT
    hotkeyManager->bindShortcut("example_shortcut", exampleShortcutKeys, []() {
        std::cout << "Example shortcut triggered" << std::endl;
    });

    QMetaObject::invokeMethod(&a, std::bind(&GlobalHotkeyManager::startListeningForEvents, hotkeyManager), Qt::QueuedConnection);


    // Test kill app using timer
    // Using about to quit
    QTimer::singleShot(4000, [hotkeyManager]() {
        std::cout << "Killing app" << std::endl;
        qApp->exit();
    });

    return a.exec();
}
