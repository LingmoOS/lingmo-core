#pragma once

#include <QObject>

namespace Lingmo::HotKey {
// Convert Qt::key into native keycode defined in evdev
int GetNativeKeycode(const Qt::Key& key);
// Convert Qt::KeyboardModifiers into native modifier defined in evdev
int GetNativeModifier(const Qt::KeyboardModifiers& modifier);
}
