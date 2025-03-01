/*
 * SPDX-FileCopyrightText: 2025 Elysia <elysia@lingmo.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "global_shortcut_interface.h"

#include "globalshortcutadaptor.h"
#include "hotkey_manager.h"
#include <QtGui/qkeysequence.h>

GlobalShortcutInterface::GlobalShortcutInterface(QObject* parent)
    : QObject(parent)
    , m_adaptor { new GlobalshortcutAdaptor(this) }
    , m_hotkeyManager{new GlobalHotkeyManager(this)}
{
    // Start manager
    QMetaObject::invokeMethod(this, std::bind(&GlobalHotkeyManager::startListeningForEvents, m_hotkeyManager), Qt::QueuedConnection);
}

void GlobalShortcutInterface::BindShortcut(const Shortcut& shortcut) {
    auto identifier = shortcut.first.toStdString();
    auto shortcut_str_list = QKeySequence(shortcut.second["keys"].toString());
    // Convert into unordered_set<int>, using defines in evdevs
    //Only support one key for now
    auto keys = shortcut_str_list[0].toCombined();
    auto key = Qt::Key(keys & ~Qt::KeyboardModifierMask);
    auto modifiers = Qt::KeyboardModifiers(keys & Qt::KeyboardModifierMask);
}

void GlobalShortcutInterface::UnbindShortcut(const QString& shortcutIdentifier) {}
void GlobalShortcutInterface::ListShortcuts(){}