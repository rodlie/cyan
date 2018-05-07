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
#include <QSettings>

#define LOW_BATTERY 15
#define CRITICAL_BATTERY 10

SysTray::SysTray(QObject *parent)
    : QObject(parent)
    , tray(0)
    , menu(0)
    , man(0)
    , wasLowBattery(false)
    , lowBatteryValue(0)
    , critBatteryValue(0)
{
    menu = new QMenu();

    tray = new QSystemTrayIcon(QIcon::fromTheme("battery"), this);
    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
    connect(tray, SIGNAL(messageClicked()), this, SLOT(handleTrayMessageClicked()));

    man = new Manager(this);
    connect(man, SIGNAL(updatedDevices()), this, SLOT(checkDevices()));
    connect(man, SIGNAL(closedLid()), this, SLOT(handleClosedLid()));
    connect(man, SIGNAL(openedLid()), this, SLOT(handleOpenedLid()));
    connect(man, SIGNAL(switchedToBattery()), this, SLOT(handleOnBattery()));
    connect(man, SIGNAL(switchedToAC()), this, SLOT(handleOnAC()));
    //connect(man, SIGNAL(lowBattery(bool)), this, SLOT(handleLowBattery(bool)));
    generateContextMenu();
    loadSettings();
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
    /*if (!hasBattery) {
        if (tray->isVisible()) { tray->hide(); }
    } else {
        if (!tray->isVisible() && tray->isSystemTrayAvailable()) { tray->show(); }
    }*/
    if (!tray->isVisible() && tray->isSystemTrayAvailable()) { tray->show(); }
}

void SysTray::checkDevices()
{
    double batteryLeft = man->batteryLeft();
    tray->setToolTip(tr("Battery at %1%").arg(batteryLeft));

    // set tray icon based on battery level
    if (batteryLeft<=(double)lowBatteryValue && man->onBattery()) { handleLowBattery(true); }
    else { handleLowBattery(false); }

    // hibernate on critical battery level (10% as default)
    if (batteryLeft<=(double)critBatteryValue && man->onBattery() && man->canHibernate()) { man->hibernate(); }

    handleShowHideTray();
}

void SysTray::handleClosedLid()
{
    qDebug() << "user closed lid";
    if (man->onBattery()) {

    } else {
        // TODO: lock screen
    }
}

void SysTray::handleOpenedLid()
{
    qDebug() << "user opened lid";
}

void SysTray::handleOnBattery()
{
    qDebug() << "switched to battery power";
}

void SysTray::handleOnAC()
{
    qDebug() << "switched to ac power";
}

void SysTray::handleLowBattery(bool low)
{
    if (low && !wasLowBattery) {
        tray->setIcon(QIcon::fromTheme("battery-caution"));
    } else if (!low && wasLowBattery) {
        tray->setIcon(QIcon::fromTheme("battery"));
    }
}

void SysTray::loadSettings()
{
    QSettings settings("lumina-desktop", "lumina-power-manager");
    settings.beginGroup("battery");
    if (settings.value("low").isValid()) { lowBatteryValue = settings.value("low").toInt(); }
    else { lowBatteryValue = LOW_BATTERY; }
    if (settings.value("critical").isValid()) { critBatteryValue = settings.value("critical").toInt(); }
    else { critBatteryValue = CRITICAL_BATTERY; }
    settings.endGroup();
}
