// main.cpp
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <vector>


#include "hotkey_manager.h"

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    [[maybe_unused]] GlobalHotkeyManager* manager = new GlobalHotkeyManager();

    // auto thread = QThread::create([manager]() {
    //     manager->listen_for_events();
    // });
    // QObject::connect(thread, &QThread::finished, [manager]() {
    //     manager->stop_listening_for_events();
    // });
    // thread->start();

    return a.exec();
}
