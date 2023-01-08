/*
#
# Cyan Graphics - https://cyan.graphics - https://github.com/rodlie/cyan
#
# Copyright (c) Ole-André Rodlie <ole.andre.rodlie@gmail.com>. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
#
*/

#ifndef MDI_H
#define MDI_H

#include <QMdiArea>
#include <QList>
#include <QUrl>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>

namespace Cyan
{
    class Mdi : public QMdiArea
    {
        Q_OBJECT

    public:
        Mdi(QWidget *parent = nullptr);

    signals:
        void dropped(const QList<QUrl> &urls);

    protected:
        void dragEnterEvent(QDragEnterEvent *e);
        void dragMoveEvent(QDragMoveEvent *e);
        void dragLeaveEvent(QDragLeaveEvent *e);
        void dropEvent(QDropEvent *e);
    };
}

#endif // MDI_H
