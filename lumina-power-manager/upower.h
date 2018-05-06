/*
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef UPOWER_H
#define UPOWER_H

#include <QDBusObjectPath>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDBusInterface>
#include <QXmlStreamReader>
#include <QDebug>
#include <QStringList>

#define DBUS_SERVICE "org.freedesktop.UPower"
#define DBUS_PATH "/org/freedesktop/UPower"
#define DBUS_OBJMANAGER "org.freedesktop.DBus.ObjectManager"
#define DBUS_PROPERTIES "org.freedesktop.DBus.Properties"
#define DBUS_INTROSPECTABLE "org.freedesktop.DBus.Introspectable"
#define DBUS_DEVICE_ADDED "DeviceAdded"
#define DBUS_DEVICE_REMOVED "DeviceRemoved"

class UPower
{
public:
    static QStringList getDevices()
    {
        QStringList result;
        QDBusMessage call = QDBusMessage::createMethodCall(DBUS_SERVICE, QString("%1/devices").arg(DBUS_PATH), DBUS_INTROSPECTABLE, "Introspect");
        QDBusPendingReply<QString> reply = QDBusConnection::systemBus().call(call);
        QList<QDBusObjectPath> devices;
        QXmlStreamReader xml(reply.value());
        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name().toString() == "node" ) {
                QString name = xml.attributes().value("name").toString();
                if(!name.isEmpty()) { devices << QDBusObjectPath("/org/freedesktop/UPower/devices/" + name); }
            }
        }
        foreach (QDBusObjectPath device, devices) {
            result << device.path();
        }
        return result;
    }
};

#endif // POWER_H
