/*
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "device.h"
#include "udisks2.h"

Device::Device(const QString block, QObject *parent)
    : QObject(parent)
    , path(block)
    , isOptical(false)
    , isRemovable(false)
    , hasMedia(false)
    , opticalDataTracks(0)
    , opticalAudioTracks(0)
    , isBlankDisc(false)
    , hasPartition(false)
    , dbus(0)
{
    QDBusConnection system = QDBusConnection::systemBus();
    dbus = new QDBusInterface(DBUS_SERVICE, path, QString("%1.Block").arg(DBUS_SERVICE), system, parent);
    system.connect(dbus->service(), dbus->path(), DBUS_PROPERTIES, "PropertiesChanged", this, SLOT(handlePropertiesChanged(QString,QMap<QString,QVariant>)));
    updateDeviceProperties();
}

void Device::mount()
{
    if (!dbus->isValid() || !mountpoint.isEmpty()) { return; }
    QString reply = uDisks2::mountDevice(path);
    if (!reply.isEmpty()) {
        emit errorMessage(path, reply);
        return;
    }
    updateDeviceProperties();
}

void Device::unmount()
{
    if (!dbus->isValid() || mountpoint.isEmpty()) { return; }
    QString reply = uDisks2::unmountDevice(path);
    updateDeviceProperties();
    if (!reply.isEmpty() || !mountpoint.isEmpty()) {
        if (reply.isEmpty()) { reply = QObject::tr("Failed to umount %1").arg(name); }
        emit errorMessage(path, reply);
        return;
    }
    if (isOptical) { eject(); }
}

void Device::eject()
{
    if (!dbus->isValid()) { return; }
    QString reply = uDisks2::ejectDevice(drive);
    updateDeviceProperties();
    if (!reply.isEmpty()/* || hasMedia*/) {
        if (reply.isEmpty()) { reply = QObject::tr("Failed to eject %1").arg(name); }
        emit errorMessage(path, reply);
    }
}

void Device::updateDeviceProperties()
{
    if (!dbus->isValid()) { return; }

    bool hadMedia =  hasMedia;
    QString lastMountpoint = mountpoint;
    QString lastName = name;

    drive = uDisks2::getDrivePath(path);
    name = uDisks2::getDeviceName(drive);
    dev = path.split("/").takeLast();
    isRemovable = uDisks2::isRemovable(drive);
    mountpoint = uDisks2::getMountPoint(path);
    filesystem = uDisks2::getFileSystem(path);
    isOptical = uDisks2::isOptical(drive);
    hasMedia = uDisks2::hasMedia(drive);
    opticalDataTracks = uDisks2::opticalDataTracks(drive);
    opticalAudioTracks = uDisks2::opticalAudioTracks(drive);
    isBlankDisc = uDisks2::opticalMediaIsBlank(drive);
    hasPartition = uDisks2::hasPartition(path);

    if (hadMedia != hasMedia) {
        emit mediaChanged(path, hasMedia);
    }
    if (lastMountpoint != mountpoint) {
        emit mountpointChanged(path, mountpoint);
    }
    if (lastName != name) {
        emit nameChanged(path, name);
    }
}

void Device::handlePropertiesChanged(const QString &interfaceType, const QMap<QString, QVariant> &changedProperties)
{
    Q_UNUSED(interfaceType)
    Q_UNUSED(changedProperties)
    updateDeviceProperties();
}
