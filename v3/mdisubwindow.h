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

#ifndef MDISUBWINDOW_H
#define MDISUBWINDOW_H

#include <QString>
#include <QMdiSubWindow>
#include <QGraphicsScene>
#include <QCloseEvent>


#include "engine.h"
#include "basicview.h"

namespace Cyan
{
    class MdiSubWindow : public QMdiSubWindow
    {
        Q_OBJECT

    public:
        MdiSubWindow( QWidget *parent = nullptr,
                      const QString &filename = QString(),
                      const Engine::ColorSettings &colorSettings = Engine::ColorSettings(),
                      Qt::WindowFlags flags = Qt::WindowFlags() );
        const QString getFilename();
        void setColorSpace(const Engine::ColorSpace &colorspace);
        Engine::ColorSpace getColorSpace();
        void setColorIntent(const Engine::RenderingIntent &intent);
        Engine::RenderingIntent getColorIntent();
        void setColorBlackPoint(bool blackpoint);
        bool getColorBlackPoint();
        void setColorProfileRGB(const QString &profile);
        void setColorProfileCMYK(const QString &profile);
        void setColorProfileGRAY(const QString &profile);
        void setColorProfileDisplay(const QString &profile);
        void setProxy(int proxy);
        const QString getColorProfileRGB();
        const QString getColorProfileCMYK();
        const QString getColorProfileGRAY();
        const QString getColorProfileDisplay();
        int getProxy();
        void setColorSettings(const Engine::ColorSettings &settings,
                              bool forceColorspace = false);
        const Engine::ColorSettings getColorSettings();
        BasicView* getView();

    signals:
        void closed(const QString &filename);

    private:
        QString _filename;
        Engine::ColorSettings _colorSettings;
        BasicView *_view;
        QGraphicsScene *_scene;

    protected:
        void closeEvent(QCloseEvent *e);
    };
}

#endif // MDISUBWINDOW_H
