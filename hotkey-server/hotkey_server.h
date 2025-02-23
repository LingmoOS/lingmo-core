#ifndef __HOTKEY_SERVER_H__
#define __HOTKEY_SERVER_H__

#include <QCoreApplication>

class HotkeyServer : public QCoreApplication {
    Q_OBJECT
public:
    explicit HotkeyServer(int argc, char *argv[]);

};

#endif // __HOTKEY_SERVER_H__