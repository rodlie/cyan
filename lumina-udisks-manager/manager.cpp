#include "manager.h"
#include "udisks2.h"
#include <QDebug>
#include <QTimer>

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
    qDebug() << "setup device" << path;
    QDBusConnection system = QDBusConnection::systemBus();
    dbus = new QDBusInterface(DBUS_SERVICE, path, QString("%1.Block").arg(DBUS_SERVICE), system, parent);
    qDebug() << "connect?" << system.connect(dbus->service(), dbus->path(), DBUS_PROPERTIES, "PropertiesChanged", this, SLOT(handlePropertiesChanged(QString,QMap<QString,QVariant>)));
    updateDeviceProperties();
}

void Device::mount()
{
    qDebug() << "mount" << path;
    if (!mountpoint.isEmpty()) { return; }
    QString reply = uDisks2::mountDevice(path);
    if (!reply.isEmpty()) {
        qDebug() << "FAILED TO MOUNT" << reply;
    }
    updateDeviceProperties();
}

void Device::unmount()
{
    qDebug() << "unmount" << path;
    if (mountpoint.isEmpty()) { return; }
    QString reply = uDisks2::unmountDevice(path);
    updateDeviceProperties();
    if (!reply.isEmpty() || !mountpoint.isEmpty()) {
        qDebug() << "FAILED TO UNMOUNT" << mountpoint;
    }
    if (isOptical) { eject(); }
}

void Device::eject()
{
    qDebug() << "eject" << path;
    QString reply = uDisks2::ejectDevice(drive);
    updateDeviceProperties();
    if (!reply.isEmpty() || hasMedia) {
        qDebug() << "FAILED TO EJECT" << reply;
    }
}

void Device::updateDeviceProperties()
{
    qDebug() << "update device properties";


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
    qDebug() << "device properties changed" << path << interfaceType << changedProperties;
    updateDeviceProperties();
}

Manager::Manager(QObject *parent)
    : QObject(parent)
{
    setupDBus();
}

void Manager::setupDBus()
{
    qDebug() << "setup dbus";
    QDBusConnection system = QDBusConnection::systemBus();
    if (system.isConnected()) {
        qDebug() << "device added connect?" << system.connect(DBUS_SERVICE, DBUS_PATH, DBUS_OBJMANAGER, DBUS_DEVICE_ADDED, this, SLOT(deviceAdded(const QDBusObjectPath&)));
        qDebug() << "device removed connect?" << system.connect(DBUS_SERVICE, DBUS_PATH, DBUS_OBJMANAGER, DBUS_DEVICE_REMOVED, this, SLOT(deviceRemoved(const QDBusObjectPath&)));
        scanDevices();
    } else { QTimer::singleShot(300000, this, SLOT(setupDbus())); }
}

void Manager::scanDevices()
{
    qDebug() << "devs" << devices;
    qDebug() << "do a device scan...";
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

void Manager::deviceAdded(const QDBusObjectPath &obj)
{
    qDebug() << "device added";
    QString path = obj.path();
    bool deviceExists = devices.contains(path);
    if (path.startsWith(QString("%1/jobs").arg(DBUS_PATH))) { return; }

    qDebug() << "device was added" << path;
    qDebug() << "device exists?" << deviceExists;


    scanDevices();
}

void Manager::deviceRemoved(const QDBusObjectPath &obj)
{
    qDebug() << "device removed";
    QString path = obj.path();
    bool deviceExists = devices.contains(path);
    if (path.startsWith(QString("%1/jobs").arg(DBUS_PATH))) { return; }

    qDebug() << "device was removed" << path;
    qDebug() << "device exists?" << deviceExists;

    if (deviceExists) {
        if (uDisks2::getDevices().contains(path)) {
            qDebug() << "device is still present, so ignore remove.";
            return;
        }
        delete devices.take(path);
    }
    scanDevices();
}

void Manager::handleDeviceMediaChanged(QString devicePath, bool mediaPresent)
{
    qDebug() << "===> device media status changed" << devicePath << mediaPresent;
    emit mediaChanged(devicePath, mediaPresent);
}

void Manager::handleDeviceMountpointChanged(QString devicePath, QString deviceMountpoint)
{
    qDebug() << "===> device mountpoint changed" << devicePath << deviceMountpoint;
    emit mountpointChanged(devicePath, deviceMountpoint);
}

void Manager::handleDeviceErrorMessage(QString devicePath, QString deviceError)
{
    qDebug() << "===> device error" << devicePath << deviceError;
    emit deviceErrorMessage(devicePath, deviceError);
}
