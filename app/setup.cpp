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
#include "editor.h"
#include <QStyleFactory>
#include <QApplication>
#include <QVBoxLayout>
#include <QDir>
#include <QToolBox>

#include "CyanLayerWidget.h"

void Editor::setupStyle()
{
    // style app
    qApp->setStyle(QStyleFactory::create("fusion"));
    if (QIcon::themeName().isEmpty()) {
        // we include fallback hicolor icons
        QIcon::setThemeName("hicolor");
    }

    // set colors
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53,53,53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(15,15,15));
    palette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    palette.setColor(QPalette::Link, Qt::white);
    palette.setColor(QPalette::LinkVisited, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53,53,53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Highlight, QColor(196,110,33));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    qApp->setPalette(palette);

    // stylesheet (override various stuff in Qt)
    setStyleSheet(QString("QMenu::separator { background-color: rgb(53, 53, 53); height: 1px; }"
                          "QToolBar { border-color: none; }"
                          "QToolButton::menu-indicator { image: none; }"));
}

void Editor::setupUI()
{
    setupStyle();
    setupWidgets();
    setupMenus();
    setupActions();
    setupConnections();
    setupIcons();
    setupShortcuts();
    setupOptions();

    setStatusBar(mainStatusBar);
    setMenuBar(mainMenu);

    mainMenu->addMenu(fileMenu);
    mainMenu->addMenu(editMenu);
    mainMenu->addMenu(colorMenu);
    mainMenu->addMenu(viewMenu);
    mainMenu->addMenu(optMenu);
    mainMenu->addMenu(windowsMenu);
    mainMenu->addMenu(helpMenu);

    windowsMenu->attachToMdiArea(mdi);

    mainToolBar->addWidget(newButton);
    mainToolBar->addWidget(openButton);
    mainToolBar->addWidget(saveButton);
    mainToolBar->addSeparator();
    mainToolBar->addWidget(interactButton);
    //mainToolBar->addWidget(zoomButton);
  //  mainToolBar->addSeparator();
    //mainToolBar->addWidget(convertButton);
   // mainToolBar->addSeparator();
    mainToolBar->addWidget(colorPicker);

    interactButton->addAction(viewMoveAct);
    interactButton->addAction(viewDrawAct);
    viewDrawAct->setDisabled(true);

    //newButton->addAction(newImageAct);
    //newButton->addAction(newLayerAct);

    //openButton->addAction(openImageAct);
    //openButton->addAction(openLayerAct);

    //zoomButton->addAction(viewZoom100Act);
    //zoomButton->addAction(viewZoomFitAct);

    fileMenu->addAction(newImageAct);
    //fileMenu->addAction(newLayerAct);
    //fileMenu->addAction(newTextLayerAct);
    //fileMenu->addSeparator();
    fileMenu->addAction(openImageAct);
    fileMenu->addAction(saveProjectAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveProjectAsAct);
    fileMenu->addSeparator();
    //fileMenu->addAction(openLayerAct);
    fileMenu->addAction(saveImageAct);
    //fileMenu->addAction(saveLayerAct);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAct);

    helpMenu->addAction(aboutCyanAct);
    helpMenu->addAction(aboutImageMagickAct);
    helpMenu->addAction(aboutLcmsAct);
#ifdef WITH_FFMPEG
    helpMenu->addAction(aboutFFmpegAct);
#endif
    helpMenu->addAction(aboutQtAct);

    saveMenu->addAction(saveProjectAct);
    saveMenu->addAction(saveProjectAsAct);
    saveMenu->addAction(saveImageAct);
    //saveMenu->addAction(saveLayerAct);

    saveButton->setMenu(saveMenu);

    colorMenu->addAction(convertRGBAct);
    colorMenu->addAction(convertCMYKAct);
    colorMenu->addAction(convertGRAYAct);
    colorMenu->addSeparator();
    colorMenu->addAction(convertAssignAct);
    colorMenu->addAction(convertExtractAct);
    colorMenu->addSeparator();
    colorMenu->addMenu(colorProfileRGBMenu);
    colorMenu->addMenu(colorProfileCMYKMenu);
    colorMenu->addMenu(colorProfileGRAYMenu);
    colorMenu->addSeparator();
    colorMenu->addMenu(colorIntentMenu);
    colorMenu->addAction(blackPointAct);

    //convertButton->setMenu(colorMenu);

    viewMenu->addMenu(guideMenu);
    viewMenu->addSeparator();
    viewMenu->addAction(viewZoom100Act);
    viewMenu->addAction(viewZoomFitAct);

    guideMenu->addAction(addGuideHAct);
    guideMenu->addAction(addGuideVAct);
    guideMenu->addAction(showGuidesAct);

    viewMoveAct->setChecked(true);
    interactButton->setDefaultAction(viewMoveAct);

    populateColorProfileMenu(colorProfileRGBMenu,
                             Magick::sRGBColorspace);
    populateColorProfileMenu(colorProfileCMYKMenu,
                             Magick::CMYKColorspace);
    populateColorProfileMenu(colorProfileGRAYMenu,
                             Magick::GRAYColorspace);
    populateColorIntentMenu();

    setActionsDisabled(true);



    // magick memory resources
    optMenu->addMenu(memoryMenu);
    QStringList memActs;
    memActs << "2" << "4" << "6" << "8" << "10" << "12" << "14" << "16" << "18" << "20" << "22" << "24";
    for (int i=0;i<memActs.size();++i) {
        QAction *act = new QAction(this);
        act->setCheckable(true);
        QString value = memActs.at(i);
        act->setText(QString("%1 GB RAM").arg(value));
        act->setToolTip(tr("Amount of RAM that can be used"));
        act->setData(value.toInt());
        connect(act, SIGNAL(triggered(bool)), this, SLOT(handleMagickMemoryAct(bool)));
        magickMemoryResourcesGroup->addAction(act);
    }
    memoryMenu->addActions(magickMemoryResourcesGroup->actions());

    // splitters
    mainSplitter->setOrientation(Qt::Horizontal);
    leftSplitter->setOrientation(Qt::Vertical);
    rightSplitter->setOrientation(Qt::Vertical);
    mainSplitter->addWidget(leftSplitter);
    mainSplitter->addWidget(mdi);
    mainSplitter->addWidget(rightSplitter);
    rightSplitter->addWidget(layersWidget);
    setCentralWidget(mainSplitter);
}

