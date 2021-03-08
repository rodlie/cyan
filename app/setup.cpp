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
#include <QSettings>
#include <QMessageBox>

#include "CyanLayerWidget.h"

void Editor::setupStyle(bool native)
{
    if (native) {
        if (QIcon::themeName().isEmpty()) { // if no theme add hicolor
            QIcon::setThemeName("hicolor");
        }
        return; // ignore custom
    }

    // set qt style (force fusion)
    qApp->setStyle(QStyleFactory::create("fusion"));

    // set icon theme (force hicolor)
    QIcon::setThemeName("hicolor");

    // set (dark) colors
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
    QSettings settings;
    settings.beginGroup("gui");
    _native = settings.value("native", false).toBool();
    settings.endGroup();

    setupStyle(_native);
    setupWidgets(_native);
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
    mainMenu->addMenu(layersMenu);
    mainMenu->addMenu(windowsMenu);
    mainMenu->addMenu(optMenu);
    mainMenu->addMenu(helpMenu);

    windowsMenu->attachToMdiArea(mdi);

    mainToolBar->addWidget(newButton);
    mainToolBar->addWidget(openButton);
    mainToolBar->addWidget(saveButton);
    mainToolBar->addSeparator();
    mainToolBar->addWidget(interactButton);
    mainToolBar->addWidget(layersButton);
    mainToolBar->addWidget(colorsButton);
    //mainToolBar->addSeparator();
    //mainToolBar->addWidget(colorPicker);

    interactButton->addAction(viewMoveAct);
    interactButton->addAction(viewDrawAct);

    fileMenu->addAction(newImageAct);
    fileMenu->addAction(openImageAct);
    //fileMenu->addAction(saveProjectAct);
    //fileMenu->addSeparator();
    //fileMenu->addAction(saveProjectAsAct);
    //fileMenu->addSeparator();
    fileMenu->addAction(saveImageAct);
    fileMenu->addAction(saveImageAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAct);

    helpMenu->addAction(aboutCyanAct);

    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    /*helpMenu->addAction(aboutImageMagickAct);
    helpMenu->addAction(aboutLcmsAct);
    helpMenu->addAction(aboutQtAct);
*/
    //saveMenu->addAction(saveProjectAct);
    //saveMenu->addAction(saveProjectAsAct);
//    saveMenu->addAction(saveImageAct);

    //saveButton->setMenu(saveMenu);

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

    viewMenu->addMenu(guideMenu);
    viewMenu->addSeparator();
    viewMenu->addAction(viewZoom100Act);
    viewMenu->addAction(viewZoomFitAct);

    layersMenu->addAction(newImageLayerAct);
    layersMenu->addAction(removeLayerAct);
    layersMenu->addAction(duplicateLayerAct);
    layersMenu->addAction(moveUpLayerAct);
    layersMenu->addAction(moveDownLayerAct);

    layersButton->addActions(layersMenu->actions());
    colorsButton->addActions(colorMenu->actions());

#ifndef Q_OS_MAC
    QAction *viewModeWindows = new QAction(this);
    QAction *viewModeTabs = new QAction(this);

    connect(viewModeWindows, SIGNAL(triggered(bool)), this, SLOT(handleViewModeAct(bool)));
    connect(viewModeTabs, SIGNAL(triggered(bool)), this, SLOT(handleViewModeAct(bool)));

    viewModeWindows->setText(tr("Windows"));
    viewModeWindows->setData(QMdiArea::SubWindowView);
    viewModeWindows->setCheckable(true);
    viewModeTabs->setText(tr("Tabs"));
    viewModeTabs->setData(QMdiArea::TabbedView);
    viewModeTabs->setCheckable(true);

    viewModeGroup->addAction(viewModeWindows);
    viewModeGroup->addAction(viewModeTabs);

    viewMenu->addSeparator();
    viewMenu->addActions(viewModeGroup->actions());
#endif

    guideMenu->addAction(addGuideHAct);
    guideMenu->addAction(addGuideVAct);
    guideMenu->addAction(showGuidesAct);

    viewMoveAct->setChecked(true);
    interactButton->setDefaultAction(viewMoveAct);

    populateColorProfileMenu(colorProfileRGBMenu,
                             profileRGBGroup,
                             Magick::sRGBColorspace);
    populateColorProfileMenu(colorProfileCMYKMenu,
                             profileCMYKGroup,
                             Magick::CMYKColorspace);
    populateColorProfileMenu(colorProfileGRAYMenu,
                             profileGRAYGroup,
                             Magick::GRAYColorspace);

    populateColorIntentMenu();

    setActionsDisabled(true);


    // native/app style
    QActionGroup *nativeGroup = new QActionGroup(this);

    QAction *nativeStyle = new QAction(this);
    nativeStyle->setCheckable(true);
    nativeStyle->setText(tr("System Theme"));
    nativeStyle->setData(true);
    nativeGroup->addAction(nativeStyle);
    connect(nativeStyle, SIGNAL(triggered(bool)), this, SLOT(handleStyleChange(bool)));

    QAction *appStyle = new QAction(this);
    appStyle->setCheckable(true);
    appStyle->setText(tr("Application Theme"));
    appStyle->setData(false);
    nativeGroup->addAction(appStyle);
    connect(appStyle, SIGNAL(triggered(bool)), this, SLOT(handleStyleChange(bool)));

    viewMenu->addSeparator();
    viewMenu->addActions(nativeGroup->actions());

    if (_native) { nativeStyle->setChecked(true); }
    else { appStyle->setChecked(true); }

    // splitters
    mainSplitter->setOrientation(Qt::Horizontal);
    leftSplitter->setOrientation(Qt::Vertical);
    rightSplitter->setOrientation(Qt::Vertical);
    bottomSplitter->setOrientation(Qt::Horizontal);
    middleSplitter->setOrientation(Qt::Vertical);
    topSplitter->setOrientation(Qt::Horizontal);

    mainSplitter->addWidget(leftSplitter);
    mainSplitter->addWidget(middleSplitter);
    mainSplitter->addWidget(rightSplitter);

    rightSplitter->addWidget(layersWidget);

    middleSplitter->addWidget(topSplitter);
    middleSplitter->addWidget(mdi);
    middleSplitter->addWidget(bottomSplitter);

#ifdef BATCH
    CyanBatchWidget *batch = new CyanBatchWidget(this);
    leftSplitter->addWidget(batch);
#endif

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

//    saveMenu = new QMenu(this);
//    saveMenu->setTitle(tr("Save"));

    colorMenu = new QMenu(this);
    colorMenu->setTitle(tr("Colors"));

    layersMenu = new QMenu(this);
    layersMenu->setTitle(tr("Layers"));

    viewMenu = new QMenu(this);
    viewMenu->setTitle(tr("View"));

    guideMenu = new QMenu(this);
    guideMenu->setTitle(tr("Guides"));

    memoryMenu = new QMenu(this);
    memoryMenu->setTitle(tr("Engine memory limit"));

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

void Editor::setupWidgets(bool native)
{
    // the main splitters
    // TODO! add top and bottom
    mainSplitter = new QSplitter(this);
    mainSplitter->setObjectName(QString("mainSplitter"));
    leftSplitter = new QSplitter(this);
    leftSplitter->setObjectName(QString("leftSplitter"));
    rightSplitter = new QSplitter(this);
    rightSplitter->setObjectName(QString("rightSplitter"));
    bottomSplitter = new QSplitter(this);
    bottomSplitter->setObjectName(QString("bottomSplitter"));
    middleSplitter = new QSplitter(this);
    middleSplitter->setObjectName(QString("middleSplitter"));
    topSplitter = new QSplitter(this);
    topSplitter->setObjectName(QString("topSplitter"));

    // MDI
    mdi = new Mdi(this);
    if (!native) {
        mdi->setBackground(QBrush(QColor(20, 20, 20)));
    }

    // status bar
    mainStatusBar = new QStatusBar(this);
    mainStatusBar->setObjectName(QString("mainStatusBar"));

    // layer widget
    layersWidget = new CyanLayerWidget(this, native);

    // add zoom % in status bar
    currentZoomStatusLabel = new QLabel(this);
    currentZoomStatusLabel->setText(QString("100%"));
    currentZoomStatusIcon = new QLabel(this);
    currentZoomStatusIcon->setPixmap(QIcon::fromTheme("zoom").pixmap(QSize(16, 16)));
    mainStatusBar->addPermanentWidget(currentZoomStatusLabel);
    mainStatusBar->addPermanentWidget(currentZoomStatusIcon);

    // main color picker
    /*colorPicker = new QtColorPicker(this, -1, true, false);
    colorPicker->setIconSize(QSize(16, 32));
    colorPicker->setStandardColors();*/
    //colorPicker->hide();

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
    /*saveButton->setMenu(saveMenu);
    saveButton->setPopupMode(QToolButton::InstantPopup);
    saveButton->setText(tr("Save"));
    saveButton->setToolTip(tr("Save"));*/

    interactButton = new QToolButton(this);
    interactButton->setPopupMode(QToolButton::InstantPopup);
    interactButton->setText(tr("Tool"));
    interactButton->setToolTip(tr("Tool"));

    layersButton = new QToolButton(this);
    layersButton->setPopupMode(QToolButton::InstantPopup);
    layersButton->setText(tr("Layers"));
    layersButton->setToolTip(tr("Layers"));

    colorsButton = new QToolButton(this);
    colorsButton->setPopupMode(QToolButton::InstantPopup);
    colorsButton->setText(tr("Colors"));
    colorsButton->setToolTip(tr("Colors"));
}


void Editor::setupActions()
{
    viewModeGroup = new QActionGroup(this);

    profileRGBGroup = new QActionGroup(this);
    profileCMYKGroup = new QActionGroup(this);
    profileGRAYGroup = new QActionGroup(this);

    newImageAct = new QAction(this);
    newImageAct->setText(tr("New"));

    openImageAct = new QAction(this);
    openImageAct->setText(tr("Open"));

    saveImageAct = new QAction(this);
    saveImageAct->setText(tr("Save"));
    //saveImageAct->setDisabled(true);

//    saveProjectAct = new QAction(this);
//    saveProjectAct->setText(tr("Save project"));
//    saveProjectAct->setDisabled(true);

    saveImageAsAct = new QAction(this);
    saveImageAsAct->setText(tr("Save as ..."));

    /*saveProjectAsAct = new QAction(this);
    saveProjectAsAct->setText(tr("Save project as ..."));
    saveProjectAsAct->setDisabled(true);*/

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
    viewDrawAct->setDisabled(true);

    aboutCyanAct = new QAction(this);
    aboutCyanAct->setText(QString("%1 %2")
                          .arg(tr("About"))
                          .arg(qApp->applicationName()));

    undoAct = new QAction(this);
    undoAct->setText(tr("Undo"));
    //undoAct->setDisabled(true);

    redoAct = new QAction(this);
    redoAct->setText(tr("Redo"));
    //redoAct->setDisabled(true);

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
    convertExtractAct->setVisible(false);

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

    newImageLayerAct = new QAction(tr("New Layer"), this);
    removeLayerAct = new QAction(tr("Remove layer"), this);
    moveUpLayerAct = new QAction(tr("Move layer up"), this);
    moveDownLayerAct = new QAction(tr("Move layer down"), this);
    duplicateLayerAct = new QAction(tr("Duplicate layer"), this);
}

void Editor::setupConnections()
{
    connect(viewZoom100Act, SIGNAL(triggered(bool)), this, SLOT(handleZoom100ActionTriggered()));
    connect(viewZoomFitAct, SIGNAL(triggered(bool)), this, SLOT(handleZoomFitActionTriggered(bool)));

    connect(newImageAct, SIGNAL(triggered(bool)), this, SLOT(newImageDialog()));
    connect(openImageAct, SIGNAL(triggered(bool)), this, SLOT(loadImageDialog()));
    connect(openButton, SIGNAL(clicked(bool)), this, SLOT(loadImageDialog()));
    connect(saveButton, SIGNAL(clicked(bool)), this, SLOT(saveImageDialog()));
    //connect(saveProjectAct, SIGNAL(triggered(bool)), this, SLOT(saveProjectDialog()));
    connect(saveImageAct, SIGNAL(triggered(bool)), this, SLOT(saveImageDialog()));
    connect(saveImageAsAct, SIGNAL(triggered(bool)), this, SLOT(saveImageAsDialog()));

    connect(quitAct, SIGNAL(triggered(bool)), this, SLOT(checkTabsOnClose()));

    connect(viewMoveAct, SIGNAL(triggered(bool)), this, SLOT(handleSetMoveMode(bool)));
    connect(viewDrawAct, SIGNAL(triggered(bool)), this, SLOT(handleSetDrawMode(bool)));

    connect(aboutCyanAct, SIGNAL(triggered()), this, SLOT(aboutCyan()));

    connect(undoAct, SIGNAL(triggered()), this, SLOT(setViewUndo()));
    connect(redoAct, SIGNAL(triggered()), this, SLOT(setViewRedo()));

    connect(convertRGBAct, SIGNAL(triggered()), this, SLOT(handleColorConvertRGB()));
    connect(convertCMYKAct, SIGNAL(triggered()), this, SLOT(handleColorConvertCMYK()));
    connect(convertGRAYAct, SIGNAL(triggered()), this, SLOT(handleColorConvertGRAY()));
    connect(convertAssignAct, SIGNAL(triggered()), this, SLOT(handleColorProfileAssign()));

    connect(this, SIGNAL(statusMessage(QString)), this, SLOT(handleStatus(QString)));
    connect(this, SIGNAL(warningMessage(QString)), this, SLOT(handleWarning(QString)));
    connect(this, SIGNAL(errorMessage(QString)), this, SLOT(handleError(QString)));
    connect(mdi, SIGNAL(openImages(QList<QUrl>)), this, SLOT(handleOpenImages(QList<QUrl>)));

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
            SIGNAL(newLayer(bool)),
            this,
            SLOT(newLayerDialog(bool)));
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

    //connect(colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(handleColorChanged(QColor)));
    connect(newButton, SIGNAL(clicked(bool)), this, SLOT(newImageDialog()));

    connect(addGuideHAct, SIGNAL(triggered(bool)), this, SLOT(handleAddGuideHAct(bool)));
    connect(addGuideVAct, SIGNAL(triggered(bool)), this, SLOT(handleAddGuideVAct(bool)));
    connect(showGuidesAct, SIGNAL(triggered(bool)), this, SLOT(handleShowGuidesAct(bool)));


    connect(newImageLayerAct,
            SIGNAL(triggered(bool)),
            layersWidget,
            SLOT(createNewLayer(bool)));
    /*connect(newTextLayerAct,
            SIGNAL(triggered(bool)),
            this,
            SLOT(handleNewTextAct(bool)));*/
    connect(removeLayerAct,
            SIGNAL(triggered(bool)),
            layersWidget,
            SLOT(removeCurrentLayer(bool)));
    connect(moveUpLayerAct,
            SIGNAL(triggered(bool)),
            layersWidget,
            SLOT(moveCurrentLayerUp(bool)));
    connect(moveDownLayerAct,
            SIGNAL(triggered(bool)),
            layersWidget,
            SLOT(moveCurrentLayerDown(bool)));
    connect(duplicateLayerAct,
            SIGNAL(triggered(bool)),
            layersWidget,
            SLOT(duplicateCurrentLayer(bool)));
}

