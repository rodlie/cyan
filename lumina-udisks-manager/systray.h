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
    QMenu *menu;
    Manager *man;
private slots:
    void generateContextMenu();
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void handleMessageClicked();
    void handleContextMenuAction();
    void handleDeviceError(QString path, QString error);
    void handleDeviceMediaChanged(QString path, bool media);
    void handleDeviceMountpointChanged(QString path, QString mountpoint);
};

#endif // SYSTRAY_H
