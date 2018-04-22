/*
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "systray.h"
#include <QIcon>
#include "common.h"
#include "dialog.h"

SysTray::SysTray(QObject *parent)
    : QObject(parent)
    , tray(0)
{
    tray = new QSystemTrayIcon(QIcon::fromTheme("keyboard"), this);
    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(handleTray(QSystemTrayIcon::ActivationReason)));
    if (tray->isSystemTrayAvailable()) { tray->show(); }
    Common::loadKeyboard();
}

void SysTray::handleTray(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason) {
    case QSystemTrayIcon::Context:
    case QSystemTrayIcon::Trigger:
        openDialog();
    default:;
    }
}

void SysTray::openDialog()
{
    Dialog *dialog = new Dialog();
    dialog->show();
}
