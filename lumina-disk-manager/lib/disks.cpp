/*
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "disks.h"
#include "udisks2.h"
#include <QDebug>
#include <QTimer>

Disks::Disks(QObject *parent)
    : QObject(parent)
    , dbus(0)
{
    setupDBus();
    timer.setInterval(60000);
    connect(&timer, SIGNAL(timeout()), this, SLOT(checkUDisks()));
    timer.start();
}

QStringList Disks::getDevices()
{
    return uDisks2::getDevices();
}

void Disks::setupDBus()
{
    QDBusConnection system = QDBusConnection::systemBus();
    if (system.isConnected()) {
        system.connect(DBUS_SERVICE, DBUS_PATH, DBUS_OBJMANAGER, DBUS_DEVICE_ADDED, this, SLOT(deviceAdded(const QDBusObjectPath&)));
        system.connect(DBUS_SERVICE, DBUS_PATH, DBUS_OBJMANAGER, DBUS_DEVICE_REMOVED, this, SLOT(deviceRemoved(const QDBusObjectPath&)));
        if (dbus==NULL) { dbus = new QDBusInterface(DBUS_SERVICE, DBUS_PATH, DBUS_OBJMANAGER, system); } // only used to verify the UDisks is running
        scanDevices();
    }
}

void Disks::scanDevices()
{
    QStringList foundDevices = uDisks2::getDevices();
    for (int i=0;i<foundDevices.size();i++) {
        QString foundDevicePath = foundDevices.at(i);
        bool hasDevice = devices.contains(foundDevicePath);
        if (hasDevice) { continue; }
        Device *newDevice = new Device(foundDevicePath, this);
        connect(newDevice, SIGNAL(mediaChanged(QString,bool)), this, SLOT(handleDeviceMediaChanged(QString,bool)));
        connect(newDevice, SIGNAL(mountpointChanged(QString,QString)), this, SLOT(handleDeviceMountpointChanged(QString,QString)));
        connect(newDevice, SIGNAL(errorMessage(QString,QString)), this, SLOT(handleDeviceErrorMessage(QString,QString)));
        devices[foundDevicePath] = newDevice;
    }
    emit updatedDevices();
}

void Disks::deviceAdded(const QDBusObjectPath &obj)
{
    if (!dbus->isValid()) { return; }
    QString path = obj.path();
    if (path.startsWith(QString("%1/jobs").arg(DBUS_PATH))) { return; }

    scanDevices();
    emit foundNewDevice(path);
}

void Disks::deviceRemoved(const QDBusObjectPath &obj)
{
    if (!dbus->isValid()) { return; }
    QString path = obj.path();
    bool deviceExists = devices.contains(path);
    if (path.startsWith(QString("%1/jobs").arg(DBUS_PATH))) { return; }

    if (deviceExists) {
        if (uDisks2::getDevices().contains(path)) { return; }
        delete devices.take(path);
    }
    scanDevices();
}

void Disks::handleDeviceMediaChanged(QString devicePath, bool mediaPresent)
{
    emit mediaChanged(devicePath, mediaPresent);
}

void Disks::handleDeviceMountpointChanged(QString devicePath, QString deviceMountpoint)
{
    emit mountpointChanged(devicePath, deviceMountpoint);
}

void Disks::handleDeviceErrorMessage(QString devicePath, QString deviceError)
{
    emit deviceErrorMessage(devicePath, deviceError);
}

void Disks::checkUDisks()
{
    if (!QDBusConnection::systemBus().isConnected()) {
        setupDBus();
        return;
    }
    if (!dbus->isValid()) { scanDevices(); }
}
