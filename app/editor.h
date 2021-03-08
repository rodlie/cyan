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

#include "CyanCommon.h"
//#include "qtcolorpicker.h"
#include "qtwindowlistmenu.h"

#ifdef BATCH
#include "CyanBatchWidget.h"
#endif

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
    QAction *saveImageAsAct;
    //QAction *saveProjectAct;
    //QAction *saveProjectAsAct;
    QAction *blackPointAct;
    QAction *quitAct;

    QAction *newImageLayerAct;
    QAction *removeLayerAct;
    QAction *moveUpLayerAct;
    QAction *moveDownLayerAct;
    QAction *duplicateLayerAct;

    QAction *viewMoveAct;
    QAction *viewDrawAct;
    QAction *viewZoom100Act;
    QAction *viewZoomFitAct;

    QAction *aboutCyanAct;

    QAction *undoAct;
    QAction *redoAct;

    QAction *convertRGBAct;
    QAction *convertCMYKAct;
    QAction *convertGRAYAct;
    QAction *convertAssignAct;
    QAction *convertExtractAct;

    QAction *addGuideVAct;
    QAction *addGuideHAct;
    QAction *showGuidesAct;

    QActionGroup *viewModeGroup;

    QActionGroup *profileRGBGroup;
    QActionGroup *profileCMYKGroup;
    QActionGroup *profileGRAYGroup;

    QMenu *fileMenu;
    QMenu *optMenu;
    QMenu *helpMenu;
    //QMenu *saveMenu;
    QMenu *colorMenu;
    QMenu *colorProfileRGBMenu;
    QMenu *colorProfileCMYKMenu;
    QMenu *colorProfileGRAYMenu;
    QMenu *colorIntentMenu;
    QMenu *layersMenu;
    QMenu *viewMenu;
    QMenu *guideMenu;
    QMenu *memoryMenu;
    QMenu *editMenu;
    QtWindowListMenu *windowsMenu;

    QToolButton *newButton;
    QToolButton *openButton;
    QToolButton *saveButton;
    QToolButton *interactButton;
    QToolButton *layersButton;
    QToolButton *colorsButton;

    CyanLayerWidget *layersWidget;
    //QtColorPicker *colorPicker;

    QLabel *currentZoomStatusIcon;
    QLabel *currentZoomStatusLabel;

    QSplitter *mainSplitter;
    QSplitter *rightSplitter;
    QSplitter *leftSplitter;
    QSplitter *bottomSplitter;
    QSplitter *middleSplitter;
    QSplitter *topSplitter;

    bool hasTextSupport; // remove

    bool _native;

signals:
    void openImage(const QString &filename);
    void statusMessage(const QString &message);
    void errorMessage(const QString &message);
    void warningMessage(const QString &message);
    void currentTextChanged(const QString &html,
                            int align,
                            int rotate,
                            bool enabled);

public slots:
    View* getCurrentCanvas();
    void openConsoleImage(const QString &filename);

private slots:
    // plugins
    void initPlugins(QList<QPluginLoader*> plugins);
    void initWidgetPlugin(CyanWidgetPlugin* plugin);

    // setup
    void setupStyle(bool native = false);
    void setupUI();
    void setupMenus();
    void setupWidgets(bool native = false);
    void setupActions();
    void setupConnections();
    void setupIcons();
    void setupShortcuts();
    void setupOptions();
    void handleStyleChange(bool triggered);

    // about
    void aboutCyan();

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
    void handleCurrentLayerTextChanged(const QString &html,
                                       int align,
                                       int rotate,
                                       bool enabled);

    // tabs
    void newTab(CyanImageFormat::CyanCanvas canvas);
    void newTab(Magick::Image image = Magick::Image(),
                QSize geo = QSize(0, 0));
    void handleTabActivated(QMdiSubWindow *tab);
    void updateTabTitle(View *view = nullptr);

    // color
    void populateColorProfileMenu(QMenu *menu,
                                  QActionGroup *group,
                                  Magick::ColorspaceType colorspace);
    void handleColorProfileTriggered();
    void setDefaultColorProfiles();
    void setDefaultColorProfileFromFilename(QActionGroup *group,
                                            const QString &filename);
    void setDefaultColorProfileFromTitle(QActionGroup *group,
                                         const QString &title);
    const QString selectedDefaultColorProfile(QActionGroup *group);
    Magick::Blob selectedDefaultColorProfileData(QActionGroup *group);

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
    void saveColorProfileSettings();

    //
    void saveSettings();
    void saveSettingsLastOpenDir(const QString &dir);
    void saveSettingsLastSaveDir(const QString &dir);
    void loadSettings();
    const QString loadSettingsLastOpenDir();
    const QString loadSettingsLastSaveDir();

    void loadProject(const QString &filename);
    void loadUnknownProject(const QString &filename);
    void writeProject(const QString &filename, bool setFilename = true);
    void writeProjectPSD(const QString &filename, bool setFilename = true);

    //void saveImage(const QString &filename);
    void loadImage(const QString &filename);
    void readImage(Magick::Blob blob, const QString &filename = QString());
    void readImage(const QString &filename);
    void writeImage(const QString &filename, bool setFilename = true);
    void writeLayer(const QString &filename, int id);

    //void saveProjectDialog();
    void saveImageDialog(bool ignoreExisting = false, bool setFilename = true);
    void saveImageAsDialog();
    void saveLayerDialog();
    void loadImageDialog();
    void newImageDialog();
    void newLayerDialog(bool isText = false);
    void newTextLayerDialog();

    void handleZoom100ActionTriggered();
    void handleZoomFitActionTriggered(bool triggered);

    void connectView(View *view);
    void setViewTool(View *view);
    void handleViewClosed();

    void handleZoomChanged();
    void handleZoomFitChanged(bool fit);
    void setCurrentZoom();

    void handleCanvasStatus();

    void checkTabsOnClose();
    bool hasDirtyProjects();

    void hasImageMagick();

    void setActionsDisabled(bool disabled);
    void setProjectSaveDisabled(bool disabled);

    // history
    void setViewUndo();
    void setViewRedo();

    // guides
    void handleAddGuideHAct(bool triggered);
    void handleAddGuideVAct(bool triggered);
    void handleShowGuidesAct(bool triggered);

    //
    void handleViewModeAct(bool triggered);

    // TODO
    void handleOpenImages(const QList<QUrl> &urls);
    void handleOpenLayers(const QList<QUrl> &urls);

protected:
    void resizeEvent(QResizeEvent *e) override;
    void closeEvent(QCloseEvent *e) override;
};

#endif // EDITOR_H
