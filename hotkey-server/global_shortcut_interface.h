/*
 * SPDX-FileCopyrightText: 2025 Elysia <elysia@lingmo.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#pragma once

#include <QDBusAbstractAdaptor>
#include <qcontainerfwd.h>
#include <qtmetamacros.h>

#include "dbushelper.h"
#include "keycode_helper.h"

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
    /**
     * @brief Bind a shortcut
     *
     * @param shortcut  (in) The shortcut to bind
     * @param success  (out) Whether the shortcut is successfully bound
     */
    uint BindShortcut(const Shortcut& shortcut, bool& success);
    uint UnbindShortcut(const QString& shortcutIdentifier);
    uint ListShortcuts(QVariantMap &results);

private:
    // D-Bus adaptor
    GlobalshortcutAdaptor* m_adaptor;

    // Hotkey magaer
    GlobalHotkeyManager* m_hotkeyManager;

    // Store registered shortcuts
    QList<QPair<QString, Lingmo::HotKey::NativeShortcut>> m_shortcuts;

    /**
     * @brief Get native shortcut from Qt key
     *
     * @param key
     * @param modifiers
     * @return Lingmo::HotKey::NativeShortcut
     */
    Lingmo::HotKey::NativeShortcut _getNativeShortcut(const Qt::Key& key, const Qt::KeyboardModifiers& modifiers, QString _description = "");

    /**
     * @brief Get registerd shortcut identifiers
     * 
     * @return QVariant 
     */
    QVariant shortcutDescriptionsVariant() const;

    /**
     * @brief Get registerd shortcut descriptions and details
     *
     * @return Shortcuts
     */
    Shortcuts shortcutDescriptions() const;
};