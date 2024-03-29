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

#ifndef CYAN_H
#define CYAN_H

#include <QMainWindow>
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
#include <QTabWidget>
#include <QTreeWidget>
#include <QComboBox>

#include "qtwindowlistmenu.h"

#include "engine.h"
#include "mdi.h"
#include "mdisubwindow.h"

namespace Cyan
{
    class Window : public QMainWindow
    {
        Q_OBJECT

    public:
        Window(QWidget *parent = nullptr);
        ~Window();

    public slots:
        void openImage( bool showDialog = true,
                        const QString &filename = QString() );
        void saveImage();
        void readImage(const QString &filename,
                       const Engine::ColorSettings &cs);

        void applyDisplayProfile(const QString &filename,
                                 const Engine::ColorProfiles &profiles);
        void updateDisplayProfile();
        void updateDisplayProfile(const QString &filename,
                                  const Engine::ColorSpace &colorspace);
        void clearDisplayProfile(const QString &filename,
                                 const Engine::ColorSettings &cs);
        void resetDisplayProfile();
        void resetDisplayProfile(const QString &filename);

    signals:
        void openImageReady(const Engine::Image &image);
        void applyDisplayProfileReady(const Engine::Image &image);
        void clearDisplayProfileReady(const Engine::Image &image);
        void applyPrintProfileReady(const Engine::Image &image);
        void clearPrintProfileReady(const Engine::Image &image);
        void showStatusMessage(const QString &message, int timeout = 0);

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
        QMenu *_menuColorPrint;
        QMenu *_menuZoom;
        QAction *_menuColorBlackPoint;
        QActionGroup *_menuColorRGBGroup;
        QActionGroup *_menuColorCMYKGroup;
        QActionGroup *_menuColorGRAYGroup;
        QActionGroup *_menuColorDisplayGroup;
        QActionGroup *_menuColorPrintGroup;
        QActionGroup *_menuColorIntentGroup;
        QToolButton *_menuColorButton;
        QToolButton *_menuColorDisplayButton;
        QToolButton *_menuColorPrintButton;
        QToolButton *_menuZoomButton;
        QtWindowListMenu *_menuWindows;
        QAction *_actionOpenImage;
        QAction *_actionSaveImage;
        QAction *_actionAbout;
        QTabWidget *_tabs;
        QTreeWidget *_tabImageDetails;
        QTreeWidget *_tabPrintDetails;
        QString _lastTab;
        QComboBox *_proxy;

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
                                      Engine::ColorSpace colorspace,
                                      bool isDisplay = false,
                                      bool isPrint = false);
        void populateColorIntentMenu();
        void handleColorProfileTriggered();
        void handleColorProfileDisplayTriggered();
        void handleColorProfilePrintTriggered();
        void handleColorIntentTriggered();
        void handleColorBlackPointTriggered();
        void handleColorDisplayButtonTriggered(bool checked);
        void handleColorPrintButtonTriggered(bool checked);
        bool isFileOpen(const QString &filename);
        Cyan::MdiSubWindow* getTab(const QString &filename);
        Cyan::MdiSubWindow* getCurrentTab();
        void handleOpenImageReady(const Engine::Image &image);
        void handleUpdateImageReady(const Engine::Image &image);
        void handleWindowActivated(QMdiSubWindow *window);
        void setDetails(QTreeWidget *tree,
                        const QString &info);
        void handleClosedWindow(const QString &filename);
        void setDefaultColorProfile(const Engine::ColorSpace &colorspace,
                                    const QString &filename,
                                    bool isDisplay = false,
                                    bool isPrint = false);
        void loadColorSettings();
        void saveColorSettings(bool forceSync = false);
        const Engine::ColorSettings getColorSettings();
        void loadUISettings();
        void saveUISettings();
        void loadSettings();
        void saveSettings();
        bool canApplyDisplayProfile();
        bool canApplyPrintProfile();
        void setPrintDetailsVisibility(bool visible);
        bool colorSettingsDiffer(const Engine::ColorSettings &cs,
                                 bool checkColorspace = false,
                                 bool checkDisplay = false,
                                 bool checkPrint = false);
        void setColorActionsEnabled(bool enabled);
        void openAboutDialog();
        void openConvertDialog(const QString &filename);
        void handleProxyChanged(int index);
        void updateRecentLoadDirectory(const QString &path);
        const QString getRecentLoadDirectory();
        void updateRecentSaveDirectory(const QString &path);
        const QString getRecentSaveDirectory();
        void updateConfigPath(const QString &grp, const QString &key, const QString &val);
        const QString getConfig(const QString &grp, const QString &key, const QString &def);
    };
}

#endif // CYAN_H
