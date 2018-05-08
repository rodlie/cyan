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
#include <QPainter>
#include "common.h"

SysTray::SysTray(QObject *parent)
    : QObject(parent)
    , tray(0)
    , trayText(0)
    , menu(0)
    , man(0)
    , pm(0)
    , wasLowBattery(false)
    , lowBatteryValue(LOW_BATTERY)
    , critBatteryValue(CRITICAL_BATTERY)
    , hasService(false)
    , lidActionBattery(LID_BATTERY_DEFAULT)
    , lidActionAC(LID_AC_DEFAULT)
    , criticalAction(CRITICAL_DEFAULT)
{
    menu = new QMenu();

    tray = new QSystemTrayIcon(QIcon::fromTheme("battery"), this);
    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
    connect(tray, SIGNAL(messageClicked()), this, SLOT(handleTrayMessageClicked()));
    if (tray->isSystemTrayAvailable()) { tray->show(); }

    trayText = new QSystemTrayIcon(this);
    if (trayText->isSystemTrayAvailable()) { trayText->show(); }

    man = new Manager(this);
    connect(man, SIGNAL(updatedDevices()), this, SLOT(checkDevices()));
    connect(man, SIGNAL(closedLid()), this, SLOT(handleClosedLid()));
    connect(man, SIGNAL(openedLid()), this, SLOT(handleOpenedLid()));
    connect(man, SIGNAL(switchedToBattery()), this, SLOT(handleOnBattery()));
    connect(man, SIGNAL(switchedToAC()), this, SLOT(handleOnAC()));
    //connect(man, SIGNAL(lowBattery(bool)), this, SLOT(handleLowBattery(bool)));

    pm = new PowerManagement();
    connect(pm, SIGNAL(HasInhibitChanged(bool)), this, SLOT(handleHasInhibitChanged(bool)));
    connect(pm, SIGNAL(update()), this, SLOT(loadSettings()));

    generateContextMenu();
    loadSettings();
    registerService();
    QTimer::singleShot(1000, this, SLOT(checkDevices()));
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
    //if (!tray->isVisible() && tray->isSystemTrayAvailable()) { tray->show(); }
}

void SysTray::checkDevices()
{
    double batteryLeft = man->batteryLeft();
    tray->setToolTip(tr("Battery at %1%").arg(batteryLeft));
    if (batteryLeft==100) { tray->setToolTip(tr("Charged")); }
    if (!man->onBattery() && batteryLeft<100) { tray->setToolTip(tray->toolTip().append(tr(" (Charging)"))); }

    // TODO: settings!
    QPixmap pixmap(24,24);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    //painter.setPen(QColor(Qt::white));
    painter.drawText(pixmap.rect(), Qt::AlignCenter, QString("%1%").arg(batteryLeft));
    trayText->setIcon(pixmap);
    if (batteryLeft>0 && batteryLeft<100) {
        if (!trayText->isVisible()) { trayText->show(); }
    } else {
        if (trayText->isVisible()) { trayText->hide(); }
    }

    // set tray icon based on battery level
    if (batteryLeft<=(double)lowBatteryValue && man->onBattery()) { handleLowBattery(true); }
    else { handleLowBattery(false); }

    // critical battery level
    if (batteryLeft<=(double)critBatteryValue && man->onBattery()) { handleCritical(); }

    if (!hasService) { registerService(); }

    //handleShowHideTray();
}

void SysTray::handleClosedLid()
{
    int type = lidNone;
    if (man->onBattery()) { type = lidActionBattery; }
    else { type = lidActionAC; }
    switch(type) {
    case lidLock:
        man->lockScreen();
        break;
    case lidSleep:
        man->suspend();
        break;
    case lidHibernate:
        man->hibernate();
        break;
    default: ;
    }
}

void SysTray::handleOpenedLid()
{
    qDebug() << "user opened lid, should we do something?";
}

void SysTray::handleOnBattery()
{
    tray->showMessage(tr("On Battery"), tr("Switched to battery power."));
}

void SysTray::handleOnAC()
{
    tray->showMessage(tr("On AC"), tr("Switched to AC power."));
}

void SysTray::handleLowBattery(bool low)
{
    if (low && !wasLowBattery) {
        tray->setIcon(QIcon::fromTheme("battery-caution"));
    } else if (!low && wasLowBattery) {
        tray->setIcon(QIcon::fromTheme("battery"));
    }
    wasLowBattery = low;
}

void SysTray::loadSettings()
{
    qDebug() << "load settings";
    if (Common::validPowerSettings("lowBattery")) {
        lowBatteryValue = Common::loadPowerSettings("lowBattery").toInt();
    }
    if (Common::validPowerSettings("criticalBattery")) {
        critBatteryValue = Common::loadPowerSettings("criticalBattery").toInt();
    }
    if (Common::validPowerSettings("lidBattery")) {
        lidActionBattery = Common::loadPowerSettings("lidBattery").toInt();
    }
    if (Common::validPowerSettings("lidAC")) {
        lidActionAC = Common::loadPowerSettings("lidAC").toInt();
    }
    if (Common::validPowerSettings("criticalAction")) {
        criticalAction = Common::loadPowerSettings("criticalAction").toInt();
    }
    qDebug() << "low battery setting" << lowBatteryValue;
    qDebug() << "critical battery setting" << critBatteryValue;
    qDebug() << "lid battery" << lidActionBattery;
    qDebug() << "lid ac" << lidActionAC;
    qDebug() << "critical action" << criticalAction;
}

void SysTray::registerService()
{
    if (hasService) { return; }
    if (!QDBusConnection::sessionBus().isConnected()) {
        qWarning("Cannot connect to D-Bus.");
        return;
    }
    if (!QDBusConnection::sessionBus().registerService(SERVICE)) {
        qWarning() << QDBusConnection::sessionBus().lastError().message();
        return;
    }
    if (!QDBusConnection::sessionBus().registerObject("/PowerManagement", pm, QDBusConnection::ExportAllContents)) {
        qWarning() << QDBusConnection::sessionBus().lastError().message();
        return;
    }
    hasService = true;
}

void SysTray::handleHasInhibitChanged(bool has_inhibit)
{
    qDebug() << "HasInhibitChanged" << has_inhibit;
}

void SysTray::handleCritical()
{
    qDebug() << "critical battery level, action?" << criticalAction;
    switch(criticalAction) {
    case criticalHibernate:
        man->hibernate();
        break;
    case criticalShutdown:
        qDebug() << "feature not added!"; // TODO!!!!
        break;
    default: ;
    }
}
