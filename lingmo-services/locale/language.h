// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-FileCopyrightText: 2026 LingmoOS Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "base/dbus.h"
#include "base/settings.h"
#include "notification.h"

class Language
{
public:
    // What a language change does to the appearance: a primary language with
    // a font of its own sets it, and the fallback families follow the order.
    struct Appearance {
        std::function<void(const QString &family)> setSystemFont;
        std::function<void()> updateFontConfig;
    };

    Language(Lingmo::Base::Settings &settings, Lingmo::Base::DBus::Connection *session,
             Appearance appearance);
    ~Language();

    Language(const Language &) = delete;
    Language &operator=(const Language &) = delete;

    // org.lingmo.Services.Language at /org/lingmo/Services/Language.
    bool publish();

    QString languageCode() const;
    void setLanguage(const QString &code);
    QStringList preferredLanguages() const;
    void setPreferredLanguages(const QStringList &codes);
    // LANG, LANGUAGE and the LC_ variables for the preferred languages, as
    // programs started now get them; LC_ALL is empty.
    std::map<QString, QString> environment() const;

    // A locale's language and territory, such as "en_US" for "en", as the
    // session names it.
    static QString localeName(const QString &code);

    // Gives this process the new language variables, before its own text
    // follows; base::SessionLanguage::apply() unless set.
    std::function<void(const std::map<QString, QString> &variables)> applyEnvironment;
    // Asks lingmo-locales for a locale the system lacks; `done` tells
    // whether it is there now.
    std::function<Lingmo::Base::DBus::PendingCall(const QString &locale, std::function<void(bool)> done)>
            generateLocale;

private:
    void publishEnvironment();
    // A primary language whose locale is missing has it generated, then
    // programs get the variables again, now with its dates and numbers.
    void prepareLocale();
    void emitLanguageChanged();

    Lingmo::Base::Settings &m_settings;
    Lingmo::Base::DBus::Connection *m_session;
    Appearance m_appearance;
    std::unique_ptr<Lingmo::Base::DBus::Object> m_object;
    std::unique_ptr<Notification> m_notification;
    Lingmo::Base::DBus::PendingCall m_localeCall;
};

#endif
