/*
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "manager.h"
#include "upower.h"
#include <QDebug>
#include <QTimer>

Device::Device(const QString block, QObject *parent)
    : QObject(parent)
    , path(block)
    , isRechargable(false)
    , isPresent(false)
    , percentage(0)
    , online(false)
    , hasPowerSupply(false)
    , isBattery(false)
    , isAC(false)
    , capacity(0)
    , energy(0)
    , energyFullDesign(0)
    , energyFull(0)
    , energyEmpty(0)
    , dbus(0)
{
    QDBusConnection system = QDBusConnection::systemBus();
    dbus = new QDBusInterface(DBUS_SERVICE, path, QString("%1.Device").arg(DBUS_SERVICE), system, parent);
    system.connect(dbus->service(), dbus->path(), QString("%1.Device").arg(DBUS_SERVICE), "Changed", this, SLOT(handlePropertiesChanged()));
    if (name.isEmpty()) { name = path.split("/").takeLast(); }
    updateDeviceProperties();
}

void Device::updateDeviceProperties()
{
    if (!dbus->isValid()) { return; }

    capacity =  dbus->property("Capacity").toDouble();
    isRechargable =  dbus->property("IsRechargeable").toBool();
    isPresent =  dbus->property("IsPresent").toBool();
    percentage =  dbus->property("Percentage").toDouble();
    energyFullDesign = dbus->property("EnergyFullDesign").toDouble();
    energyFull = dbus->property("EnergyFull").toDouble();
    energyEmpty = dbus->property("EnergyEmpty").toDouble();
    energy = dbus->property("Energy").toDouble();
    online = dbus->property("Online").toBool();
    hasPowerSupply = dbus->property("PowerSupply").toBool();

    uint type = dbus->property("Type").toUInt();
    if (type == 2) { isBattery = true; }
    else {
        isBattery = false;
        if (type == 1) { isAC = true; } else { isAC = false; }
    }

    vendor = dbus->property("Vendor").toString();
    nativePath = dbus->property("NativePath").toString();

    emit deviceChanged(path);
}

void Device::handlePropertiesChanged()
{
    updateDeviceProperties();
}

Manager::Manager(QObject *parent)
    : QObject(parent)
    , dbus(0)
{
    setupDBus();
    timer.setInterval(60000);
    connect(&timer, SIGNAL(timeout()), this, SLOT(checkUPower()));
    timer.start();
}

void Manager::setupDBus()
{
    QDBusConnection system = QDBusConnection::systemBus();
    if (system.isConnected()) {
        system.connect(DBUS_SERVICE, DBUS_PATH, DBUS_OBJMANAGER, DBUS_DEVICE_ADDED, this, SLOT(deviceAdded(const QDBusObjectPath&)));
        system.connect(DBUS_SERVICE, DBUS_PATH, DBUS_OBJMANAGER, DBUS_DEVICE_REMOVED, this, SLOT(deviceRemoved(const QDBusObjectPath&)));
        if (dbus==NULL) { dbus = new QDBusInterface(DBUS_SERVICE, DBUS_PATH, DBUS_OBJMANAGER, system); } // only used to verify that UPower is running
        scanDevices();
    }
}

void Manager::scanDevices()
{
    QStringList foundDevices = UPower::getDevices();
    for (int i=0;i<foundDevices.size();i++) {
        QString foundDevicePath = foundDevices.at(i);
        bool hasDevice = devices.contains(foundDevicePath);
        if (hasDevice) { continue; }
        Device *newDevice = new Device(foundDevicePath, this);
        connect(newDevice, SIGNAL(deviceChanged(QString)), this, SLOT(handleDeviceChanged(QString)));
        devices[foundDevicePath] = newDevice;
    }
    emit updatedDevices();
}

void Manager::deviceAdded(const QDBusObjectPath &obj)
{
    if (!dbus->isValid()) { return; }
    QString path = obj.path();
    if (path.startsWith(QString("%1/jobs").arg(DBUS_PATH))) { return; }

    scanDevices();
}

void Manager::deviceRemoved(const QDBusObjectPath &obj)
{
    if (!dbus->isValid()) { return; }
    QString path = obj.path();
    bool deviceExists = devices.contains(path);
    if (path.startsWith(QString("%1/jobs").arg(DBUS_PATH))) { return; }

    if (deviceExists) {
        if (UPower::getDevices().contains(path)) { return; }
        delete devices.take(path);
    }
    scanDevices();
}

void Manager::handleDeviceChanged(QString devicePath)
{
    if (devicePath.isEmpty()) { return; }
    emit updatedDevices();
}


void Manager::checkUPower()
{
    if (!QDBusConnection::systemBus().isConnected()) {
        setupDBus();
        return;
    }
    if (!dbus->isValid()) { scanDevices(); }
}
