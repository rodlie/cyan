/*
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef UDISKS2_H
#define UDISKS2_H

#include <QDBusObjectPath>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDBusInterface>
#include <QXmlStreamReader>

#define DBUS_SERVICE "org.freedesktop.UDisks2"
#define DBUS_PATH "/org/freedesktop/UDisks2"
#define DBUS_OBJMANAGER "org.freedesktop.DBus.ObjectManager"
#define DBUS_PROPERTIES "org.freedesktop.DBus.Properties"
#define DBUS_INTROSPECTABLE "org.freedesktop.DBus.Introspectable"
#define DBUS_DEVICE_ADDED "InterfacesAdded"
#define DBUS_DEVICE_REMOVED "InterfacesRemoved"

class uDisks2
{
public:
    static QString getDrivePath(QString path)
    {
        QDBusInterface iface(DBUS_SERVICE, path, QString("%1.Block").arg(DBUS_SERVICE), QDBusConnection::systemBus());
        if (!iface.isValid()) { return QString(); }
        QDBusObjectPath drive = iface.property("Drive").value<QDBusObjectPath>();
        return drive.path();
    }
    static bool hasPartition(QString path)
    {
        QDBusInterface iface(DBUS_SERVICE, path, QString("%1.Partition").arg(DBUS_SERVICE), QDBusConnection::systemBus());
        if (!iface.isValid()) { return false; }
        if (iface.property("Number").toUInt()>=1) { return true; }
        return false;
    }
    static QString getFileSystem(QString path)
    {
        QDBusInterface iface(DBUS_SERVICE, path, QString("%1.Block").arg(DBUS_SERVICE), QDBusConnection::systemBus());
        if (!iface.isValid()) { return QString(); }
        return iface.property("IdType").toString();
    }
    static bool isRemovable(QString path)
    {
        QDBusInterface iface(DBUS_SERVICE, path, QString("%1.Drive").arg(DBUS_SERVICE), QDBusConnection::systemBus());
        if (!iface.isValid()) { return false; }
        return iface.property("Removable").toBool();
    }
    static QString getMountPoint(QString path)
    {
        QString mountpoint;
        QDBusMessage message = QDBusMessage::createMethodCall(DBUS_SERVICE, path, DBUS_PROPERTIES, "Get");
        QList<QVariant> args;
        args << QString("%1.Filesystem").arg(DBUS_SERVICE) << "MountPoints";
        message.setArguments(args);
        QDBusMessage reply = QDBusConnection::systemBus().call(message);
        QStringList mountpoints;
        QList<QByteArray> argList;
        foreach (QVariant arg, reply.arguments()) { arg.value<QDBusVariant>().variant().value<QDBusArgument>() >> argList; }
        foreach (QByteArray point, argList) { mountpoints.append(point); }
        mountpoint = mountpoints.join("");
        return mountpoint;
    }
    static QString getDeviceName(QString path)
    {
        QDBusInterface iface(DBUS_SERVICE, path, QString("%1.Drive").arg(DBUS_SERVICE), QDBusConnection::systemBus());
        if (!iface.isValid()) { return QString(); }
        QString name = iface.property("Vendor").toString().simplified();
        if (!name.isEmpty()) { name.append(" "); }
        name.append(iface.property("Model").toString().simplified());
        return name;
    }
    static QStringList mountDevice(QString path)
    {
        QDBusInterface filesystem(DBUS_SERVICE, path, QString("%1.Filesystem").arg(DBUS_SERVICE), QDBusConnection::systemBus());
        if (!filesystem.isValid()) { return QStringList(); }
        QVariantMap options;
        if (getFileSystem(path) == "vfat") { options.insert("options", "flush"); }
        QDBusReply<QString> mountpoint =  filesystem.call("Mount", options);
        QStringList result;
        result << mountpoint.value() << mountpoint.error().message();
        return result;
    }
    static QStringList unmountDevice(QString path)
    {
        QDBusInterface filesystem(DBUS_SERVICE, path, QString("%1.Filesystem").arg(DBUS_SERVICE), QDBusConnection::systemBus());
        if (!filesystem.isValid()) { return QStringList(); }
        QDBusReply<QString> reply = filesystem.call("Unmount", QVariantMap());
        QStringList result;
        result << reply.value() << reply.error().message();
        return result;
    }
    static QVector<QStringList> getRemovableDevices()
    {
        QVector<QStringList> result;
        QDBusMessage call = QDBusMessage::createMethodCall(DBUS_SERVICE, QString("%1/block_devices").arg(DBUS_PATH), DBUS_INTROSPECTABLE, "Introspect");
        QDBusPendingReply<QString> reply = QDBusConnection::systemBus().call(call);
        QList<QDBusObjectPath> devices;
        QXmlStreamReader xml(reply.value());
        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name().toString() == "node" ) {
                QString name = xml.attributes().value("name").toString();
                if(!name.isEmpty()) { devices << QDBusObjectPath("/org/freedesktop/UDisks2/block_devices/" + name); }
            }
        }
        foreach (QDBusObjectPath device, devices) {
            QString path = device.path();
            QString drive = getDrivePath(path);
            if (isRemovable(drive) && hasPartition(path)) {
                QStringList deviceInfo;
                QString deviceName = getDeviceName(drive);
                if (deviceName.isEmpty()) { deviceName = QObject::tr("Unknown device"); }
                deviceInfo << path << deviceName << getMountPoint(path);
                result << deviceInfo;
            }
        }
        return result;
    }
};

#endif // UDISKS2_H
