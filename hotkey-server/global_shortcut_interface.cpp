/*
 * SPDX-FileCopyrightText: 2025 Elysia <elysia@lingmo.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "global_shortcut_interface.h"

#include <QtGui> // QKeySequence

#include "globalshortcutadaptor.h"
#include "hotkey_manager.h"
#include "keycode_helper.h"

GlobalShortcutInterface::GlobalShortcutInterface(QObject* parent)
    : QObject(parent)
    , m_adaptor { new GlobalshortcutAdaptor(this) }
    , m_hotkeyManager { new GlobalHotkeyManager(this) }
{
    // Start manager
    QMetaObject::invokeMethod(this, std::bind(&GlobalHotkeyManager::startListeningForEvents, m_hotkeyManager), Qt::QueuedConnection);
}

void GlobalShortcutInterface::BindShortcut(const Shortcut& shortcut)
{
    auto identifier = shortcut.first;
    auto shortcut_str_list = QKeySequence(shortcut.second["keys"].toString());
    auto shortcut_description = shortcut.second["description"].toString();
    // Convert into unordered_set<int>, using defines in evdevs
    // TODO: Only support one key for now
    auto keys = shortcut_str_list[0].toCombined();
    auto key = Qt::Key(keys & ~Qt::KeyboardModifierMask);
    auto modifiers = Qt::KeyboardModifiers(keys & Qt::KeyboardModifierMask);

    auto nativeShortcut = _getNativeShortcut(key, modifiers);
    m_hotkeyManager->bindShortcut(identifier.toStdString(), nativeShortcut, [this, identifier]() { emit Activated(identifier); }, shortcut_description);
}

void GlobalShortcutInterface::UnbindShortcut(const QString& shortcutIdentifier) { }
void GlobalShortcutInterface::ListShortcuts() { }

Lingmo::HotKey::NativeShortcut GlobalShortcutInterface::_getNativeShortcut(const Qt::Key& key, const Qt::KeyboardModifiers& modifiers)
{
    using namespace Lingmo::HotKey;
    return NativeShortcut { GetNativeKeycode(key), GetNativeModifier(modifiers) };
}
