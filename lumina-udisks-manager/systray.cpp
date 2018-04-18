/*
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "systray.h"
#include <QIcon>
#include <QProcess>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include <QDebug>

SysTray::SysTray(QObject *parent)
    : QObject(parent)
    , tray(0)
    , menu(0)
    , man(0)
{
    menu = new QMenu();

    tray = new QSystemTrayIcon(QIcon::fromTheme("drive-removable-media"), this);
    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
    connect(tray, SIGNAL(messageClicked()), this, SLOT(handleMessageClicked()));

    man  = new Manager(this);
    connect(man , SIGNAL(updatedDevices()), this, SLOT(generateContextMenu()));
    connect(man, SIGNAL(deviceErrorMessage(QString,QString)), this, SLOT(handleDeviceError(QString,QString)));
    connect(man, SIGNAL(mediaChanged(QString,bool)), this, SLOT(handleDeviceMediaChanged(QString,bool)));
    connect(man, SIGNAL(mountpointChanged(QString,QString)), this, SLOT(handleDeviceMountpointChanged(QString,QString)));
    generateContextMenu();
}

void SysTray::generateContextMenu()
{
    qDebug() << "generate context menu";
    for(int i=0;i<menu->actions().size();i++) {
        menu->actions().at(i)->disconnect();
        delete menu->actions().at(i);
    }
    menu->clear();

    QMapIterator<QString, Device*> device(man->devices);
    while (device.hasNext()) {
        device.next();
        qDebug() << "add to menu?" << device.key() << "is removable?" << device.value()->isRemovable;
        if ((device.value()->isOptical && !device.value()->hasMedia) || (!device.value()->isOptical && !device.value()->isRemovable) || (!device.value()->isOptical && !device.value()->hasPartition)) {
            continue;
        }

        QAction *deviceAction = new QAction(this);
        deviceAction->setData(device.key());
        deviceAction->setText(device.value()->name);

        connect(deviceAction, SIGNAL(triggered(bool)), this, SLOT(handleContextMenuAction()));
        menu->addAction(deviceAction);

        if (device.value()->mountpoint.isEmpty()) {
            deviceAction->setIcon(QIcon::fromTheme(device.value()->isOptical?"drive-optical":"drive-removable-media"));
            bool hasAudio = device.value()->opticalAudioTracks>0?true:false;
            bool hasData = device.value()->opticalDataTracks>0?true:false;
            if (device.value()->isBlankDisc||(hasAudio&&!hasData)) { deviceAction->setIcon(QIcon::fromTheme("media-eject")); }
        } else { deviceAction->setIcon(QIcon::fromTheme("media-eject")); }
    }

    qDebug() << menu->actions();
    if (menu->actions().size()==0) {
        if (tray->isVisible()) { tray->hide(); }
    } else {
        if (!tray->isVisible() && tray->isSystemTrayAvailable()) { tray->show(); }
    }
}

void SysTray::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason) {
    case QSystemTrayIcon::Context:
    case QSystemTrayIcon::Trigger:
        if (menu->actions().size()>0) { menu->popup(QCursor::pos()); }
    default:;
    }
}

void SysTray::handleMessageClicked()
{
    // Do something?
}

void SysTray::handleContextMenuAction()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (action==NULL) { return; }

    /*QString actionText = action->text();
    QString path = action->data().toString();
    QString drive = uDisks2::getDrivePath(path);
    bool isOptical = uDisks2::isOptical(drive);
    bool hasMedia = uDisks2::hasOpticalMedia(drive);
    QString mountpoint = uDisks2::getMountPoint(path);
    bool hasAudio = uDisks2::opticalAudioTracks(drive)>0?true:false;
    bool hasData = uDisks2::opticalDataTracks(drive)>0?true:false;
    bool hasBlank = uDisks2::opticalMediaIsBlank(drive);

    if (isOptical && hasMedia) {
        bool wasUnmounted = false;
        if (!mountpoint.isEmpty()) { // unmount device
            QString unmounted = uDisks2::unmountDevice(path);
            qDebug() << "unmounted?" << unmounted;
            if (!unmounted.isEmpty()) {
                if (tray->isVisible()) { tray->showMessage(tr("Failed to unmount %1").arg(actionText), tr("Failed to unmount the device, please check that none application are using any files on this device.")); }
                return;
            }
            wasUnmounted = true;
        }
        if (hasBlank || wasUnmounted || (hasAudio && !hasData)) { // eject device
            QString ejected = uDisks2::ejectDevice(drive);
            qDebug() << "ejected?" << ejected;
            if (!ejected.isEmpty()) {
                if (tray->isVisible()) { tray->showMessage(tr("Failed to eject %1").arg(actionText), tr("Failed to eject the device, this should not happen, is it still mounted?")); }
            } else { generateContextMenu(); }
            return;
        }
    }

    if (!mountpoint.isEmpty()) { // unmount device
        uDisks2::unmountDevice(path);
        mountpoint = uDisks2::getMountPoint(path);
        if (!mountpoint.isEmpty()) {
            if (tray->isVisible()) { tray->showMessage(tr("Failed to unmount %1").arg(actionText), tr("Failed to unmount the device, please check that none application are using any files on this device.")); }
        } else {
            if (tray->isVisible()) { tray->showMessage(tr("%1 removed").arg(actionText), tr("It's now safe to remove %1 from your computer.").arg(actionText)); }
            generateContextMenu();
        }
    } else { // mount device
        QStringList mounted = uDisks2::mountDevice(path);
        generateContextMenu();
        if (!mounted.at(0).isEmpty()) {
            // TODO: check for DVD
            QProcess::startDetached(QString("lumina-open %1").arg(mounted.at(0)));
        } else {
            QString msg = tr("Failed to mount volume %1, this is probably due to an unsupported file system (%2) or permissions.").arg(actionText).arg(uDisks2::getFileSystem(path));
            if (mounted.size()>1) { msg = mounted.at(1); }
            if (tray->isVisible()) { tray->showMessage(tr("Failed to mount %1").arg(actionText), msg); }
        }
    }*/
}

