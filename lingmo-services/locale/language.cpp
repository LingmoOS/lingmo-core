// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-FileCopyrightText: 2026 LingmoOS Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "language.h"
#include "appearance/fontconfigwriter.h"

#include <algorithm>
#include <map>

#include <systemd/sd-bus.h>
#include <unicode/uloc.h>

#include "base/i18n.h"
#include "base/session_language.h"
#include "base/strings.h"
#include "common/environment.h"
#include "common/language_environment.h"

using namespace Lingmo::Base;
using namespace Lingmo::Base::strings;
using Lingmo::I18n::tr;
namespace language = Lingmo::Core::language;

static void removeAll(QStringList &list, const QString &value)
{
    list.erase(std::remove(list.begin(), list.end(), value), list.end());
}

static void removeDuplicates(QStringList &list)
{
    QStringList unique;
    for (const QString &entry : list) {
        if (std::find(unique.begin(), unique.end(), entry) == unique.end())
            unique.push_back(entry);
    }
    list = std::move(unique);
}

Language::Language(Settings &settings, dbus::Connection *session, Appearance appearance)
    : m_settings(settings)
    , m_session(session)
    , m_appearance(std::move(appearance))
{
}

Language::~Language() = default;

bool Language::publish()
{
    if (!m_session)
        return false;
    m_object = std::make_unique<dbus::Object>(m_session, "/org/lingmo/Services/Language",
                                              "org.lingmo.Services.Language");
    m_object->addMethod("setLanguage", [this](const QString &value) { setLanguage(value); },
                        {{"value"}, {}});
    m_object->addMethod("setPreferredLanguages",
                        [this](const QStringList &value) { setPreferredLanguages(value); },
                        {{"value"}, {}});
    m_object->addProperty("preferredLanguages", [this] { return preferredLanguages(); });
    m_object->addProperty("languageCode", [this] { return languageCode(); });
    m_object->addProperty("environment", [this] { return environment(); });
    m_object->addSignal("languageChanged", "");
    if (!m_object->publish())
        return false;
    // A language picked before its locale was there, or on another system.
    prepareLocale();
    return true;
}

QString Language::languageCode() const
{
    const QString code = m_settings.stringValue("Language/language");
    return code.empty() ? language::systemLanguage() : code;
}

QStringList Language::preferredLanguages() const
{
    const QString primary = languageCode();
    QStringList codes = m_settings.stringListValue("Language/preferredLanguages");
    removeAll(codes, primary);
    codes.insert(codes.begin(), primary);
    removeDuplicates(codes);
    return codes;
}

std::map<QString, QString> Language::environment() const
{
    Lingmo::Core::Environment environment = Lingmo::Core::processEnvironment();
    language::apply(environment, preferredLanguages());
    std::map<QString, QString> variables;
    for (const auto &[name, value] : environment) {
        if (name == "LANG" || name == "LANGUAGE" || startsWith(name, "LC_"))
            variables[name] = value;
    }
    // D-Bus cannot remove variables; an empty LC_ALL lets individual categories take effect.
    variables["LC_ALL"] = QString();
    return variables;
}

void Language::setLanguage(const QString &code)
{
    QStringList codes = preferredLanguages();
    removeAll(codes, code);
    codes.insert(codes.begin(), code);
    setPreferredLanguages(codes);
}

QString Language::localeName(const QString &code)
{
    char likely[ULOC_FULLNAME_CAPACITY] = {};
    UErrorCode status = U_ZERO_ERROR;
    uloc_addLikelySubtags(code.c_str(), likely, sizeof(likely), &status);
    if (U_FAILURE(status))
        return "C";
    char languageCode[ULOC_LANG_CAPACITY] = {};
    char country[ULOC_COUNTRY_CAPACITY] = {};
    uloc_getLanguage(likely, languageCode, sizeof(languageCode), &status);
    uloc_getCountry(likely, country, sizeof(country), &status);
    if (U_FAILURE(status) || !*languageCode || QStringView(languageCode) == "c")
        return "C";
    return *country ? QString(languageCode) + '_' + country : QString(languageCode);
}

void Language::setPreferredLanguages(const QStringList &languages)
{
    QStringList codes = languages;
    removeAll(codes, QString());
    removeDuplicates(codes);
    if (codes.empty() || codes == preferredLanguages())
        return;

    const QString code = codes.front();
    const bool primaryChanged = code != languageCode();
    m_settings.setValue("Language/preferredLanguages", codes);

    if (primaryChanged) {
        const QString font = fontForLocale(localeName(code));
        if (!font.empty() && m_appearance.setSystemFont)
            m_appearance.setSystemFont(font);
    }

    m_settings.setValue("Language/language", code);
    m_settings.sync();
    // The CJK fallback follows the order of the languages, so it is rewritten after the sync.
    if (m_appearance.updateFontConfig)
        m_appearance.updateFontConfig();

    publishEnvironment();
    prepareLocale();

    if (!primaryChanged)
        return;

    m_notification = std::make_unique<Notification>(m_session);
    m_notification->setDesktopEntry("lingmo-settings");
    m_notification->setIconName("preferences-system");
    m_notification->setBody(tr("Some applications may need to be restarted for the change to take effect."));
    m_notification->setTimeout(10);
    m_notification->show();
}

void Language::publishEnvironment()
{
    const std::map<QString, QString> activationEnvironment = environment();
    QStringList systemdEnvironment;
    for (const auto &[name, value] : activationEnvironment) {
        if (name != "LC_ALL")
            systemdEnvironment.push_back(name + '=' + value);
    }
    if (m_session) {
        const dbus::Proxy bus(m_session, "org.freedesktop.DBus", "/org/freedesktop/DBus",
                              "org.freedesktop.DBus");
        bus.send("UpdateActivationEnvironment", activationEnvironment);

        dbus::Message systemd = m_session->newMethodCall("org.freedesktop.systemd1", "/org/freedesktop/systemd1",
                                                         "org.freedesktop.systemd1.Manager",
                                                         "UnsetAndSetEnvironment");
        sd_bus_message_set_auto_start(systemd.get(), 0);
        systemd.append(QStringList {"LC_ALL"}, systemdEnvironment);
        m_session->send(systemd);
    }
    // This process takes the new language itself, before its own text below;
    // the signal reaches it only later, if at all.
    if (applyEnvironment)
        applyEnvironment(activationEnvironment);
    else
        SessionLanguage::apply(activationEnvironment);
    emitLanguageChanged();
}

void Language::prepareLocale()
{
    const QString code = languageCode();
    const QString locale = language::localeFor(code);
    if (!generateLocale || language::isAvailable(locale))
        return;
    m_localeCall = generateLocale(locale, [this, code](bool generated) {
        if (generated && code == languageCode())
            publishEnvironment();
    });
}

void Language::emitLanguageChanged()
{
    if (m_object)
        m_object->emitSignal("languageChanged");
}