void Editor::setupMenus()
{
    mainMenu = new QMenuBar(this);
    mainMenu->setObjectName(QString("mainMenu"));

    fileMenu = new QMenu(this);
    fileMenu->setTitle(tr("File"));

    optMenu = new QMenu(this);
    optMenu->setTitle(tr("Options"));

    helpMenu = new QMenu(this);
    helpMenu->setTitle(tr("Help"));

    saveMenu = new QMenu(this);
    saveMenu->setTitle(tr("Save"));

    colorMenu = new QMenu(this);
    colorMenu->setTitle(tr("Colors"));

    viewMenu = new QMenu(this);
    viewMenu->setTitle(tr("View"));

    guideMenu = new QMenu(this);
    guideMenu->setTitle(tr("Guides"));

    memoryMenu = new QMenu(this);
    memoryMenu->setTitle(tr("Memory"));

    editMenu = new QMenu(this);
    editMenu->setTitle(tr("Edit"));

    windowsMenu = new QtWindowListMenu(this);

    colorProfileRGBMenu = new QMenu(this);
    colorProfileRGBMenu->setTitle(tr("Default RGB profile"));
    colorProfileRGBMenu->setObjectName(QString("colorProfileRGBMenu"));

    colorProfileCMYKMenu = new QMenu(this);
    colorProfileCMYKMenu->setTitle(tr("Default CMYK profile"));
    colorProfileCMYKMenu->setObjectName(QString("colorProfileCMYKMenu"));

    colorProfileGRAYMenu = new QMenu(this);
    colorProfileGRAYMenu->setTitle(tr("Default GRAY profile"));
    colorProfileGRAYMenu->setObjectName(QString("colorProfileGRAYMenu"));

    colorIntentMenu = new QMenu(this);
    colorIntentMenu->setTitle(tr("Rendering Intent"));
}

