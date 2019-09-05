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

#ifndef MDISUBWINDOW_H
#define MDISUBWINDOW_H

#include <QObject>
#include <QWidget>
#include <QMdiSubWindow>
#include <QCloseEvent>

class MdiSubWindow : public QMdiSubWindow
{
    Q_OBJECT

public:
    MdiSubWindow(QWidget *parent = nullptr,
                 Qt::WindowFlags flags = Qt::WindowFlags());

protected:
    void closeEvent(QCloseEvent *e);
};

#endif // SUBWINDOW_H
