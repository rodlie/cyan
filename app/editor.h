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

#ifndef EDITOR_H
#define EDITOR_H

#include <QMainWindow>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QActionGroup>
#include <QMdiArea>
#include <QDockWidget>
//#include <QMdiSubWindow>
#include <QComboBox>
#include <QStatusBar>
#include <QSlider>
#include <QMap>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QToolButton>
#include <QSplitter>
#include <QLabel>
#include <QResizeEvent>
#include <QCloseEvent>

#include "CyanImageFormat.h"
#include "CyanView.h"
#include "CyanPluginLoader.h"
#include "CyanMdi.h"
#include "CyanMdiWindow.h"
#include "CyanLayerWidget.h"

#include "cyan_common.h" // TODO replace

#include "qtcolorpicker.h"

//#include "cyan_textwidget.h"
#include "qtwindowlistmenu.h"

class Editor : public QMainWindow
{
    Q_OBJECT

public:

    Editor(QWidget *parent = nullptr);
    ~Editor();

private:
    CyanPlugins *pluginHandler;
    CyanCommon common;
    Mdi *mdi;

    QToolBar *mainToolBar;

    QMenuBar *mainMenu;
    QStatusBar *mainStatusBar;

    QAction *newImageAct;
    QAction *openImageAct;
    QAction *saveImageAct;
    QAction *saveProjectAct;
    QAction *saveProjectAsAct;
    QAction *newLayerAct;
    QAction *newTextLayerAct;
    QAction *openLayerAct;
    QAction *saveLayerAct;
    QAction *blackPointAct;
    QAction *quitAct;

    QAction *viewMoveAct;
    QAction *viewDrawAct;
    QAction *viewZoom100Act;
    QAction *viewZoomFitAct;

    QAction *aboutCyanAct;
    QAction *aboutImageMagickAct;
    QAction *aboutLcmsAct;
    QAction *aboutQtAct;

    QAction *convertRGBAct;
    QAction *convertCMYKAct;
    QAction *convertGRAYAct;
    QAction *convertAssignAct;
    QAction *convertExtractAct;

    QMenu *fileMenu;
    QMenu *optMenu;
    QMenu *helpMenu;
    //QMenu *newMenu;
    QMenu *saveMenu;
    QMenu *colorMenu;
    QMenu *colorProfileRGBMenu;
    QMenu *colorProfileCMYKMenu;
    QMenu *colorProfileGRAYMenu;
    QMenu *colorIntentMenu;
    //QMenu *layerMenu;
    QtWindowListMenu *windowsMenu;

    QToolButton *newButton;
    QToolButton *openButton;
    QToolButton *saveButton;
    QToolButton *moveButton;
    QToolButton *zoomButton;
    CyanLayerWidget *layersWidget;
   // QDockWidget *layersDock;

    //CyanTextWidget *textWidget;

    QSlider *brushSize;
    QDockWidget *brushDock;
    //QDockWidget *textDock;
    //QDockWidget *colorDock;

    //QtColorTriangle *colorTriangle;
    QtColorPicker *colorPicker;

    //QToolButton *textButton;
    QToolButton *convertButton;

    //ctkPopupWidget *textPopup;
    //ctkPopupWidget *layerPopup;
    //ctkPopupWidget *colorPopup;

    QLabel *currentZoomStatusIcon;
    QLabel *currentZoomStatusLabel;

    QSplitter *mainSplitter;
    QSplitter *rightSplitter;
    QSplitter *leftSplitter;

signals:

    void openImage(const QString &filename);
    void statusMessage(const QString &message);
    void errorMessage(const QString &message);
    void warningMessage(const QString &message);

public slots:

    View* getCurrentCanvas();

private slots:

    // plugins
    void initPlugins(QList<QPluginLoader*> plugins);
    void initWidgetPlugin(CyanWidgetPlugin* plugin);

    // setup
    void setupStyle();
    void setupUI();
    void setupMenus();
    void setupToolbars();
    void setupWidgets();
    void setupActions();
    void setupButtons();
    //void setupColorManagement();
    void setupConnections();
    void setupIcons();
    void setupShortcuts();
    void setupOptions();

    // about
    void aboutCyan();
    void aboutImageMagick();
    void aboutLcms();

    // messages
    void handleError(const QString &message);
    void handleWarning(const QString &message);
    void handleStatus(const QString &message);

    // view tools
    void handleSwitchMoveTool(View::InteractiveMode tool = View::InteractiveNoMode);
    void handleSetMoveMode(bool triggered);
    void handleSetDrawMode(bool triggered);
    void handleBrushSize();
    void handleUpdateBrushSize(int stroke);

