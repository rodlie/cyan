/*
# Copyright Ole-André Rodlie.
#
# ole.andre.rodlie@gmail.com
#
# This software is governed by the CeCILL license under French law and
# abiding by the rules of distribution of free software. You can use,
# modify and / or redistribute the software under the terms of the CeCILL
# license as circulated by CEA, CNRS and INRIA at the following URL
# "https://www.cecill.info".
#
# As a counterpart to the access to the source code and rights to
# modify and redistribute granted by the license, users are provided only
# with a limited warranty and the software's author, the holder of the
# economic rights and the subsequent licensors have only limited
# liability.
#
# In this respect, the user's attention is drawn to the associated risks
# with loading, using, modifying and / or developing or reproducing the
# software by the user in light of its specific status of free software,
# that can mean that it is complicated to manipulate, and that also
# so that it is for developers and experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to test and test the software's suitability
# Requirements in the conditions of their systems
# data to be ensured and, more generally, to use and operate
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL license and that you accept its terms.
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
#include <QMdiSubWindow>
#include <QComboBox>
#include <QStatusBar>
#include <QSlider>
#include <QMap>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QToolButton>

#include "common.h"
#include "view.h"
#include "mdi.h"
#include "qtcolorpicker.h"
#include "qtcolortriangle.h"

#include "cyanlayerwidget.h"

class Editor : public QMainWindow
{
    Q_OBJECT

public:

    Editor(QWidget *parent = nullptr);
    ~Editor();

private:

    Common common;
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
    QAction *openLayerAct;
    QAction *saveLayerAct;
    QAction *blackPointAct;
    QAction *quitAct;

    QAction *viewMoveAct;
    QAction *viewDrawAct;

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
    QMenu *newMenu;
    QMenu *saveMenu;
    QMenu *colorMenu;
    QMenu *colorProfileRGBMenu;
    QMenu *colorProfileCMYKMenu;
    QMenu *colorProfileGRAYMenu;
    QMenu* colorIntentMenu;

    QToolButton *newButton;
    QToolButton *saveButton;
    CyanLayerWidget *layersWidget;
    QDockWidget *layersDock;



    QSlider *brushSize;
    QDockWidget *brushDock;

    QtColorTriangle *colorTriangle;
    QtColorPicker *colorPicker;




signals:

    void openImage(const QString &filename);
    void statusMessage(const QString &message);
    void errorMessage(const QString &message);
    void warningMessage(const QString &message);

public slots:

    View* getCurrentCanvas();

private slots:

    // setup
    void setupStyle();
    void setupUI();
    void setupMenus();
    void setupToolbars();
    void setupWidgets();
    void setupActions();
    void setupButtons();
    void setupColorManagement();
    void setupImageLayers();
    void setupConnections();
    void setupIcons();
    void setupShortcuts();
    void setupOptions();

    // about
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
    void addLayerToView(Magick::Image image,
                        View *view);
    void handleRemoveLayer(int id);
    void handleMoveLayerDown(int id);
    void handleMoveLayerUp(int id);

    // tabs
    void newTab(Common::Canvas canvas);
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
    void loadSettings();

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
    void newLayerDialog();


    //void handleNewImage(Magick::Image image);



    void connectView(View *view);
    void setViewTool(View *view);
    void handleViewClosed();


    /*void newTabFromLayer(View *parentView,
                         int layerID);*/


    // TODO
    void handleOpenImages(const QList<QUrl> &urls);
    void handleOpenLayers(const QList<QUrl> &urls);
};

#endif // EDITOR_H
