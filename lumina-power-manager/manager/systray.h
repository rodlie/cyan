/*
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef SYSTRAY_H
#define SYSTRAY_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include "manager.h"
#include "powermanagement.h"
#include <QTimer>

class SysTray : public QObject
{
    Q_OBJECT
public:
    explicit SysTray(QObject *parent = NULL);
private:
    QSystemTrayIcon *tray;
    QMenu *menu;
    Manager *man;
    PowerManagement *pm;
    bool wasLowBattery;
    int lowBatteryValue;
    int critBatteryValue;
    bool hasService;
    int lidActionBattery;
    int lidActionAC;
    int criticalAction;
    int autoSleepBattery;
    int autoSleepAC;
    QTimer *timer;
    int timeouts;
private slots:
    void generateContextMenu();
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void showMessage(QString title, QString message);
    void checkDevices();
    void handleClosedLid();
    void handleOpenedLid();
    void handleOnBattery();
    void handleOnAC();
    void loadSettings();
    void registerService();
    void handleHasInhibitChanged(bool has_inhibit);
    void handleCritical();
    void drawBattery(double left);
    void timeout();
    int xIdle();
    void resetTimer();
};

#endif // SYSTRAY_H
