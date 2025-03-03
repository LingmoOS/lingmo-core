/*
 * SPDX-FileCopyrightText: 2025 Elysia <elysia@lingmo.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#pragma once

#include <QDBusAbstractAdaptor>
#include <qtmetamacros.h>

#include "dbushelper.h"

// forward declaration
class GlobalshortcutAdaptor;
class GlobalHotkeyManager;
namespace Lingmo::HotKey {
class NativeShortcut;
}

class GlobalShortcutInterface : public QObject {
    Q_OBJECT
public:
    explicit GlobalShortcutInterface(QObject* parent = nullptr);
    ~GlobalShortcutInterface() = default;

Q_SIGNALS:
    void Activated(const QString& shortcutIdentifier);
    void Deactivated(const QString& shortcutIdentifier);
    void ShortcutsChanged(const Shortcut& shortcut);

public Q_SLOTS:
    void BindShortcut(const Shortcut& shortcut);
    void UnbindShortcut(const QString& shortcutIdentifier);
    void ListShortcuts();

private:
    // D-Bus adaptor
    GlobalshortcutAdaptor* m_adaptor;

    // Hotkey magaer
    GlobalHotkeyManager* m_hotkeyManager;

    Lingmo::HotKey::NativeShortcut _getNativeShortcut(const Qt::Key& key, const Qt::KeyboardModifiers& modifiers);
};