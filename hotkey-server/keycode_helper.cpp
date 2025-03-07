#include "keycode_helper.h"

#include <linux/input-event-codes.h>
#include <qnamespace.h>

namespace Lingmo::HotKey {

NativeShortcut::NativeShortcut()
    : key {}
    , modifier {}
    , valid(false)
{
}

NativeShortcut::NativeShortcut(int _key, std::unordered_set<int> _modifier, QString _description)
    : key { _key }
    , modifier { _modifier }
    , description(_description)
    , valid(false)
{
}

bool NativeShortcut::isValid() const
{
    return valid;
}

bool NativeShortcut::operator==(NativeShortcut other) const
{
    return (key == other.key) && (modifier == other.modifier) && valid == other.valid;
}

bool NativeShortcut::operator!=(NativeShortcut other) const
{
    return (key != other.key) || (modifier != other.modifier) || valid != other.valid;
}

///////////////////////////////////////////////////////////////////////////////////////

int GetNativeKeycode(const Qt::Key& key)
{
    switch (key) {
    case Qt::Key_Escape:
        return KEY_ESC;
    case Qt::Key_1:
        return KEY_1;
    case Qt::Key_2:
        return KEY_2;
    case Qt::Key_3:
        return KEY_3;
    case Qt::Key_4:
        return KEY_4;
    case Qt::Key_5:
        return KEY_5;
    case Qt::Key_6:
        return KEY_6;
    case Qt::Key_7:
        return KEY_7;
    case Qt::Key_8:
        return KEY_8;
    case Qt::Key_9:
        return KEY_9;
    case Qt::Key_0:
        return KEY_0;
    case Qt::Key_Minus:
        return KEY_MINUS;
    case Qt::Key_Equal:
        return KEY_EQUAL;
    case Qt::Key_Backspace:
        return KEY_BACKSPACE;
    case Qt::Key_Tab:
        return KEY_TAB;
    case Qt::Key_Q:
        return KEY_Q;
    case Qt::Key_W:
        return KEY_W;
    case Qt::Key_E:
        return KEY_E;
    case Qt::Key_R:
        return KEY_R;
    case Qt::Key_T:
        return KEY_T;
    case Qt::Key_Y:
        return KEY_Y;
    case Qt::Key_U:
        return KEY_U;
    case Qt::Key_I:
        return KEY_I;
    case Qt::Key_O:
        return KEY_O;
    case Qt::Key_P:
        return KEY_P;
    case Qt::Key_BracketLeft:
        return KEY_LEFTBRACE;
    case Qt::Key_BracketRight:
        return KEY_RIGHTBRACE;
    case Qt::Key_Return:
        return KEY_ENTER;
    case Qt::Key_Control:
        return KEY_LEFTCTRL;
    case Qt::Key_A:
        return KEY_A;
    case Qt::Key_S:
        return KEY_S;
    case Qt::Key_D:
        return KEY_D;
    case Qt::Key_F:
        return KEY_F;
    case Qt::Key_G:
        return KEY_G;
    case Qt::Key_H:
        return KEY_H;
    case Qt::Key_J:
        return KEY_J;
    case Qt::Key_K:
        return KEY_K;
    case Qt::Key_L:
        return KEY_L;
    case Qt::Key_Semicolon:
        return KEY_SEMICOLON;
    case Qt::Key_Apostrophe:
        return KEY_APOSTROPHE;
    case Qt::Key_Shift:
        return KEY_LEFTSHIFT;
    case Qt::Key_Backslash:
        return KEY_BACKSLASH;
    case Qt::Key_Z:
        return KEY_Z;
    case Qt::Key_X:
        return KEY_X;
    case Qt::Key_C:
        return KEY_C;
    case Qt::Key_V:
        return KEY_V;
    case Qt::Key_B:
        return KEY_B;
    case Qt::Key_N:
        return KEY_N;
    case Qt::Key_M:
        return KEY_M;
    case Qt::Key_Comma:
        return KEY_COMMA;
    case Qt::Key_Period:
        return KEY_DOT;
    case Qt::Key_Slash:
        return KEY_SLASH;
    // TODO: case Qt::Key_Shift: return KEY_RIGHTSHIFT;
    case Qt::Key_Asterisk:
        return KEY_KPASTERISK;
    case Qt::Key_Alt:
        return KEY_LEFTALT;
    case Qt::Key_Space:
        return KEY_SPACE;
    case Qt::Key_CapsLock:
        return KEY_CAPSLOCK;
    case Qt::Key_F1:
        return KEY_F1;
    case Qt::Key_F2:
        return KEY_F2;
    case Qt::Key_F3:
        return KEY_F3;
    case Qt::Key_F4:
        return KEY_F4;
    case Qt::Key_F5:
        return KEY_F5;
    case Qt::Key_F6:
        return KEY_F6;
    case Qt::Key_F7:
        return KEY_F7;
    case Qt::Key_F8:
        return KEY_F8;
    case Qt::Key_F9:
        return KEY_F9;
    case Qt::Key_F10:
        return KEY_F10;
    case Qt::Key_NumLock:
        return KEY_NUMLOCK;
    case Qt::Key_ScrollLock:
        return KEY_SCROLLLOCK;
    // TODO: Qt do not have keys for keypad!
    case Qt::Key_Zenkaku_Hankaku:
        return KEY_ZENKAKUHANKAKU;
    case Qt::Key_F11:
        return KEY_F11;
    case Qt::Key_F12:
        return KEY_F12;
    case Qt::Key_Katakana:
        return KEY_KATAKANA;
    case Qt::Key_Hiragana:
        return KEY_HIRAGANA;
    case Qt::Key_Henkan:
        return KEY_HENKAN;
    case Qt::Key_Muhenkan:
        return KEY_MUHENKAN;
    // TODO: case Qt::Key_Control: return KEY_RIGHTCTRL;
    // TODO: case Qt::Key_Alt: return KEY_RIGHTALT;
    case Qt::Key_Home:
        return KEY_HOME;
    case Qt::Key_Up:
        return KEY_UP;
    case Qt::Key_PageUp:
        return KEY_PAGEUP;
    case Qt::Key_Left:
        return KEY_LEFT;
    case Qt::Key_Right:
        return KEY_RIGHT;
    case Qt::Key_End:
        return KEY_END;
    case Qt::Key_Down:
        return KEY_DOWN;
    case Qt::Key_PageDown:
        return KEY_PAGEDOWN;
    case Qt::Key_Insert:
        return KEY_INSERT;
    case Qt::Key_Delete:
        return KEY_DELETE;
    case Qt::Key_VolumeMute:
        return KEY_MUTE;
    case Qt::Key_VolumeDown:
        return KEY_VOLUMEDOWN;
    case Qt::Key_VolumeUp:
        return KEY_VOLUMEUP;
    case Qt::Key_PowerDown:
        return KEY_POWER;
    case Qt::Key_Pause:
        return KEY_PAUSE;
    case Qt::Key_Meta:
        return KEY_LEFTMETA;
    // TODO: case Qt::Key_Meta: return KEY_RIGHTMETA;
    case Qt::Key_Stop:
        return KEY_STOP;
    case Qt::Key_Redo:
        return KEY_AGAIN;
    case Qt::Key_Undo:
        return KEY_UNDO;
    case Qt::Key_Copy:
        return KEY_COPY;
    case Qt::Key_Open:
        return KEY_OPEN;
    case Qt::Key_Paste:
        return KEY_PASTE;
    case Qt::Key_Find:
        return KEY_FIND;
    case Qt::Key_Cut:
        return KEY_CUT;
    case Qt::Key_Help:
        return KEY_HELP;
    case Qt::Key_Menu:
        return KEY_MENU;
    case Qt::Key_Calculator:
        return KEY_CALC;
    case Qt::Key_Sleep:
        return KEY_SLEEP;
    case Qt::Key_WakeUp:
        return KEY_WAKEUP;

    case Qt::Key_F13:
        return KEY_F13;
    case Qt::Key_F14:
        return KEY_F14;
    case Qt::Key_F15:
        return KEY_F15;
    case Qt::Key_F16:
        return KEY_F16;
    case Qt::Key_F17:
        return KEY_F17;
    case Qt::Key_F18:
        return KEY_F18;
    case Qt::Key_F19:
        return KEY_F19;
    case Qt::Key_F20:
        return KEY_F20;
    case Qt::Key_F21:
        return KEY_F21;
    case Qt::Key_F22:
        return KEY_F22;
    case Qt::Key_F23:
        return KEY_F23;
    case Qt::Key_F24:
        return KEY_F24;
    default:
        return KEY_UNKNOWN;
    }
}

Qt::Key GetQtKeycode(const int& keycode)
{
    switch (keycode) {
    case KEY_ESC:
        return Qt::Key_Escape;
    case KEY_1:
        return Qt::Key_1;
    case KEY_2:
        return Qt::Key_2;
    case KEY_3:
        return Qt::Key_3;
    case KEY_4:
        return Qt::Key_4;
    case KEY_5:
        return Qt::Key_5;
    case KEY_6:
        return Qt::Key_6;
    case KEY_7:
        return Qt::Key_7;
    case KEY_8:
        return Qt::Key_8;
    case KEY_9:
        return Qt::Key_9;
    case KEY_0:
        return Qt::Key_0;
    case KEY_MINUS:
        return Qt::Key_Minus;
    case KEY_EQUAL:
        return Qt::Key_Equal;
    case KEY_BACKSPACE:
        return Qt::Key_Backspace;
    case KEY_TAB:
        return Qt::Key_Tab;
    case KEY_Q:
        return Qt::Key_Q;
    case KEY_W:
        return Qt::Key_W;
    case KEY_E:
        return Qt::Key_E;
    case KEY_R:
        return Qt::Key_R;
    case KEY_T:
        return Qt::Key_T;
    case KEY_Y:
        return Qt::Key_Y;
    case KEY_U:
        return Qt::Key_U;
    case KEY_I:
        return Qt::Key_I;
    case KEY_O:
        return Qt::Key_O;
    case KEY_P:
        return Qt::Key_P;
    case KEY_LEFTBRACE:
        return Qt::Key_BracketLeft;
    case KEY_RIGHTBRACE:
        return Qt::Key_BracketRight;
    case KEY_ENTER:
        return Qt::Key_Return;
    case KEY_LEFTCTRL:
        return Qt::Key_Control;
    case KEY_A:
        return Qt::Key_A;
    case KEY_S:
        return Qt::Key_S;
    case KEY_D:
        return Qt::Key_D;
    case KEY_F:
        return Qt::Key_F;
    case KEY_G:
        return Qt::Key_G;
    case KEY_H:
        return Qt::Key_H;
    case KEY_J:
        return Qt::Key_J;
    case KEY_K:
        return Qt::Key_K;
    case KEY_L:
        return Qt::Key_L;
    case KEY_SEMICOLON:
        return Qt::Key_Semicolon;
    case KEY_APOSTROPHE:
        return Qt::Key_Apostrophe;
    case KEY_LEFTSHIFT:
        return Qt::Key_Shift;
    case KEY_BACKSLASH:
        return Qt::Key_Backslash;
    case KEY_Z:
        return Qt::Key_Z;
    case KEY_X:
        return Qt::Key_X;
    case KEY_C:
        return Qt::Key_C;
    case KEY_V:
        return Qt::Key_V;
    case KEY_B:
        return Qt::Key_B;
    case KEY_N:
        return Qt::Key_N;
    case KEY_M:
        return Qt::Key_M;
    case KEY_COMMA:
        return Qt::Key_Comma;
    case KEY_DOT:
        return Qt::Key_Period;
    case KEY_SLASH:
        return Qt::Key_Slash;
    // TODO: case KEY_RIGHTSHIFT: return Qt::Key_Shift;
    case KEY_KPASTERISK:
        return Qt::Key_Asterisk;
    case KEY_LEFTALT:
        return Qt::Key_Alt;
    case KEY_SPACE:
        return Qt::Key_Space;
    case KEY_CAPSLOCK:
        return Qt::Key_CapsLock;
    case KEY_F1:
        return Qt::Key_F1;
    case KEY_F2:
        return Qt::Key_F2;
    case KEY_F3:
        return Qt::Key_F3;
    case KEY_F4:
        return Qt::Key_F4;
    case KEY_F5:
        return Qt::Key_F5;
    case KEY_F6:
        return Qt::Key_F6;
    case KEY_F7:
        return Qt::Key_F7;
    case KEY_F8:
        return Qt::Key_F8;
    case KEY_F9:
        return Qt::Key_F9;
    case KEY_F10:
        return Qt::Key_F10;
    case KEY_NUMLOCK:
        return Qt::Key_NumLock;
    case KEY_SCROLLLOCK:
        return Qt::Key_ScrollLock;
    // TODO: Qt do not have keys for keypad!
    case KEY_ZENKAKUHANKAKU:
        return Qt::Key_Zenkaku_Hankaku;
    case KEY_F11:
        return Qt::Key_F11;
    case KEY_F12:
        return Qt::Key_F12;
    case KEY_KATAKANA:
        return Qt::Key_Katakana;
    case KEY_HIRAGANA:
        return Qt::Key_Hiragana;
    case KEY_HENKAN:
        return Qt::Key_Henkan;
    case KEY_MUHENKAN:
        return Qt::Key_Muhenkan;
    // TODO: case KEY_RIGHTCTRL: return Qt::Key_Control;
    // TODO: case KEY_RIGHTALT: return Qt::Key_Alt;
    case KEY_HOME:
        return Qt::Key_Home;
    case KEY_UP:
        return Qt::Key_Up;
    case KEY_PAGEUP:
        return Qt::Key_PageUp;
    case KEY_LEFT:
        return Qt::Key_Left;
    case KEY_RIGHT:
        return Qt::Key_Right;
    case KEY_END:
        return Qt::Key_End;
    case KEY_DOWN:
        return Qt::Key_Down;
    case KEY_PAGEDOWN:
        return Qt::Key_PageDown;
    case KEY_INSERT:
        return Qt::Key_Insert;
    case KEY_DELETE:
        return Qt::Key_Delete;
    case KEY_MUTE:
        return Qt::Key_VolumeMute;
    case KEY_VOLUMEDOWN:
        return Qt::Key_VolumeDown;
    case KEY_VOLUMEUP:
        return Qt::Key_VolumeUp;
    case KEY_POWER:
        return Qt::Key_PowerDown;
    case KEY_PAUSE:
        return Qt::Key_Pause;
    case KEY_LEFTMETA:
        return Qt::Key_Meta;
    // TODO: case KEY_RIGHTMETA: return Qt::Key_Meta;
    case KEY_STOP:
        return Qt::Key_Stop;
    case KEY_AGAIN:
        return Qt::Key_Redo;
    case KEY_UNDO:
        return Qt::Key_Undo;
    case KEY_COPY:
        return Qt::Key_Copy;
    case KEY_OPEN:
        return Qt::Key_Open;
    case KEY_PASTE:
        return Qt::Key_Paste;
    case KEY_FIND:
        return Qt::Key_Find;
    case KEY_CUT:
        return Qt::Key_Cut;
    case KEY_HELP:
        return Qt::Key_Help;
    case KEY_MENU:
        return Qt::Key_Menu;
    case KEY_CALC:
        return Qt::Key_Calculator;
    case KEY_SLEEP:
        return Qt::Key_Sleep;
    case KEY_WAKEUP:
        return Qt::Key_WakeUp;

    case KEY_F13:
        return Qt::Key_F13;
    case KEY_F14:
        return Qt::Key_F14;
    case KEY_F15:
        return Qt::Key_F15;
    case KEY_F16:
        return Qt::Key_F16;
    case KEY_F17:
        return Qt::Key_F17;
    case KEY_F18:
        return Qt::Key_F18;
    case KEY_F19:
        return Qt::Key_F19;
    case KEY_F20:
        return Qt::Key_F20;
    case KEY_F21:
        return Qt::Key_F21;
    case KEY_F22:
        return Qt::Key_F22;
    case KEY_F23:
        return Qt::Key_F23;
    case KEY_F24:
        return Qt::Key_F24;
    default:
        return Qt::Key_unknown;
    }
}

std::unordered_set<int> GetNativeModifier(const Qt::KeyboardModifiers& modifier)
{
    std::unordered_set<int> result = {};

    if (modifier & Qt::ShiftModifier) {
        result.insert(KEY_LEFTSHIFT);
    } 
    if (modifier & Qt::ControlModifier) {
        result.insert(KEY_LEFTCTRL);
    } 
    if (modifier & Qt::AltModifier) {
        result.insert(KEY_LEFTALT);
    } 
    if (modifier & Qt::MetaModifier) {
        result.insert(KEY_LEFTMETA);
    }

    return result;
}

Qt::KeyboardModifiers GetQtModifier(const int& modifier)
{
    Qt::KeyboardModifiers result = {};

    if (modifier == KEY_LEFTSHIFT) {
        result = Qt::ShiftModifier;
    } else if (modifier == KEY_LEFTCTRL) {
        result = Qt::ControlModifier;
    } else if (modifier == KEY_LEFTALT) {
        result = Qt::AltModifier;
    } else if (modifier == KEY_LEFTMETA) {
        result = Qt::MetaModifier;
    }

    return result;
}
}