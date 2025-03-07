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

    auto testingShortcut1 = Shortcut({
        "testing_shortcut1",
        {
            {"keys", "Ctrl+Alt+N"},
            {"description", "Testing shortcut1"}
        }
    });

    bool status;
    interface.BindShortcut(testingShortcut1, status);

    //Test Q_SLOT
    QObject::connect(&interface, &GlobalShortcutInterface::Activated, [](const QString& shortcutIdentifier){
        std::cout << "Shortcut " << shortcutIdentifier.toStdString() << " activated" << std::endl;
    });

    QTimer::singleShot(2000, [&interface]() {
        auto all = interface.shortcutDescriptions();
        std::cout << "Current registered shortcut: " << std::endl;
        for (const auto& shortcut : all) {
            std::cout << shortcut.first.toStdString() << " " << shortcut.second["trigger_description"].toString().toStdString() << std::endl;
        }
    });

    return a.exec();
}
