/*
#
# Cyan <https://cyan.fxarena.net>
# Copyright Ole-André Rodlie, FxArena DA.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
*/

#ifndef CYANABOUTDIALOG_H
#define CYANABOUTDIALOG_H

#include <QObject>
#include <QDialog>

#include "CyanGlobal.h"

class CYANSHARED_EXPORT CyanAboutDialog : public QDialog
{
    Q_OBJECT

public:
    CyanAboutDialog(QWidget *parent = nullptr);

private slots:
    void qtButtonTriggered();
    void ghButtonTriggered();
};

#endif // CYANABOUTDIALOG_H
