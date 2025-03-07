/*
 * SPDX-FileCopyrightText: 2025 Elysia <elysia@lingmo.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "global_shortcut_interface.h"

#include <QtGui> // QKeySequence
#include <qkeysequence.h>
#include <qvariant.h>

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

void GlobalShortcutInterface::BindShortcut(const Shortcut& shortcut, bool& success)
{
    auto identifier = shortcut.first;
    auto shortcut_str_list = QKeySequence(shortcut.second["keys"].toString());
    auto shortcut_description = shortcut.second["description"].toString();
    // Convert into unordered_set<int>, using defines in evdevs
    // TODO: Only support one key for now
    auto keys = shortcut_str_list[0].toCombined();
    auto key = Qt::Key(keys & ~Qt::KeyboardModifierMask);
    auto modifiers = Qt::KeyboardModifiers(keys & Qt::KeyboardModifierMask);

    auto nativeShortcut = _getNativeShortcut(key, modifiers, shortcut_description);
    success = m_hotkeyManager->bindShortcut(identifier.toStdString(), nativeShortcut, [this, identifier]() { emit Activated(identifier); }, shortcut_description);
    if (success) {
        m_shortcuts.emplaceBack(QPair<QString, Lingmo::HotKey::NativeShortcut> { identifier, nativeShortcut });
    }
}

uint GlobalShortcutInterface::UnbindShortcut(const QString& shortcutIdentifier) { }
uint GlobalShortcutInterface::ListShortcuts(QVariantMap &results) { 

    results = {
        {"shortcuts", this->shortcutDescriptionsVariant()},
    };
    return 0;
}

Lingmo::HotKey::NativeShortcut GlobalShortcutInterface::_getNativeShortcut(const Qt::Key& key, const Qt::KeyboardModifiers& modifiers, QString _description)
{
    using namespace Lingmo::HotKey;
    return NativeShortcut { GetNativeKeycode(key), GetNativeModifier(modifiers), _description };
}

QVariant GlobalShortcutInterface::shortcutDescriptionsVariant() const
{
    QDBusArgument retVar;
    retVar << shortcutDescriptions();
    return QVariant::fromValue(retVar);
}

Shortcuts GlobalShortcutInterface::shortcutDescriptions() const
{
    using namespace Lingmo::HotKey;
    Shortcuts ret;
    for (const auto& [identifier, nativeinfo] : m_shortcuts) {
        QStringList triggers;
        for (const auto& modifier : nativeinfo.modifier) {
            triggers += QKeySequence(GetQtModifier(modifier)).toString(QKeySequence::NativeText);
        }
        triggers += QKeySequence(GetQtKeycode(nativeinfo.key)).toString(QKeySequence::NativeText);

        ret.append({ identifier,
            QVariantMap {
                { QStringLiteral("description"), nativeinfo.description },
                { QStringLiteral("trigger_description"), triggers.join(("")) },
            } });
    }
    Q_ASSERT(ret.size() == static_cast<qsizetype>(m_shortcuts.size()));
    return ret;
}
