#include "keycode_helper.h"

#include <linux/input-event-codes.h>
#include <qnamespace.h>

using namespace Lingmo::HotKey;

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

int GetNativeModifier(const Qt::KeyboardModifiers& modifier)
{
    if (modifier == Qt::ShiftModifier) {
        return KEY_LEFTSHIFT;
    } else if (modifier == Qt::ControlModifier) {
        return KEY_LEFTCTRL;
    } else if (modifier == Qt::AltModifier) {
        return KEY_LEFTALT;
    } else if (modifier == Qt::MetaModifier) {
        return KEY_LEFTMETA;
    } else {
        return KEY_UNKNOWN;
    }
}