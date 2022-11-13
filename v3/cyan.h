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
#include <QMdiSubWindow>
#include <QCloseEvent>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QGraphicsScene>

#include "qtwindowlistmenu.h"

#include "engine.h"

namespace Cyan
{
    class BasicView : public QGraphicsView
    {
        Q_OBJECT

    public:
        explicit BasicView(QWidget* parent = nullptr,
                           bool fit = false,
                           bool native = false);
        bool isFit();

    signals:
        void resetZoom();
        void zoomChanged(double scaleX, double scaleY);
        void dropped(const QList<QUrl> &urls);

    public slots:
        void setZoom(double scaleX, double scaleY);
        void setFit(bool value);
        void setImage(const QByteArray &buffer,
                      int width,
                      int height);

    private:
        bool _fit;
        bool _native;

    protected:
        void wheelEvent(QWheelEvent* e);
        void mousePressEvent(QMouseEvent *e);
        void dragEnterEvent(QDragEnterEvent *e);
        void dragMoveEvent(QDragMoveEvent *e);
        void dragLeaveEvent(QDragLeaveEvent *e);
        void dropEvent(QDropEvent *e);
        void resizeEvent(QResizeEvent *e);
    };

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

    class MdiSubWindow : public QMdiSubWindow
    {
        Q_OBJECT

    public:
        MdiSubWindow( QWidget *parent = nullptr,
                      const QString &filename = QString(),
                      Qt::WindowFlags flags = Qt::WindowFlags() );
        const QString getFilename();
        BasicView* getView();

    signals:

    private:
        QString _filename;
        BasicView *_view;
        QGraphicsScene *_scene;

    protected:
        void closeEvent(QCloseEvent *e);
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
        void readImage(const QString &filename);

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
        bool isFileOpen(const QString &filename);
        void handleOpenImageReady(const Engine::Image &image);
    };
}

#endif // CYAN_H
