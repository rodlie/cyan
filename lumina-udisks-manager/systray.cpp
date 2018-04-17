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

SysTray::SysTray(QObject *parent)
    : QObject(parent)
    , tray(0)
    , contextMenu(0)
{
    contextMenu = new QMenu();
    tray = new QSystemTrayIcon(QIcon::fromTheme("drive-removable-media"), this);
    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
    connect(tray, SIGNAL(messageClicked()), this, SLOT(handleMessageClicked()));
    setupDbus();
}

void SysTray::setupDbus()
{
    QDBusConnection system = QDBusConnection::systemBus();
    if (system.isConnected()) {
        system.connect(DBUS_SERVICE, DBUS_PATH, DBUS_OBJMANAGER, DBUS_DEVICE_ADDED, this, SLOT(deviceAdded(const QDBusObjectPath&)));
        system.connect(DBUS_SERVICE, DBUS_PATH, DBUS_OBJMANAGER, DBUS_DEVICE_REMOVED, this, SLOT(deviceRemoved(const QDBusObjectPath&)));
        if (uDisks2::getRemovableDevices().size()>0 && tray->isSystemTrayAvailable()) { tray->show(); }
        generateContextMenu();
    } else {
        if (tray->isVisible()) { tray->hide(); }
        QTimer::singleShot(300000, this, SLOT(setupDbus()));
    }
}

void SysTray::deviceAdded(const QDBusObjectPath &obj)
{
    QString path = obj.path();
    if (path.startsWith(QString("%1/jobs").arg(DBUS_PATH))) { return; }
    if (uDisks2::hasPartition(path)) {
        generateContextMenu();
        if (tray->isSystemTrayAvailable()) {
            if (!tray->isVisible()) { tray->show(); }
            tray->showMessage(uDisks2::getDeviceName(uDisks2::getDrivePath(path)), tr("Found new device"));
        }
    }
}

void SysTray::deviceRemoved(const QDBusObjectPath &obj)
{
    Q_UNUSED(obj)
    if (uDisks2::getRemovableDevices().size()==0 && tray->isVisible()) { tray->hide(); }
    generateContextMenu();
}

void SysTray::generateContextMenu()
{
    for(int i=0;i<contextMenu->actions().size();i++) {
        delete contextMenu->actions().at(i);
    }
    contextMenu->clear();
    QVector<QStringList> devices = uDisks2::getRemovableDevices();
    if (devices.size()==0) { return; }
    for (int i=0;i<devices.size();i++) {
        if (devices.at(i).size()<2) { continue; }
        QAction *devAction = new QAction(this);
        QString path = devices.at(i).at(0);
        QString title;
        QString mountpoint;
        if (devices.at(i).size()>=3) { mountpoint = devices.at(i).at(2); }
        if (mountpoint.isEmpty()) { devAction->setIcon(QIcon::fromTheme("drive-removable-media")); }
        else { devAction->setIcon(QIcon::fromTheme("media-eject")); }
        devAction->setText(devices.at(i).at(1));
        devAction->setData(path);
        connect(devAction, SIGNAL(triggered(bool)), this, SLOT(handleContextMenuAction()));
        contextMenu->addAction(devAction);
    }

}

void SysTray::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason) {
    case QSystemTrayIcon::Context:
    case QSystemTrayIcon::Trigger:
        contextMenu->popup(QCursor::pos());
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
    if (!action) { return; }
    QString actionText = action->text();
    QString path = action->data().toString();
    if (!uDisks2::getMountPoint(path).isEmpty()) { // unmount
        QStringList unmounted = uDisks2::unmountDevice(path);
        if (unmounted.at(0).isEmpty() && tray->isVisible()) {
            tray->showMessage(tr("%1 removed").arg(actionText), tr("It's now safe to remove %1 from your computer.").arg(actionText));
        }
        generateContextMenu();
        if (uDisks2::getRemovableDevices().size()==0 && tray->isVisible()) { tray->hide(); }
    } else {
        QStringList mounted = uDisks2::mountDevice(path);
        generateContextMenu();
        if (!mounted.at(0).isEmpty()) {
            QProcess::startDetached(QString("lumina-open %1").arg(mounted.at(0)));
        } else {
            QString msg = tr("Failed to mount volume %1, this is probably due to an unsupported file system (%2) or permissions.").arg(actionText).arg(uDisks2::getFileSystem(path));
            if (mounted.size()>1) { msg = mounted.at(1); }
            if (tray->isVisible()) { tray->showMessage(tr("Failed to mount %1").arg(actionText), msg);}
        }
    }
}
