/*
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "systray.h"
#include <QIcon>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include <QDebug>

SysTray::SysTray(QObject *parent)
    : QObject(parent)
    , tray(0)
    , menu(0)
    , man(0)
    , hasBattery(false)
    , hasPower(false)
{
    menu = new QMenu();

    tray = new QSystemTrayIcon(QIcon::fromTheme("battery"), this);
    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
    connect(tray, SIGNAL(messageClicked()), this, SLOT(handleTrayMessageClicked()));

    man = new Manager(this);
    connect(man, SIGNAL(updatedDevices()), this, SLOT(checkDevices()));
    generateContextMenu();
}

void SysTray::generateContextMenu()
{
    for(int i=0;i<menu->actions().size();i++) {
        menu->actions().at(i)->disconnect();
        delete menu->actions().at(i);
    }
    menu->clear();

    // TODO!

    handleShowHideTray();
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

void SysTray::handleTrayMessageClicked()
{
    handleShowHideTray();
}

void SysTray::showMessage(QString title, QString message)
{
    if (!tray->isSystemTrayAvailable()) { return; }
    if (!tray->isVisible()) { tray->show(); }
    tray->showMessage(title, message);
    QTimer::singleShot(10000, this, SLOT(handleShowHideTray()));
}

void SysTray::handleShowHideTray()
{
    if (!hasBattery) {
        if (tray->isVisible()) { tray->hide(); }
    } else {
        if (!tray->isVisible() && tray->isSystemTrayAvailable()) { tray->show(); }
    }
}

void SysTray::checkDevices()
{
    qDebug() << "something changed ...";
    double batteryLeft = 0;
    QMapIterator<QString, Device*> device(man->devices);
    while (device.hasNext()) {
        device.next();
        if (device.value()->isBattery && device.value()->isPresent) {
            if (!hasBattery) { hasBattery = true; }
            batteryLeft += device.value()->percentage;
        } else if (device.value()->isAC) {
            hasPower = device.value()->isPresent;
        }
    }

    tray->setToolTip(tr("Battery at %1%").arg(batteryLeft));
    handleShowHideTray();
}
