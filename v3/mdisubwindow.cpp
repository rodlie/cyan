/*
#
# Cyan - https://github.com/rodlie/cyan
#
# Copyright (c) 2020-2022 Ole-André Rodlie. All rights reserved.
# Copyright (c) 2018-2019 Ole-André Rodlie, FxArena. All rights reserved.
# Copyright (c) 2016-2017 Ole-André Rodlie, INRIA. All rights reserved.
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

#include <QFileInfo>

using namespace Cyan;

MdiSubWindow::MdiSubWindow(QWidget *parent,
                           const QString &filename,
                           Qt::WindowFlags flags)
    : QMdiSubWindow(parent, flags)
    , _filename(filename)
    , _view(nullptr)
    , _scene(nullptr)
{
    _scene = new QGraphicsScene(this);
    _view = new BasicView(this, true);
    _view->setScene(_scene);
    setWidget(_view);
    QFileInfo fileInfo(_filename);
    setWindowTitle( fileInfo.completeBaseName() );
}

const QString
MdiSubWindow::getFilename()
{
    return _filename;
}

BasicView *
MdiSubWindow::getView()
{
    return _view;
}

void
MdiSubWindow::closeEvent(QCloseEvent *e)
{
    QMdiSubWindow::closeEvent(e);
}
