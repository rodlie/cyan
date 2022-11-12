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

#ifndef CYAN_H
#define CYAN_H

#include <QMainWindow>
#include <QMdiArea>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QUrl>
#include <QToolBar>
#include <QSplitter>
#include <QStatusBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QToolButton>

#include "qtwindowlistmenu.h"

#include "engine.h"

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
        void dragEnterEvent(QDragEnterEvent *event);
        void dragMoveEvent(QDragMoveEvent *event);
        void dragLeaveEvent(QDragLeaveEvent *event);
        void dropEvent(QDropEvent *event);
    };
    class Window : public QMainWindow
    {
        Q_OBJECT

    public:
        Window(QWidget *parent = nullptr);
        ~Window();

    public slots:
        void openImage( bool showDialog = true,
                        const QString &filename = QString() );

    signals:
        void openImageReady(const Engine::Image &image);

    private:
        Mdi *_mdi;
        QSplitter *_splitter;
        QSplitter *_splitterLeft;
        QSplitter *_splitterRight;
        QToolBar *_toolbar;
        QStatusBar *_statusbar;
        QMenuBar *_menubar;
        QMenu *_menuFile;
        QMenu *_menuView;
        QMenu *_menuHelp;
        QMenu *_menuColor;
        QMenu *_menuColorRGB;
        QMenu *_menuColorCMYK;
        QMenu *_menuColorGRAY;
        QMenu *_menuColorIntent;
        QAction *_menuColorBlackPoint;
        QActionGroup *_menuColorRGBGroup;
        QActionGroup *_menuColorCMYKGroup;
        QActionGroup *_menuColorGRAYGroup;
        QToolButton *_menuColorButton;
        QtWindowListMenu *_menuWindows;
        QAction *_actionOpenImage;

    private slots:
        void setupUi();
        void setupMenus();
        void setupActions();
        void setupTheme(bool native = false,
                        bool nativeIcons = false);
        void handleDropped(const QList<QUrl> &urls);
        void handleActionOpenImage();
        void populateColorProfileMenu(QMenu *menu,
                                      QActionGroup *group,
                                      Engine::colorSpace colorspace);
        void handleColorProfileTriggered();
        void populateColorIntentMenu();
        void setDefaultColorIntent();
    };
}

#endif // CYAN_H
