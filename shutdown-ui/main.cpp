#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QFile>
#include <QLocale>
#include <QTranslator>
#include <QDBusConnection>

#include "actions.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    if (!QDBusConnection::sessionBus().registerService("com.cute.ShutdownUI")) {
        return -1;
    }

    if (!QDBusConnection::sessionBus().registerObject("/ShutdownUI", &app)) {
        return -1;
    }

    // Translations
    QLocale locale;
    QString qmFilePath = QString("%1/%2.qm").arg("/usr/share/cute-shutdown/translations/").arg(locale.name());
    if (QFile::exists(qmFilePath)) {
        QTranslator *translator = new QTranslator(QGuiApplication::instance());
        if (translator->load(qmFilePath)) {
            QGuiApplication::installTranslator(translator);
        } else {
            translator->deleteLater();
        }
    }

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.rootContext()->setContextProperty("actions", new Actions);
    engine.load(url);

    return app.exec();
}
