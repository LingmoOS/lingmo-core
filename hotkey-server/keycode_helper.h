#pragma once

#include <QObject>

#include <unordered_set>

namespace Lingmo::HotKey {
class NativeShortcut {
public:
    //! The native keycode
    int key;
    //! The native modifiers
    std::unordered_set<int> modifier;

    //! Creates an invalid native shortcut
    NativeShortcut();
    //! Creates a valid native shortcut, with the given key and modifiers
    NativeShortcut(int _key, std::unordered_set<int> _modifier = {});

    //! Checks, whether this shortcut is valid or not
    bool isValid() const;

    //! Equality operator
    bool operator==(NativeShortcut other) const;
    //! Inequality operator
    bool operator!=(NativeShortcut other) const;

private:
    bool valid;
};

// Convert Qt::key into native keycode defined in evdev
int GetNativeKeycode(const Qt::Key& key);
// Convert Qt::KeyboardModifiers into native modifier defined in evdev
std::unordered_set<int> GetNativeModifier(const Qt::KeyboardModifiers& modifier);
}
