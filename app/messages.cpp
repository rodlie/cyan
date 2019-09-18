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

#include "editor.h"
#include <QMessageBox>

void Editor::handleError(const QString &message)
{
    qDebug() << "error" << message;
    mainStatusBar->showMessage(message, 6000);
    QMessageBox::warning(this,
                         tr("Error"),
                         message);
}

void Editor::handleWarning(const QString &message)
{
    qDebug() << "warning" << message;
    mainStatusBar->showMessage(message);
    /*QMessageBox::warning(this,
                         tr("Cyan Warning"),
                         message);*/
}

void Editor::handleStatus(const QString &message)
{
    qDebug() << "status" << message;
    mainStatusBar->showMessage(message,
                               6000);
}