void Editor::setupIcons()
{
    setWindowIcon(QIcon::fromTheme(qApp->applicationName()));


    viewZoom100Act->setIcon(QIcon::fromTheme("zoom-original"));
    viewZoomFitAct->setIcon(QIcon::fromTheme("zoom-fit-best"));

    newImageAct->setIcon(QIcon::fromTheme("document-new"));
    openImageAct->setIcon(QIcon::fromTheme("document-open"));
    saveButton->setIcon(QIcon::fromTheme("document-save"));
    saveImageAct->setIcon(QIcon::fromTheme("document-save-as"));
    saveImageAsAct->setIcon(QIcon::fromTheme("document-save-as"));
//    saveProjectAct->setIcon(QIcon::fromTheme("document-save"));
    //saveProjectAsAct->setIcon(QIcon::fromTheme("document-save-as"));
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
    //blackPointAct->setIcon(colorWheelIcon);

    QIcon helpIcon = QIcon::fromTheme("help-about");

    aboutCyanAct->setIcon(helpIcon);
    newButton->setIcon(QIcon::fromTheme("document-new"));
    openButton->setIcon(QIcon::fromTheme("document-open"));
    interactButton->setIcon(QIcon::fromTheme("transform_move"));


    newImageLayerAct->setIcon(QIcon::fromTheme("layer",
                                               QIcon::fromTheme("document-new")));
    removeLayerAct->setIcon(QIcon::fromTheme("edit-delete"));
    moveUpLayerAct->setIcon(QIcon::fromTheme("go-up"));
    moveDownLayerAct->setIcon(QIcon::fromTheme("go-down"));
    duplicateLayerAct->setIcon(QIcon::fromTheme("layers"));
    layersButton->setIcon(QIcon::fromTheme("layers"));
    colorsButton->setIcon(colorsIcon);
}

