/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -m -a notificationsadaptor -i notificationserver.h -l NotificationServer /home/reion/Lingmo/core/notificationd/org.freedesktop.Notifications.xml
 *
 * qdbusxml2cpp is Copyright (C) 2020 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef NOTIFICATIONSADAPTOR_H
#define NOTIFICATIONSADAPTOR_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include "notificationserver.h"

#include <QByteArray>
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVariant>

/*
 * Adaptor class for interface org.freedesktop.Notifications
 */
class NotificationsAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Notifications")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.freedesktop.Notifications\">\n"
"    <method name=\"GetServerInformation\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"name\"/>\n"
"      <arg direction=\"out\" type=\"s\" name=\"vendor\"/>\n"
"      <arg direction=\"out\" type=\"s\" name=\"version\"/>\n"
"      <arg direction=\"out\" type=\"s\" name=\"spec_version\"/>\n"
"    </method>\n"
"    <method name=\"CloseNotification\">\n"
"      <arg direction=\"in\" type=\"u\" name=\"id\"/>\n"
"    </method>\n"
"    <method name=\"Notify\">\n"
"      <annotation value=\"QVariantMap\" name=\"org.qtproject.QtDBus.QtTypeName.In6\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"app_name\"/>\n"
"      <arg direction=\"in\" type=\"u\" name=\"replaces_id\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"app_icon\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"summary\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"body\"/>\n"
"      <arg direction=\"in\" type=\"as\" name=\"actions\"/>\n"
"      <arg direction=\"in\" type=\"a{sv}\" name=\"hints\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"expire_timeout\"/>\n"
"      <arg direction=\"out\" type=\"u\" name=\"id\"/>\n"
"    </method>\n"
"    <method name=\"GetCapabilities\">\n"
"      <arg direction=\"out\" type=\"as\" name=\"capabilities\"/>\n"
"    </method>\n"
"    <signal name=\"ActionInvoked\">\n"
"      <arg type=\"u\"/>\n"
"      <arg type=\"s\"/>\n"
"    </signal>\n"
"    <signal name=\"NotificationClosed\">\n"
"      <arg type=\"u\"/>\n"
"      <arg type=\"u\"/>\n"
"    </signal>\n"
"  </interface>\n"
        "")
public:
    NotificationsAdaptor(NotificationServer *parent);
    virtual ~NotificationsAdaptor();

    inline NotificationServer *parent() const
    { return static_cast<NotificationServer *>(QObject::parent()); }

public: // PROPERTIES
public Q_SLOTS: // METHODS
    void CloseNotification(uint id);
    QStringList GetCapabilities();
    QString GetServerInformation(QString &vendor, QString &version, QString &spec_version);
    uint Notify(const QString &app_name, uint replaces_id, const QString &app_icon, const QString &summary, const QString &body, const QStringList &actions, const QVariantMap &hints, int expire_timeout);
Q_SIGNALS: // SIGNALS
    void ActionInvoked(uint in0, const QString &in1);
    void NotificationClosed(uint in0, uint in1);
};

#endif
