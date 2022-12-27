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

#include <QFileInfo>

using namespace Cyan;

MdiSubWindow::MdiSubWindow(QWidget *parent,
                           const QString &filename,
                           const Engine::ColorSettings &colorSettings,
                           Qt::WindowFlags flags)
    : QMdiSubWindow(parent, flags)
    , _filename(filename)
    , _colorSettings(colorSettings)
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

void
MdiSubWindow::setColorSpace(const Engine::colorSpace &colorSpace)
{
    _colorSettings.colorspace = colorSpace;
}

const Engine::colorSpace
MdiSubWindow::getColorSpace()
{
    return _colorSettings.colorspace;
}

void
MdiSubWindow::setColorIntent(const Engine::RenderingIntent &intent)
{
    _colorSettings.intent = intent;
}

const Engine::RenderingIntent
MdiSubWindow::getColorIntent()
{
    return _colorSettings.intent;
}

void
MdiSubWindow::setColorBlackPoint(bool blackpoint)
{
    _colorSettings.blackpoint = blackpoint;
}

bool
MdiSubWindow::getColorBlackPoint()
{
    return _colorSettings.blackpoint;
}

void
MdiSubWindow::setColorProfileRGB(const QString &profile)
{
    _colorSettings.profiles.rgb = profile;
}

void
MdiSubWindow::setColorProfileCMYK(const QString &profile)
{
    _colorSettings.profiles.cmyk = profile;
}

void
MdiSubWindow::setColorProfileGRAY(const QString &profile)
{
    _colorSettings.profiles.gray = profile;
}

void
MdiSubWindow::setColorProfileDisplay(const QString &profile)
{
    _colorSettings.profiles.display = profile;
}

const QString
MdiSubWindow::getColorProfileRGB()
{
    return _colorSettings.profiles.rgb;
}

const QString
MdiSubWindow::getColorProfileCMYK()
{
    return _colorSettings.profiles.cmyk;
}

const QString
MdiSubWindow::getColorProfileGRAY()
{
    return _colorSettings.profiles.gray;
}

const QString
MdiSubWindow::getColorProfileDisplay()
{
    return _colorSettings.profiles.display;
}

void
MdiSubWindow::setColorSettings(const Engine::ColorSettings &settings,
                               bool forceColorspace)
{
    Engine::colorSpace cs = _colorSettings.colorspace;
    _colorSettings = settings;
    if (!forceColorspace) {
        _colorSettings.colorspace = cs; // keep image colorspace unless forced
    }
}

const Engine::ColorSettings
MdiSubWindow::getColorSettings()
{
    return _colorSettings;
}

BasicView *
MdiSubWindow::getView()
{
    return _view;
}

void
MdiSubWindow::closeEvent(QCloseEvent *e)
{
    emit closed(_filename);
    QMdiSubWindow::closeEvent(e);
}
