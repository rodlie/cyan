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
#include <QTabWidget>
#include <QTreeWidget>

#include "qtwindowlistmenu.h"

#include "engine.h"

#define THEME_ICON_APPLICATIONS_GRAPHICS "applications-graphics"
#define THEME_ICON_DOCUMENT_OPEN "document-open"
#define THEME_ICON_VIDEO_DISPLAY "video-display"
#define THEME_ICON_PREFERENCES_COLOR "preferences-color"

#define CYAN_ICON "Cyan"
#define CYAN_ICON_SUBWINDOW "image-x-generic"
#define CYAN_ICON_COLOR_WHEEL "colors"
#define CYAN_ICON_OPEN_IMAGE THEME_ICON_DOCUMENT_OPEN
#define CYAN_ICON_DISPLAY THEME_ICON_VIDEO_DISPLAY
#define CYAN_ICON_ZOOM "zoom"

#define CYAN_PROFILE_FALLBACK_RGB ":/icc/rgb.icc"
#define CYAN_PROFILE_FALLBACK_CMYK ":/icc/cmyk.icc"
#define CYAN_PROFILE_FALLBACK_GRAY ":/icc/gray.icc"

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
        void zoomChanged(double scaleX,
                         double scaleY);
        void dropped(const QList<QUrl> &urls);

    public slots:
        void setZoom(double scaleX,
                     double scaleY);
        void setFit(bool value);
        void setImage(const Engine::Image &image);
        const QString getSourceDetails();
        const QByteArray getSourceProfile();

    private:
        bool _fit;
        bool _native;
        QString _sourceDetails;
        QByteArray _sourceProfile;

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
        void closed(const QString &filename);

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
        QSplitter *_splitterMiddle;
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
        QMenu *_menuColorDisplay;
        QMenu *_menuZoom;
        QAction *_menuColorBlackPoint;
        QActionGroup *_menuColorRGBGroup;
        QActionGroup *_menuColorCMYKGroup;
        QActionGroup *_menuColorGRAYGroup;
        QActionGroup *_menuColorDisplayGroup;
        QActionGroup *_menuColorIntentGroup;
        QToolButton *_menuColorButton;
        QToolButton *_menuColorDisplayButton;
        QToolButton *_menuZoomButton;
        QtWindowListMenu *_menuWindows;
        QAction *_actionOpenImage;
        QTabWidget *_tabs;
        QTreeWidget *_tabDetails;
        QString _lastTab;

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
                                      Engine::colorSpace colorspace,
                                      bool isDisplay = false);
        void populateColorIntentMenu();
        void handleColorProfileTriggered();
        void handleColorProfileDisplayTriggered();
        void handleColorIntentTriggered();
        void handleColorBlackPointTriggered();
        void handleColorDisplayButtonTriggered(bool checked);
        bool isFileOpen(const QString &filename);
        Cyan::MdiSubWindow* getTab(const QString &filename);
        void handleOpenImageReady(const Engine::Image &image);
        void handleWindowActivated(QMdiSubWindow *window);
        void setImageSourceDetails(const QString &info);
        void handleClosedWindow(const QString &filename);
        const QString getDefaultColorProfile(const Engine::colorSpace &cs,
                                             bool isDisplay = false);
        void setDefaultColorProfile(const Engine::colorSpace &cs,
                                    const QString &filename,
                                    bool isDisplay = false);
        void loadColorSettings();
        void saveColorSettings();
        void loadUISettings();
        void saveUISettings();
        void loadSettings();
        void saveSettings();
        bool canApplyDisplayProfile();
    };
}

#endif // CYAN_H
