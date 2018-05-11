/*
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef DISKS_H
#define DISKS_H

#include <QObject>
#include <QMap>
#include <QtDBus>
#include <QTimer>
#include "device.h"

class Disks : public QObject
{
    Q_OBJECT

public:
    explicit Disks(QObject *parent = NULL);
    QMap<QString, Device*> devices;

private:
    QDBusInterface *dbus;
    QTimer timer;

signals:
    void updatedDevices();
    void mediaChanged(QString path, bool media);
    void mountpointChanged(QString path, QString mountpoint);
    void deviceErrorMessage(QString path, QString error);
    void foundNewDevice(QString path);

public slots:
    QStringList getDevices();

private slots:
    void setupDBus();
    void scanDevices();
    void deviceAdded(const QDBusObjectPath &obj);
    void deviceRemoved(const QDBusObjectPath &obj);
    void handleDeviceMediaChanged(QString devicePath, bool mediaPresent);
    void handleDeviceMountpointChanged(QString devicePath, QString deviceMountpoint);
    void handleDeviceErrorMessage(QString devicePath, QString deviceError);
    void checkUDisks();
};

#endif // DISKS_H
