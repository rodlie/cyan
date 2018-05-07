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

class SysTray : public QObject
{
    Q_OBJECT
public:
    explicit SysTray(QObject *parent = NULL);
private:
    QSystemTrayIcon *tray;
    QSystemTrayIcon *trayText;
    QMenu *menu;
    Manager *man;
    bool wasLowBattery;
    int lowBatteryValue;
    int critBatteryValue;
private slots:
    void generateContextMenu();
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void handleTrayMessageClicked();
    void showMessage(QString title, QString message);
    void handleShowHideTray();
    void checkDevices();
    void handleClosedLid();
    void handleOpenedLid();
    void handleOnBattery();
    void handleOnAC();
    void handleLowBattery(bool low);
    void loadSettings();
};

#endif // SYSTRAY_H