void Editor::setupWidgets()
{

    // the main splitters
    // TODO! add top and bottom
    mainSplitter = new QSplitter(this);
    mainSplitter->setObjectName(QString("mainSplitter"));
    leftSplitter = new QSplitter(this);
    leftSplitter->setObjectName(QString("leftSplitter"));
    rightSplitter = new QSplitter(this);
    rightSplitter->setObjectName(QString("rightSplitter"));

    // MDI
    mdi = new Mdi(this);
    mdi->setBackground(QBrush(QColor(20, 20, 20)));

    // status bar
    mainStatusBar = new QStatusBar(this);
    mainStatusBar->setObjectName(QString("mainStatusBar"));

    // layer widget
    layersWidget = new CyanLayerWidget(this);

    // add zoom % in status bar
    currentZoomStatusLabel = new QLabel(this);
    currentZoomStatusLabel->setText(QString("100%"));
    currentZoomStatusIcon = new QLabel(this);
    currentZoomStatusIcon->setPixmap(QIcon::fromTheme("zoom").pixmap(QSize(16, 16)));
    mainStatusBar->addPermanentWidget(currentZoomStatusLabel);
    mainStatusBar->addPermanentWidget(currentZoomStatusIcon);

    // main color picker
    colorPicker = new QtColorPicker(this, -1, true, false);
    colorPicker->setIconSize(QSize(24, 24));
    colorPicker->setStandardColors();
    colorPicker->setHidden(true);

    // toolbar
    mainToolBar = new QToolBar(this);
    mainToolBar->setObjectName(QString("mainToolBar"));
    mainToolBar->setWindowTitle(tr("Main"));
    mainToolBar->setMovable(false);
    addToolBar(Qt::LeftToolBarArea, mainToolBar);

    // toolbuttons
    newButton = new QToolButton(this);
    //newButton->setPopupMode(QToolButton::InstantPopup);
    newButton->setText(tr("New"));
    newButton->setToolTip(tr("New"));

    openButton = new QToolButton(this);
    //openButton->setPopupMode(QToolButton::InstantPopup);
    openButton->setText(tr("Open"));
    openButton->setToolTip(tr("Open"));

    saveButton = new QToolButton(this);
    saveButton->setMenu(saveMenu);
    saveButton->setPopupMode(QToolButton::InstantPopup);
    saveButton->setText(tr("Save"));
    saveButton->setToolTip(tr("Save"));

    interactButton = new QToolButton(this);
    interactButton->setPopupMode(QToolButton::InstantPopup);
    interactButton->setText(tr("Tool"));
    interactButton->setToolTip(tr("Tool"));

    /*zoomButton = new QToolButton(this);
    zoomButton->setPopupMode(QToolButton::InstantPopup);
    zoomButton->setText(tr("Zoom"));
    zoomButton->setToolTip(tr("Zoom"));*/

    /*convertButton = new QToolButton(this);
    convertButton->setMenu(colorMenu);
    convertButton->setPopupMode(QToolButton::InstantPopup);
    convertButton->setText(tr("Colors"));
    convertButton->setToolTip(tr("Color convert"));*/
}


