/*
#
# Cyan - https://github.com/rodlie/cyan
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

#include "cyan.h"

#include <QMimeData>

using namespace Cyan;

Mdi::Mdi(QWidget *parent)
    : QMdiArea(parent)
{
    setAcceptDrops(true);
}

void
Mdi::dragEnterEvent(QDragEnterEvent *e)
{
    e->acceptProposedAction();
}

void
Mdi::dragMoveEvent(QDragMoveEvent *e)
{
    e->acceptProposedAction();
}

void
Mdi::dragLeaveEvent(QDragLeaveEvent *e)
{
    e->accept();
}

void
Mdi::dropEvent(QDropEvent *e)
{
    if ( e->mimeData()->hasUrls() ) { emit dropped( e->mimeData()->urls() ); }
}

