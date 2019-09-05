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

#include "CyanMdiWindow.h"
#include <QDebug>
#include <QMessageBox>

#include "CyanView.h"

MdiSubWindow::MdiSubWindow(QWidget *parent,
                           Qt::WindowFlags flags)
    : QMdiSubWindow(parent, flags)
{
}

void MdiSubWindow::closeEvent(QCloseEvent *e)
{
    qDebug() << "tab close event!";
    View *view = qobject_cast<View*>(widget());
    if (view && view->isDirty()) {
        int ret = QMessageBox::question(this,
                                        tr("Unsaved changes"),
                                        tr("This project has unsaved changes, "
                                           "are you sure you want to close?"),
                                        QMessageBox::Yes|QMessageBox::No,
                                        QMessageBox::No);
        if (ret == QMessageBox::No) { e->ignore(); }
        else { e->accept(); }
    } else { e->accept(); }
}
