// main.cpp
#include <functional>
#include <iostream>

#include <QCoreApplication>
#include <QTimer>

#include "dbushelper.h"
#include "global_shortcut_interface.h"

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    // auto hotkeyManager = std::make_shared<GlobalHotkeyManager>(&a);

    // Bind a testing shortcut
    // Example binding Ctrl + Alt + N to the exampleCallback function
    // unordered_set<int> exampleShortcutKeys = { KEY_LEFTCTRL, KEY_LEFTALT, KEY_N }; // KEY_N, KEY_LEFTALT
    // hotkeyManager->bindShortcut("example_shortcut", exampleShortcutKeys, []() {
    //     std::cout << "Example shortcut triggered" << std::endl;
    // });

    // QMetaObject::invokeMethod(&a, std::bind(&GlobalHotkeyManager::startListeningForEvents, hotkeyManager), Qt::QueuedConnection);

    // QTimer::singleShot(1000, [hotkeyManager]() {
    //     unordered_set<int> exampleShortcutKeys = { KEY_LEFTCTRL, KEY_LEFTALT, KEY_N }; // KEY_N, KEY_LEFTALT
    //     hotkeyManager->bindShortcut("example_shortcut", exampleShortcutKeys, []() {
    //         std::cout << "Example shortcut triggered" << std::endl;
    //     });
    // });

    // Test kill app using timer
    // Using about to quit
    // QTimer::singleShot(4000, [hotkeyManager]() {
    //     std::cout << "Killing app" << std::endl;
    //     qApp->exit();
    // });

    auto interface = GlobalShortcutInterface(&a);

    auto testingShortcut = Shortcut({
        "testing_shortcut",
        {
            {"keys", "Ctrl+Alt+N"},
            {"description", "Testing shortcut"}
        }
    });

    interface.BindShortcut(testingShortcut);

    //Test Q_SLOT
    QObject::connect(&interface, &GlobalShortcutInterface::Activated, [](const QString& shortcutIdentifier){
        std::cout << "Shortcut " << shortcutIdentifier.toStdString() << " activated" << std::endl;
    });

    return a.exec();
}