void Editor::setupActions()
{
    magickMemoryResourcesGroup = new QActionGroup(this);

    newImageAct = new QAction(this);
    newImageAct->setText(tr("New"));

    openImageAct = new QAction(this);
    openImageAct->setText(tr("Open"));

    saveImageAct = new QAction(this);
    saveImageAct->setText(tr("Export image"));
    saveImageAct->setDisabled(true);

    saveProjectAct = new QAction(this);
    saveProjectAct->setText(tr("Save project"));
    saveProjectAct->setDisabled(true);

    saveProjectAsAct = new QAction(this);
    saveProjectAsAct->setText(tr("Save project as ..."));
    saveProjectAsAct->setDisabled(true);

    /*newLayerAct = new QAction(this);
    newLayerAct->setText(tr("New image layer"));
    newLayerAct->setDisabled(true);*/

    /*newTextLayerAct = new QAction(this);
    newTextLayerAct->setText(tr("New text layer"));
    newTextLayerAct->setDisabled(true);*/

    /*openLayerAct = new QAction(this);
    openLayerAct->setText(tr("Import image as layer"));
    openLayerAct->setDisabled(true);*/

    /*saveLayerAct = new QAction(this);
    saveLayerAct->setText(tr("Export layer"));
    saveLayerAct->setDisabled(true);*/

    quitAct = new QAction(this);
    quitAct->setText(tr("Quit"));

    viewMoveAct = new QAction(this);
    viewMoveAct->setText(tr("Move"));
    viewMoveAct->setCheckable(true);
    viewMoveAct->setChecked(false);

    viewDrawAct = new QAction(this);
    viewDrawAct->setText("Draw");
    viewDrawAct->setCheckable(true);
    viewDrawAct->setChecked(false);

    aboutCyanAct = new QAction(this);
    aboutCyanAct->setText(QString("%1 %2")
                          .arg(tr("About"))
                          .arg(qApp->applicationName()));

    aboutImageMagickAct = new QAction(this);
    aboutImageMagickAct->setText(QString("%1 ImageMagick").arg(tr("About")));

    aboutLcmsAct = new QAction(this);
    aboutLcmsAct->setText(QString("%1 Little CMS").arg(tr("About")));

    aboutQtAct = new QAction(this);
    aboutQtAct->setText(QString("%1 Qt").arg(tr("About")));

#ifdef WITH_FFMPEG
    aboutFFmpegAct = new QAction(this);
    aboutFFmpegAct->setText(QString("%1 FFmpeg").arg(tr("About")));
#endif

    convertRGBAct = new QAction(this);
    convertRGBAct->setText(tr("Convert to RGB"));

    convertCMYKAct = new QAction(this);
    convertCMYKAct->setText(tr("Convert to CMYK"));

    convertGRAYAct = new QAction(this);
    convertGRAYAct->setText(tr("Convert to GRAY"));

    convertAssignAct = new QAction(this);
    convertAssignAct->setText(tr("Assign color profile"));

    convertExtractAct = new QAction(this);
    convertExtractAct->setText(tr("Extract color profile"));

    blackPointAct = new QAction(this);
    blackPointAct->setText(tr("Black point compensation"));
    blackPointAct->setCheckable(true);

    viewZoom100Act = new QAction(this);
    viewZoom100Act->setText(tr("Zoom to actual size"));

    viewZoomFitAct = new QAction(this);
    viewZoomFitAct->setText(tr("Zoom to fit"));
    viewZoomFitAct->setCheckable(true);

    addGuideHAct = new QAction(this);
    addGuideHAct->setText(tr("Add horizontal line"));

    addGuideVAct = new QAction(this);
    addGuideVAct->setText(tr("Add vertical line"));

    showGuidesAct = new QAction(this);
    showGuidesAct->setText(tr("Show guides"));
    showGuidesAct->setCheckable(true);
}

