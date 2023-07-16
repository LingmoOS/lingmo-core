#include "language.h"
#include "languageadaptor.h"
#include "theme/thememanager.h"

#include <QDBusInterface>
#include <QSettings>

Language *Language::self()
{
    static Language s;
    return &s;
}

Language::Language(QObject *parent)
    : QObject(parent)
    , m_settings(new QSettings(QStringLiteral("cuteos"), QStringLiteral("language")))
{
    new LanguageAdaptor(this);
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/Language"), this);

    if (!m_settings->contains("language"))
        m_settings->setValue("language", "en_US");

    emit languageChanged();
}

QString Language::languageCode() const
{
    return m_settings->value("language").toString();
}

void Language::setLanguage(const QString &code)
{
    if (m_settings->value("language").toString() == code) {
        return;
    }

    QSettings settings(QStringLiteral("cuteos"), QStringLiteral("theme"));

    // Change fonts
    if (code == "zh_CN") {
        settings.setValue("Font", "Noto Sans CJK SC");
    } else if (code.contains("en_")) {
        settings.setValue("Font", "Noto Sans");
    } else if (code == "zh_TW") {
        settings.setValue("Font", "Noto Sans CJK TC");
    } else if (code == "zh_HK") {
        settings.setValue("Font", "Noto Serif CJK HK");
    } else if (code == "ja_JP") {
        settings.setValue("Font", "Noto Serif CJK JP");
    }

    m_settings->setValue("language", code);
    emit languageChanged();

    QDBusInterface iface("org.freedesktop.Notifications",
                         "/org/freedesktop/Notifications",
                         "org.freedesktop.Notifications",
                         QDBusConnection::sessionBus());
    if (iface.isValid()) {
        QList<QVariant> args;
        args << "cute-settings";
        args << ((unsigned int) 0);
        args << "preferences-system";
        args << "";
        args << tr("The system language has been changed, please log out and log in");
        args << QStringList();
        args << QVariantMap();
        args << (int) 10;
        iface.asyncCallWithArgumentList("Notify", args);
    }
}