void SysTray::handleDeviceError(QString path, QString error)
{
    qDebug() << "handle device error" << path << error;
    if (!tray->isSystemTrayAvailable()||!man->devices.contains(path)) { return; }
    if (!tray->isVisible()) { tray->show(); }
    tray->showMessage(QObject::tr("Error for device %1").arg(man->devices[path]->name), error);
}

void SysTray::handleDeviceMediaChanged(QString path, bool media)
{
    qDebug() << "handle device media changed" << path << media;
    if (!tray->isSystemTrayAvailable()||!man->devices.contains(path)) { return; }
    generateContextMenu();
    if (man->devices[path]->isOptical && media) {
        bool isData = man->devices[path]->opticalDataTracks>0?true:false;
        bool isAudio = man->devices[path]->opticalAudioTracks>0?true:false;
        QString opticalType;
        if (isData&&isAudio) { opticalType = "data+audio"; }
        else if (isData) { opticalType = "data"; }
        else if (isAudio) { opticalType = "audio"; }
        if (!tray->isVisible()) { tray->show(); }
        tray->showMessage(QObject::tr("%1 has media").arg(man->devices[path]->name), QObject::tr("Detected %1 media in %2").arg(opticalType).arg(man->devices[path]->name));
    }
}

void SysTray::handleDeviceMountpointChanged(QString path, QString mountpoint)
{
    qDebug() << "handle device mountpoint changed" << path << mountpoint;
    if (!tray->isSystemTrayAvailable()||!man->devices.contains(path)) { return; }
    generateContextMenu();
    if (!man->devices[path]->isRemovable) { qDebug()<<"not removable";return; }
    if (mountpoint.isEmpty()) {
        if (!tray->isVisible()) { tray->show(); }
        tray->showMessage(QObject::tr("%1 removed").arg(man->devices[path]->name), QObject::tr("It's now safe to remove %1 from your computer.").arg(man->devices[path]->name));
    } else { openMountpoint(mountpoint); }
}

void SysTray::openMountpoint(QString mountpoint)
{
    qDebug() << "open mountpoint in something" << mountpoint;
}