void Editor::setupConnections()
{

// clean up!

    connect(viewZoom100Act, SIGNAL(triggered(bool)), this, SLOT(handleZoom100ActionTriggered()));
    connect(viewZoomFitAct, SIGNAL(triggered(bool)), this, SLOT(handleZoomFitActionTriggered(bool)));

    //connect(&common, SIGNAL(errorMessage(QString)), this, SLOT(handleError(QString)));
    //connect(&common, SIGNAL(warningMessage(QString)), this, SLOT(handleWarning(QString)));

    connect(newImageAct, SIGNAL(triggered(bool)), this, SLOT(newImageDialog()));


    //connect(newLayerAct, SIGNAL(triggered(bool)), this, SLOT(newLayerDialog()));
    //connect(newTextLayerAct, SIGNAL(triggered(bool)), this, SLOT(newTextLayerDialog()));



    connect(openImageAct, SIGNAL(triggered(bool)), this, SLOT(loadImageDialog()));
    connect(openButton, SIGNAL(clicked(bool)), this, SLOT(loadImageDialog()));
    connect(saveProjectAct, SIGNAL(triggered(bool)), this, SLOT(saveProjectDialog()));
    connect(saveImageAct, SIGNAL(triggered(bool)), this, SLOT(saveImageDialog()));
    //connect(saveLayerAct, SIGNAL(triggered(bool)), this, SLOT(saveLayerDialog()));

    connect(quitAct, SIGNAL(triggered(bool)), this, SLOT(checkTabsOnClose()));

    connect(viewMoveAct, SIGNAL(triggered(bool)), this, SLOT(handleSetMoveMode(bool)));
    connect(viewDrawAct, SIGNAL(triggered(bool)), this, SLOT(handleSetDrawMode(bool)));

    connect(aboutCyanAct, SIGNAL(triggered()), this, SLOT(aboutCyan()));
    connect(aboutImageMagickAct, SIGNAL(triggered()), this, SLOT(aboutImageMagick()));
    connect(aboutLcmsAct, SIGNAL(triggered()), this, SLOT(aboutLcms()));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
#ifdef WITH_FFMPEG
    connect(aboutFFmpegAct, SIGNAL(triggered()), this, SLOT(aboutFFmpeg()));
#endif

    connect(convertRGBAct, SIGNAL(triggered()), this, SLOT(handleColorConvertRGB()));
    connect(convertCMYKAct, SIGNAL(triggered()), this, SLOT(handleColorConvertCMYK()));
    connect(convertGRAYAct, SIGNAL(triggered()), this, SLOT(handleColorConvertGRAY()));
    connect(convertAssignAct, SIGNAL(triggered()), this, SLOT(handleColorProfileAssign()));

    connect(this, SIGNAL(statusMessage(QString)), this, SLOT(handleStatus(QString)));
    connect(this, SIGNAL(warningMessage(QString)), this, SLOT(handleWarning(QString)));
    connect(this, SIGNAL(errorMessage(QString)), this, SLOT(handleError(QString)));
    connect(mdi, SIGNAL(openImages(QList<QUrl>)), this, SLOT(handleOpenImages(QList<QUrl>)));
    //connect(brushSize, SIGNAL(valueChanged(int)), this, SLOT(handleBrushSize()));

    connect(mdi,
            SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this,
            SLOT(handleTabActivated(QMdiSubWindow*)));
    connect(mdi,
            SIGNAL(subWindowActivated(QMdiSubWindow*)),
            layersWidget,
            SLOT(handleTabActivated(QMdiSubWindow*)));
    connect(layersWidget,
            SIGNAL(selectedLayer(int)),
            this,
            SLOT(handleLayerTreeSelectedLayer(int)));
    connect(layersWidget,
            SIGNAL(layerVisibilityChanged(int,bool)),
            this,
            SLOT(handleLayerVisibility(int,bool)));
    connect(layersWidget,
            SIGNAL(layerLabelChanged(int,QString)),
            this,
            SLOT(handleLayerLabel(int,QString)));
    connect(layersWidget,
            SIGNAL(layerLockChanged(int,bool)),
            this,
            SLOT(handleLayerLock(int,bool)));
    connect(layersWidget,
            SIGNAL(newLayer()),
            this,
            SLOT(newLayerDialog()));
    connect(layersWidget,
            SIGNAL(removeLayer(int)),
            this,
            SLOT(handleRemoveLayer(int)));
    connect(layersWidget,
            SIGNAL(moveLayerUp(int)),
            this,
            SLOT(handleMoveLayerUp(int)));
    connect(layersWidget,
            SIGNAL(moveLayerDown(int)),
            this,
            SLOT(handleMoveLayerDown(int)));
    connect(layersWidget,
            SIGNAL(duplicateLayer(int)),
            this,
            SLOT(handleDuplicateLayer(int)));
    connect(layersWidget,
            SIGNAL(layerOpacityChanged(double,int)),
            this,
            SLOT(handleLayersOpacity(double,int)));
    connect(layersWidget,
            SIGNAL(layerCompositeChanged(Magick::CompositeOperator,int)),
            this,
            SLOT(handleLayerCompChanged(Magick::CompositeOperator,int)));

    /*connect(textWidget,
            SIGNAL(textChanged()),
            this,
            SLOT(handleCurrentLayerTextChanged()));*/

    //connect(layerButton, SIGNAL(toggled(bool)), layerPopup, SLOT(pinPopup(bool)));
    //connect(textButton, SIGNAL(toggled(bool)), textPopup, SLOT(pinPopup(bool)));

        connect(colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(handleColorChanged(QColor)));


        connect(newButton, SIGNAL(clicked(bool)), this, SLOT(newImageDialog()));

        connect(addGuideHAct, SIGNAL(triggered(bool)), this, SLOT(handleAddGuideHAct(bool)));
        connect(addGuideVAct, SIGNAL(triggered(bool)), this, SLOT(handleAddGuideVAct(bool)));
        connect(showGuidesAct, SIGNAL(triggered(bool)), this, SLOT(handleShowGuidesAct(bool)));
}

