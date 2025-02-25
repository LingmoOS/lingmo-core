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

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Start the hotkey manager in seperate thread
    QThread hotkeyManagerThread;
    GlobalHotkeyManager *hotkeyManager = new GlobalHotkeyManager();
    hotkeyManager->moveToThread(&hotkeyManagerThread);
    QObject::connect(&hotkeyManagerThread, &QThread::started, [hotkeyManager](){
        // Bind a testing shortcut
        // Example binding Ctrl + Alt + N to the exampleCallback function
        unordered_set<int> exampleShortcutKeys = { KEY_LEFTCTRL, KEY_LEFTALT, KEY_N }; // KEY_N, KEY_LEFTALT
        hotkeyManager->bindShortcut("example_shortcut", exampleShortcutKeys, [](){
            std::cout << "Example shortcut triggered" << std::endl;
        });
        hotkeyManager->listenForEvents();
    });
    QObject::connect(&a, &QCoreApplication::aboutToQuit, [hotkeyManager, &hotkeyManagerThread](){
        hotkeyManager->stopListeningForEvents();
        hotkeyManager->deleteLater();
        hotkeyManagerThread.exit();
    });

    hotkeyManagerThread.start();

    // Test kill app using timer
    // Using about to quit
    QTimer::singleShot(10000, [](){
        std::cout << "Killing app" << std::endl;
        qApp->exit();
    });

    return a.exec();
}