    // layers
    void handleLayerCompChanged(Magick::CompositeOperator composite, int id);
    void handleLayersOpacity(double value, int id);
    void handleLayersUpdated();
    void handleLayerTreeSelectedLayer(int id);
    void handleLayerVisibility(int id, bool visible);
    void handleLayerLabel(int id, const QString &label);
    void handleLayerLock(int id, bool lock);
    void addLayerToView(Magick::Image image,
                        View *view);
    void handleRemoveLayer(int id);
    void handleMoveLayerDown(int id);
    void handleMoveLayerUp(int id);
    void handleDuplicateLayer(int id);

    // text
    void handleCurrentLayerTextChanged();

    // tabs
    void newTab(CyanImageFormat::CyanCanvas canvas);
    void newTab(Magick::Image image = Magick::Image(),
                QSize geo = QSize(0, 0));
    void handleTabActivated(QMdiSubWindow *tab);
    void updateTabTitle(View *view = nullptr);

    // color
    void populateColorProfileMenu(QMenu *menu,
                                  Magick::ColorspaceType colorspace);
    void selectDefaultColorProfile();
    void setDefaultColorProfiles(QMenu *menu);
    void setDefaultColorProfileFromFilename(QMenu *menu,
                                            const QString &filename);
    void setDefaultColorProfileFromTitle(QMenu *menu,
                                         const QString &title);
    void checkDefaultColorProfiles();
    const QString selectedDefaultColorProfile(QMenu *menu);
    Magick::Blob selectedDefaultColorProfileData(QMenu *menu);
    void populateColorIntentMenu();
    void setDefaultColorIntent();
    void loadDefaultColorIntent();
    void handleColorConvertRGB(bool ignoreColor = false,
                               const QString &title = tr("Convert to RGB"));
    void handleColorConvertCMYK(bool ignoreColor = false,
                                const QString &title = tr("Convert to CMYK"));
    void handleColorConvertGRAY(bool ignoreColor = false,
                                const QString &title = tr("Convert to GRAY"));
    void handleColorProfileAssign();
    void handleColorConvert(bool ignoreColor = false,
                            Magick::ColorspaceType colorspace = Magick::UndefinedColorspace,
                            const QString &title = tr("Convert"));
    void hasColorProfiles();
    void handleColorChanged(const QColor &color);

    //

    void saveSettings();
    void saveSettingsLastOpenDir(const QString &dir);
    void saveSettingsLastSaveDir(const QString &dir);
    void loadSettings();
    const QString loadSettingsLastOpenDir();
    const QString loadSettingsLastSaveDir();

    void loadProject(const QString &filename);
    void saveProject(const QString &filename);

    void saveImage(const QString &filename);
    void loadImage(const QString &filename);
    void readImage(Magick::Blob blob, const QString &filename = QString());
    void readImage(const QString &filename);
    void writeImage(const QString &filename);
    void writeLayer(const QString &filename, int id);

#ifdef WITH_FFMPEG
    void readAudio(const QString &filename);
    void readVideo(const QString &filename);
    void readVideo(const QString &filename, int frame);
    Magick::Image getVideoFrameAsImage(const QString &filename);
    Magick::Image getVideoFrameAsImage(const QString &filename,
                                       int frame);
#endif

    void saveProjectDialog();
    void saveImageDialog();
    void saveLayerDialog();
    void loadImageDialog();
    void newImageDialog();
    void newLayerDialog(bool isText = false);
    void newTextLayerDialog();


    //void handleNewImage(Magick::Image image);



    void handleZoom100ActionTriggered();
    void handleZoomFitActionTriggered(bool triggered);


    void connectView(View *view);
    void setViewTool(View *view);
    void handleViewClosed();

    void handleZoomChanged();
    void handleZoomFitChanged(bool fit);
    void setCurrentZoom();

    /*void newTabFromLayer(View *parentView,
                         int layerID);*/


    void handleCanvasStatus();

    void checkTabsOnClose();
    bool hasDirtyProjects();

    void setActionsDisabled(bool disabled);
    void setProjectSaveDisabled(bool disabled);


    // TODO
    void handleOpenImages(const QList<QUrl> &urls);
    void handleOpenLayers(const QList<QUrl> &urls);

protected:
    void resizeEvent(QResizeEvent *e) override;
    void closeEvent(QCloseEvent *e) override;
};

#endif // EDITOR_H