void Editor::setupIcons()
{
    setWindowIcon(QIcon::fromTheme(qApp->applicationName()));


    viewZoom100Act->setIcon(QIcon::fromTheme("zoom-original"));
    viewZoomFitAct->setIcon(QIcon::fromTheme("zoom-fit-best"));

    newImageAct->setIcon(QIcon::fromTheme("document-new"));
    //newLayerAct->setIcon(QIcon::fromTheme("layer",
      //                                    QIcon::fromTheme("document-new")));
    //newTextLayerAct->setIcon(QIcon::fromTheme("font",
      //                                        QIcon::fromTheme("document-new")));

    openImageAct->setIcon(QIcon::fromTheme("document-open"));
    saveButton->setIcon(QIcon::fromTheme("document-save"));
    saveImageAct->setIcon(QIcon::fromTheme("document-new"));
    saveProjectAct->setIcon(QIcon::fromTheme("document-save"));
    //saveLayerAct->setIcon(QIcon::fromTheme("layer"));
    saveProjectAsAct->setIcon(QIcon::fromTheme("document-save-as"));
    quitAct->setIcon(QIcon::fromTheme("application-exit"));

    viewMoveAct->setIcon(QIcon::fromTheme("transform_move"));
    viewDrawAct->setIcon(QIcon::fromTheme("paintbrush"));

    QIcon colorsIcon = QIcon::fromTheme("colors");
    QIcon colorWheelIcon = QIcon::fromTheme("color-wheel");

    convertRGBAct->setIcon(QIcon::fromTheme("convert_gray_to_color"));
    convertCMYKAct->setIcon(QIcon::fromTheme("convert_gray_to_color"));
    convertGRAYAct->setIcon(QIcon::fromTheme("convert_color_to_gray"));
    convertAssignAct->setIcon(colorsIcon);
    convertExtractAct->setIcon(colorsIcon);
    colorProfileRGBMenu->setIcon(colorWheelIcon);
    colorProfileCMYKMenu->setIcon(colorWheelIcon);
    colorProfileGRAYMenu->setIcon(colorWheelIcon);
    colorIntentMenu->setIcon(QIcon::fromTheme("video-display"));
    blackPointAct->setIcon(colorWheelIcon);

    QIcon helpIcon = QIcon::fromTheme("help-about");

    aboutQtAct->setIcon(helpIcon);
    aboutLcmsAct->setIcon(helpIcon);
    aboutImageMagickAct->setIcon(helpIcon);
    aboutCyanAct->setIcon(helpIcon);
#ifdef WITH_FFMPEG
    aboutFFmpegAct->setIcon(helpIcon);
#endif

    //convertButton->setIcon(colorsIcon);
    newButton->setIcon(QIcon::fromTheme("document-new"));
    openButton->setIcon(QIcon::fromTheme("document-open"));
    interactButton->setIcon(QIcon::fromTheme("transform_move"));
    //zoomButton->setIcon(QIcon::fromTheme("zoom"));

}

void Editor::setupShortcuts()
{
    newImageAct->setShortcut(QKeySequence(tr("Ctrl+N")));
    //newLayerAct->setShortcut(QKeySequence(tr("Ctrl+L")));
    //newTextLayerAct->setShortcut(QKeySequence(tr("Ctrl+Shift+L")));
    openImageAct->setShortcut(QKeySequence(tr("Ctrl+O")));
    quitAct->setShortcut(QKeySequence(tr("Ctrl+Q")));

    viewZoom100Act->setShortcut(QKeySequence(tr("Ctrl+1")));
    viewZoomFitAct->setShortcut(QKeySequence(tr("Ctrl+2")));

    addGuideHAct->setShortcut(QKeySequence(tr("Ctrl+Shift+G")));
    addGuideVAct->setShortcut(QKeySequence(tr("Ctrl+G")));
}

void Editor::setupOptions()
{

}