void Editor::setupShortcuts()
{
    newImageAct->setShortcut(QKeySequence(tr("Ctrl+N")));
    openImageAct->setShortcut(QKeySequence(tr("Ctrl+O")));
    saveImageAct->setShortcut(QKeySequence(tr("Ctrl+S")));
    saveImageAsAct->setShortcut(QKeySequence(tr("Ctrl+Shift+S")));
    quitAct->setShortcut(QKeySequence(tr("Ctrl+Q")));

    viewZoom100Act->setShortcut(QKeySequence(tr("Ctrl+1")));
    viewZoomFitAct->setShortcut(QKeySequence(tr("Ctrl+2")));

    addGuideHAct->setShortcut(QKeySequence(tr("Ctrl+Shift+G")));
    addGuideVAct->setShortcut(QKeySequence(tr("Ctrl+G")));

    newImageLayerAct->setShortcut(QKeySequence(tr("Ctrl+Shift+N")));
    removeLayerAct->setShortcut(QKeySequence(tr("Ctrl+D")));
    duplicateLayerAct->setShortcut(QKeySequence(tr("Ctrl+Shift+D")));
    moveUpLayerAct->setShortcut(Qt::Key_PageUp);
    moveDownLayerAct->setShortcut(Qt::Key_PageDown);

    undoAct->setShortcut(QKeySequence(tr("Ctrl+Z")));
    redoAct->setShortcut(QKeySequence(tr("Ctrl+Shift+Z")));
}

void Editor::setupOptions()
{
}

void Editor::handleStyleChange(bool triggered)
{
    Q_UNUSED(triggered)
    qDebug() << "handle style change?";
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action) { return; }
    if (action->data().toBool() != _native) {
        QSettings settings;
        settings.beginGroup("gui");
        settings.setValue("native", action->data().toBool());
        settings.endGroup();
        settings.sync();
        _native = action->data().toBool();
        QMessageBox::information(this,
                                 tr("Restart to apply"),
                                 tr("You will need to restart Cyan to apply this setting."));
    }
}

void Editor::handleError(const QString &message)
{
    qDebug() << "error" << message;
    mainStatusBar->showMessage(message, 6000);
    QMessageBox::warning(this,
                         tr("Error"),
                         message);
}

void Editor::handleWarning(const QString &message)
{
    qDebug() << "warning" << message;
    mainStatusBar->showMessage(message);
    /*QMessageBox::warning(this,
                         tr("Cyan Warning"),
                         message);*/
}

void Editor::handleStatus(const QString &message)
{
    qDebug() << "status" << message;
    mainStatusBar->showMessage(message,
                               6000);
}
