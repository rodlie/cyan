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
#include <QMap>
#include "udisks2.h"

class SysTray : public QObject
{
    Q_OBJECT
public:
    explicit SysTray(QObject *parent = NULL);
private:
    QSystemTrayIcon *tray;
    QMenu *contextMenu;
    QStringList monitoredDevices;
signals:
    void needDeviceList();
private slots:
    void setupDbus();
    void deviceAdded(const QDBusObjectPath &obj);
    void deviceRemoved(const QDBusObjectPath &obj);
    void generateContextMenu();
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void handleMessageClicked();
    void handleContextMenuAction();
    void handlePropertiesChanged(const QString &interface, const QMap<QString, QVariant> &changedProperties);
};

#endif // SYSTRAY_H
