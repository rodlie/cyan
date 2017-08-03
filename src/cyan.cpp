/*
* Cyan <http://cyan.fxarena.net> <https://github.com/olear/cyan>,
* Copyright (C) 2016, 2017 Ole-André Rodlie
*
* Cyan is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as published
* by the Free Software Foundation.
*
* Cyan is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Cyan.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>
*/

#include "cyan.h"
#include <QCoreApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSettings>
#include <QTimer>
#include <QDebug>
#include <QFileInfo>
#include <QFileDialog>
#include <QMatrix>
#include <QMessageBox>
#include <QIcon>
#include <QKeySequence>
#include <QUrl>
#include <QMimeData>
#include <QVBoxLayout>
#include <QDirIterator>
#include <QApplication>
#include <QStyleFactory>
#include <QDomDocument>
#include <QDomNode>

#ifdef __APPLE__
#define CYAN_FONT_SIZE 10
#else
#define CYAN_FONT_SIZE 8
#endif

#define COLOR_FILTER_ITEM_DATA 32
#define BLACKPOINT_CHECKBOX_LOCATION 18

CyanView::CyanView(QWidget* parent) : QGraphicsView(parent)
, fit(false) {
    setAcceptDrops(true);
    setBackgroundBrush(Qt::darkGray);
    setDragMode(QGraphicsView::ScrollHandDrag);
}

void CyanView::wheelEvent(QWheelEvent* event) {
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    double scaleFactor = 1.15;
    if (event->delta() > 0) { // up
        fit = false;
        scale(scaleFactor, scaleFactor);
        emit myZoom(scaleFactor, scaleFactor);
    } else { // down
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        emit myZoom(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}

void CyanView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        fit = false;
        emit resetZoom();
    } else {
        if (event->button() == Qt::RightButton) {
            setFit(true);
        } else {
            QGraphicsView::mousePressEvent(event);
        }
    }
}

void CyanView::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void CyanView::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void CyanView::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void CyanView::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->urls().at(0).isEmpty()) {
        QUrl url = event->mimeData()->urls().at(0);
        QString suffix = QFileInfo(url.toLocalFile()).suffix().toUpper();
        if (suffix == "PNG"
            || suffix == "JPG"
            || suffix == "JPEG"
            || suffix == "TIF"
            || suffix == "TIFF"
            || suffix == "PSD")
        {
            emit openImage(url.toLocalFile());
        } else if (suffix == "ICC" || suffix == "ICM") {
            emit openProfile(url.toLocalFile());
        }
    }
}

void CyanView::resizeEvent(QResizeEvent */*event*/)
{
    if (fit) {
        fitInView(0, 0, scene()->width(), scene()->height(), Qt::KeepAspectRatio);
    }
}

void CyanView::doZoom(double scaleX, double scaleY)
{
    scale(scaleX,scaleY);
}

void CyanView::setFit(bool value)
{
    fit = value;
    fitInView(0, 0, scene()->width(), scene()->height(), Qt::KeepAspectRatio);
}

CyanProfile::CyanProfile(QWidget *parent)
    : QDialog(parent)
    , profileFileName(0)
    , profileDescription(0)
    , profileCopyright(0)
    , profileSaveButton(0)
    , profileCloseButton(0)
{
    setWindowTitle(tr("Edit Color Profile"));
    setWindowIcon(QIcon(":/cyan.png"));

    QFrame *containerFrame = new QFrame();
    QFrame *buttonFrame = new QFrame();
    QFrame *descriptionFrame = new QFrame();
    QFrame *copyrightFrame = new QFrame();

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    QVBoxLayout *containerLayout = new QVBoxLayout(containerFrame);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonFrame);
    QHBoxLayout *descriptionLayout = new QHBoxLayout(descriptionFrame);
    QHBoxLayout *copyrightLayout = new QHBoxLayout(copyrightFrame);

    descriptionLayout->setContentsMargins(0,0,0,0);
    copyrightLayout->setContentsMargins(0,0,0,0);

    QLabel *profileLabel = new QLabel();
    profileLabel->setPixmap(QPixmap::fromImage(QImage(":/profile.png")));
    profileLabel->setAlignment(Qt::AlignTop);

    QLabel *descriptionLabel = new QLabel();
    descriptionLabel->setText(tr("Description"));
    descriptionLabel->setMinimumWidth(100);

    QLabel *copyrightLabel = new QLabel();
    copyrightLabel->setText(tr("Copyright"));
    copyrightLabel->setMinimumWidth(100);

    profileFileName = new QLineEdit();
    profileFileName->hide();
    profileFileName->setReadOnly(true);

    profileDescription = new QLineEdit();
    profileDescription->setMinimumWidth(400);
    profileCopyright = new QLineEdit();
    profileCopyright->setMinimumWidth(400);

    profileSaveButton = new QPushButton();
    profileSaveButton->setText(tr("Save"));
    profileSaveButton->setIcon(QIcon(":/cyan-save.png"));

    profileCloseButton = new QPushButton();
    profileCloseButton->setText(tr("Cancel"));

    mainLayout->addWidget(profileLabel);
    mainLayout->addWidget(containerFrame);

    containerLayout->addWidget(profileFileName);
    containerLayout->addWidget(descriptionFrame);
    containerLayout->addWidget(copyrightFrame);
    containerLayout->addWidget(buttonFrame);
    containerLayout->addStretch();

    copyrightLayout->addWidget(copyrightLabel);
    copyrightLayout->addWidget(profileCopyright);

    descriptionLayout->addWidget(descriptionLabel);
    descriptionLayout->addWidget(profileDescription);

    buttonLayout->addStretch();
    buttonLayout->addWidget(profileCloseButton);
    buttonLayout->addWidget(profileSaveButton);

    connect(profileCloseButton, SIGNAL(released()), this, SLOT(closeDialog()));
}

CyanProfile::~CyanProfile()
{
}

void CyanProfile::closeDialog()
{
    profileCopyright->clear();
    profileDescription->clear();
    profileFileName->clear();
    hide();
}

Cyan::Cyan(QWidget *parent)
    : QMainWindow(parent)
    , scene(0)
    , view(0)
    , mainBar(0)
    , convertBar(0)
    , profileBar(0)
    , rgbProfile(0)
    , cmykProfile(0)
    , grayProfile(0)
    , inputProfile(0)
    , outputProfile(0)
    , monitorProfile(0)
    , renderingIntent(0)
    , blackPoint(0)
    , mainBarLoadButton(0)
    , mainBarSaveButton(0)
    , menuBar(0)
    , fileMenu(0)
    , helpMenu(0)
    , openImageAction(0)
    , saveImageAction(0)
    , quitAction(0)
    , currentImageData(0)
    , currentImageProfile(0)
    , currentImageNewProfile(0)
    , currentImageThumbnail(0)
    , exportEmbeddedProfileAction(0)
    , bitDepth(0)
    , cmyLevel(0)
    , kLevel(0)
    , inkDensity(0)
    , cmykBar(0)
    , colorCyanMin(0)
    , colorCyanMax(0)
    , colorMagentaMin(0)
    , colorMagentaMax(0)
    , colorYellowMin(0)
    , colorYellowMax(0)
    , colorBlackMin(0)
    , colorBlackMax(0)
    , progBar(0)
    , colorFilterDock(0)
    , colorFilterList(0)
    , colorFilterCategory(0)
{
    qApp->setStyle(QStyleFactory::create("fusion"));

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53,53,53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(15,15,15));
    palette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    //palette.setColor(QPalette::ToolTipBase, Qt::white);
    //palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Link, Qt::white);
    palette.setColor(QPalette::LinkVisited, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53,53,53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Highlight, QColor(142,45,197).lighter());
    palette.setColor(QPalette::HighlightedText, Qt::black);
    palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    qApp->setPalette(palette);

    setWindowTitle(qApp->applicationName());
    setWindowIcon(QIcon(":/cyan.png"));

    scene = new QGraphicsScene();
    view = new CyanView();
    view->setScene(scene);
    view->fit = true;

    setCentralWidget(view);

    mainBar = new QToolBar();
    convertBar = new QToolBar();
    profileBar = new QToolBar();
    cmykBar = new QToolBar();

    mainBar->setObjectName("MainToolbar");
    mainBar->setWindowTitle(tr("Toolbar"));

    convertBar->setObjectName("ColorConverter");
    convertBar->setWindowTitle(tr("Color Converter"));

    profileBar->setObjectName("ColorManagement");
    profileBar->setWindowTitle(tr("Color Management"));

    cmykBar->setObjectName("CMYK");
    cmykBar->setWindowTitle(tr("CMYK"));

    addToolBar(Qt::TopToolBarArea, mainBar);
    addToolBar(Qt::TopToolBarArea, convertBar);
    addToolBar(Qt::TopToolBarArea, cmykBar);
    addToolBar(Qt::BottomToolBarArea, profileBar);

    insertToolBarBreak(cmykBar);

    rgbProfile = new QComboBox();
    cmykProfile = new QComboBox();
    grayProfile = new QComboBox();
    inputProfile = new QComboBox();
    outputProfile = new QComboBox();
    monitorProfile = new QComboBox();
    renderingIntent = new QComboBox();
    blackPoint = new QCheckBox();
    bitDepth = new QComboBox();
    cmyLevel = new QDoubleSpinBox();
    kLevel = new QDoubleSpinBox();
    progBar = new QProgressBar();
    colorFilterCategory = new QComboBox();

    rgbProfile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    cmykProfile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    grayProfile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    inputProfile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    outputProfile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    monitorProfile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    renderingIntent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    bitDepth->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    progBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    colorFilterCategory->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    cmyLevel->setMaximum(100);
    cmyLevel->setMinimum(0);
    kLevel->setMaximum(100);
    kLevel->setMinimum(0);
    progBar->setMaximumWidth(75);
    progBar->setRange(0,0);
    progBar->hide();

    QIcon bitDepthIcon(":/cyan-display.png");
    bitDepth->addItem(bitDepthIcon, tr("Default"));
    if (proc.quantumDepth()>=8) {
        bitDepth->addItem(bitDepthIcon, tr("8-bit"), 8);
    }
    if (proc.quantumDepth()>=16) {
        bitDepth->addItem(bitDepthIcon, tr("16-bit"), 16);
    }
    if (proc.quantumDepth()>=32) {
        bitDepth->addItem(bitDepthIcon, tr("32-bit"), 32);
    }
    bitDepth->setMaximumWidth(150);

    QIcon renderIcon(":/cyan-display.png");
    renderingIntent->addItem(renderIcon, tr("Undefined"), 0);
    renderingIntent->addItem(renderIcon, tr("Saturation"), 1);
    renderingIntent->addItem(renderIcon, tr("Perceptual"), 2);
    renderingIntent->addItem(renderIcon, tr("Absolute"), 3);
    renderingIntent->addItem(renderIcon, tr("Relative"), 4);

    QLabel *inputLabel = new QLabel();
    QLabel *outputLabel = new QLabel();
    QLabel *monitorLabel = new QLabel();
    QLabel *renderLabel = new QLabel();
    QLabel *blackLabel = new QLabel();
    QLabel *rgbLabel = new QLabel();
    QLabel *cmykLabel = new QLabel();
    QLabel *grayLabel = new QLabel();
    QLabel *bitDepthLabel = new QLabel();
    QLabel *colorCyanLabel = new QLabel();
    QLabel *colorMagentaLabel = new QLabel();
    QLabel *colorYellowLabel = new QLabel();
    QLabel *colorBlackLabel = new QLabel();
    QLabel *inkDensityLabel = new QLabel();

    cmyLevelLabel = new QLabel();
    kLevelLabel = new QLabel();
    inkDensity = new QLabel();
    colorCyanMin = new QLabel();
    colorCyanMax = new QLabel();
    colorMagentaMin = new QLabel();
    colorMagentaMax = new QLabel();
    colorYellowMin = new QLabel();
    colorYellowMax = new QLabel();
    colorBlackMin = new QLabel();
    colorBlackMax = new QLabel();

    inputLabel->setText(tr("Input"));
    outputLabel->setText(tr("Output"));
    monitorLabel->setText(tr("Monitor"));
    renderLabel->setText(tr("Intent"));
    blackLabel->setText(tr("Black Point"));
    rgbLabel->setText(tr("RGB"));
    cmykLabel->setText(tr("CMYK"));
    grayLabel->setText(tr("GRAY"));
    bitDepthLabel->setText(tr("Depth"));
    colorCyanLabel->setText(tr("Cyan"));
    colorCyanLabel->setPixmap(QPixmap::fromImage(QImage(":/cyan-color.png")));
    colorMagentaLabel->setText(tr("Magenta"));
    colorMagentaLabel->setPixmap(QPixmap::fromImage(QImage(":/magenta-color.png")));
    colorYellowLabel->setText(tr("Yellow"));
    colorYellowLabel->setPixmap(QPixmap::fromImage(QImage(":/yellow-color.png")));
    colorBlackLabel->setText(tr("Black"));
    colorBlackLabel->setPixmap(QPixmap::fromImage(QImage(":/black-color.png")));
    inkDensityLabel->setPixmap(QPixmap::fromImage(QImage(":/ink.png")));

    cmyLevelLabel->setText(tr("Color Add"));
    cmyLevelLabel->setPixmap(QPixmap::fromImage(QImage(":/cmy-color.png")));
    kLevelLabel->setText(tr("Black Substract"));
    kLevelLabel->setPixmap(QPixmap::fromImage(QImage(":/black-color.png")));
    inkDensity->setText(tr("N/A"));
    colorCyanMin->setText(tr("Min: N/A"));
    colorCyanMax->setText(tr("Max: N/A"));
    colorMagentaMin->setText(tr("Min: N/A"));
    colorMagentaMax->setText(tr("Max: N/A"));
    colorYellowMin->setText(tr("Min: N/A"));
    colorYellowMax->setText(tr("Max: N/A"));
    colorBlackMin->setText(tr("Min: N/A"));
    colorBlackMax->setText(tr("Max: N/A"));

    inputLabel->setToolTip(tr("Input profile for image"));
    outputLabel->setToolTip(tr("Profile used to convert image"));
    monitorLabel->setToolTip(tr("Monitor profile, used for proofing"));
    renderLabel->setToolTip(tr("Rendering intent used"));
    blackLabel->setToolTip(tr("Enable/Disable black point compensation"));
    rgbLabel->setToolTip(tr("Default RGB profile, used when image don't have an embedded profile"));
    cmykLabel->setToolTip(tr("Default CMYK profile, used when image don't have an embedded profile"));
    grayLabel->setToolTip(tr("Default GRAY profile, used when image don't have an embedded profile"));
    bitDepthLabel->setToolTip(tr("Adjust image output bit depth"));
    colorCyanLabel->setToolTip(tr("Cyan"));
    colorMagentaLabel->setToolTip(tr("Magenta"));
    colorYellowLabel->setToolTip(tr("Yellow"));
    colorBlackLabel->setToolTip(tr("Black"));
    inkDensityLabel->setToolTip(tr("Ink density"));

    cmyLevelLabel->setToolTip(tr("Adjust CMY levels (Add) in percent."));
    kLevelLabel->setToolTip(tr("Adjust K levels in (Substract) percent."));

    convertBar->addWidget(inputLabel);
    convertBar->addWidget(inputProfile);
    convertBar->addSeparator();
    convertBar->addWidget(outputLabel);
    convertBar->addWidget(outputProfile);
    convertBar->addWidget(bitDepthLabel);
    convertBar->addWidget(bitDepth);

    profileBar->addWidget(rgbLabel);
    profileBar->addWidget(rgbProfile);
    profileBar->addSeparator();
    profileBar->addWidget(cmykLabel);
    profileBar->addWidget(cmykProfile);
    profileBar->addSeparator();
    profileBar->addWidget(grayLabel);
    profileBar->addWidget(grayProfile);
    profileBar->addSeparator();
    profileBar->addWidget(monitorLabel);
    profileBar->addWidget(monitorProfile);
    profileBar->addSeparator();
    profileBar->addWidget(renderLabel);
    profileBar->addWidget(renderingIntent);
    profileBar->addSeparator();

    if (proc.supportBlackPoint()) {
        profileBar->addWidget(blackLabel);
        profileBar->addWidget(blackPoint);
    }

    profileBar->addSeparator();
    profileBar->addWidget(progBar);

    cmykBar->addWidget(colorCyanLabel);
    cmykBar->addWidget(colorCyanMin);
    cmykBar->addWidget(colorCyanMax);
    cmykBar->addSeparator();
    cmykBar->addWidget(colorMagentaLabel);
    cmykBar->addWidget(colorMagentaMin);
    cmykBar->addWidget(colorMagentaMax);
    cmykBar->addSeparator();
    cmykBar->addWidget(colorYellowLabel);
    cmykBar->addWidget(colorYellowMin);
    cmykBar->addWidget(colorYellowMax);
    cmykBar->addSeparator();
    cmykBar->addWidget(colorBlackLabel);
    cmykBar->addWidget(colorBlackMin);
    cmykBar->addWidget(colorBlackMax);
    cmykBar->addSeparator();
    cmykBar->addWidget(inkDensityLabel);
    cmykBar->addWidget(inkDensity);
    cmykBar->addSeparator();
    cmykBar->addWidget(cmyLevelLabel);
    cmykBar->addWidget(cmyLevel);
    cmykBar->addWidget(kLevelLabel);
    cmykBar->addWidget(kLevel);

    mainBarLoadButton = new QPushButton();
    mainBarSaveButton = new QPushButton();

    mainBarLoadButton->setToolTip(tr("Open image"));
    mainBarLoadButton->setIcon(QIcon(":/cyan-open.png"));
    mainBarSaveButton->setToolTip(tr("Save image"));
    mainBarSaveButton->setIcon(QIcon(":/cyan-save.png"));
    mainBarSaveButton->setDisabled(true);

    mainBar->addWidget(mainBarLoadButton);
    mainBar->addWidget(mainBarSaveButton);

    menuBar = new QMenuBar();
    setMenuBar(menuBar);

    fileMenu = new QMenu(tr("File"));
    helpMenu = new QMenu(tr("Help"));
    menuBar->addMenu(fileMenu);
    menuBar->addMenu(helpMenu);

    QAction *aboutAction = new QAction(tr("About ") + qApp->applicationName(), this);
    aboutAction->setIcon(QIcon(":/cyan.png"));
    helpMenu->addAction(aboutAction);

    QAction *aboutQtAction = new QAction(tr("About Qt"), this);
    helpMenu->addAction(aboutQtAction);

    openImageAction = new QAction(tr("Open image"), this);
    openImageAction->setIcon(QIcon(":/cyan-open.png"));
    openImageAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    fileMenu->addAction(openImageAction);

    saveImageAction = new QAction(tr("Save image"), this);
    saveImageAction->setIcon(QIcon(":/cyan-save.png"));
    saveImageAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    saveImageAction->setDisabled(true);
    fileMenu->addAction(saveImageAction);

    fileMenu->addSeparator();

    exportEmbeddedProfileAction = new QAction(tr("Export image profile"), this);
    exportEmbeddedProfileAction->setIcon(QIcon(":/cyan-save.png"));
    exportEmbeddedProfileAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
    exportEmbeddedProfileAction->setDisabled(true);
    fileMenu->addAction(exportEmbeddedProfileAction);

    fileMenu->addSeparator();

    quitAction = new QAction(tr("Quit"),this);
    quitAction->setIcon(QIcon(":/cyan-quit.png"));
    quitAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    fileMenu->addAction(quitAction);

    colorFilterDock = new QDockWidget(this);
    colorFilterDock->setObjectName("colorFilterDock");
    colorFilterDock->setWindowTitle(tr("Color Filters"));
    addDockWidget(Qt::RightDockWidgetArea, colorFilterDock);

    colorFilterList = new QListWidget(this);
    colorFilterList->setViewMode(QListWidget::IconMode);
    colorFilterList->setIconSize(QSize(200,200));
    colorFilterList->setResizeMode(QListWidget::Adjust);
    colorFilterList->setContextMenuPolicy(Qt::CustomContextMenu);

    QWidget *colorFilterWidget = new QWidget();
    QVBoxLayout *colorFilterDockLayout = new QVBoxLayout();
    colorFilterWidget->setLayout(colorFilterDockLayout);
    colorFilterDockLayout->addWidget(colorFilterCategory);
    colorFilterDockLayout->addWidget(colorFilterList);
    colorFilterDock->setWidget(colorFilterWidget);

    qRegisterMetaType<magentaImage>("magentaImage");
    qRegisterMetaType<magentaAdjust>("magentaAdjust");

    connect(&proc, SIGNAL(returnImage(magentaImage)), this, SLOT(getImage(magentaImage)));

    connect(aboutAction, SIGNAL(triggered()), this, SLOT(aboutCyan()));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(openImageAction, SIGNAL(triggered()), this, SLOT(openImageDialog()));
    connect(saveImageAction, SIGNAL(triggered()), this, SLOT(saveImageDialog()));
    connect(mainBarLoadButton, SIGNAL(released()), this, SLOT(openImageDialog()));
    connect(mainBarSaveButton, SIGNAL(released()), this, SLOT(saveImageDialog()));
    connect(exportEmbeddedProfileAction, SIGNAL(triggered()), this, SLOT(exportEmbeddedProfileDialog()));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    connect(rgbProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(updateRgbDefaultProfile(int)));
    connect(cmykProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCmykDefaultProfile(int)));
    connect(grayProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(updateGrayDefaultProfile(int)));
    connect(monitorProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(updateMonitorDefaultProfile(int)));

    connect(inputProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(inputProfileChanged(int)));
    connect(outputProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(outputProfileChanged(int)));
    connect(bitDepth, SIGNAL(currentIndexChanged(int)), this, SLOT(bitDepthChanged(int)));

    connect(view, SIGNAL(resetZoom()), this, SLOT(resetImageZoom()));
    connect(view, SIGNAL(openImage(QString)), this, SLOT(openImage(QString)));
    connect(view, SIGNAL(openProfile(QString)), this, SLOT(openProfile(QString)));

    connect(profileDialog.profileSaveButton, SIGNAL(released()), this, SLOT(saveProfile()));

    connect(cmyLevel, SIGNAL(editingFinished()), this, SLOT(updateImage()));
    connect(kLevel, SIGNAL(editingFinished()), this, SLOT(updateImage()));

    connect(renderingIntent, SIGNAL(currentIndexChanged(int)), this, SLOT(renderingIntentUpdated(int)));
    connect(blackPoint, SIGNAL(stateChanged(int)), this, SLOT(blackPointUpdated(int)));

    connect(colorFilterCategory, SIGNAL(currentIndexChanged(int)), this, SLOT(colorFilterCategoryChanged(int)));
    connect(&proc, SIGNAL(returnColorPreview(magentaImage)), this, SLOT(applyColorFilterThumb(magentaImage)));
    connect(colorFilterList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(colorFilterListClicked(QListWidgetItem*)));
    connect(colorFilterList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(colorFilterListClicked(QListWidgetItem*)));

    setStyleSheet(QString("QLabel {margin-left:5px;margin-right:5px;} QComboBox {padding:3px;} QLabel, QComboBox, QDoubleSpinBox, QMenuBar {font-size: %1pt;}").arg(QString::number(CYAN_FONT_SIZE)));

    QTimer::singleShot(0, this, SLOT(readConfig()));
}

Cyan::~Cyan()
{
    writeConfig();
}

void Cyan::readConfig()
{
    QSettings settings;

    settings.beginGroup("color");
    if (proc.supportBlackPoint()) {
        blackPoint->setChecked(settings.value("black").toBool());
    }
    if (settings.value("render").isValid()) {
        renderingIntent->setCurrentIndex(settings.value("render").toInt());
    }
    settings.endGroup();

    settings.beginGroup("ui");
    if (settings.value("state").isValid()) {
        restoreState(settings.value("state").toByteArray());
    }
    if (settings.value("size").isValid()) {
        resize(settings.value("size", QSize(320, 256)).toSize());
    }
    if (settings.value("pos").isValid()) {
        move(settings.value("pos", QPoint(0,0)).toPoint());
    }
    if (settings.value("max").toBool() == true) {
        this->showMaximized();
    }
    settings.endGroup();

    loadDefaultProfiles();
    loadColorFilters();
    gimpPlugin();

    bool bitDepthNoWarn = false;
    settings.beginGroup("default");
    if (settings.value("nowarning_bitdepth").isValid()) {
        bitDepthNoWarn = settings.value("nowarning_bitdepth").toBool();
    }
    if (!bitDepthNoWarn) {
        if (proc.quantumDepth() < 32) {
            QMessageBox::warning(this, tr("Cyan Quantum Depth"), tr("Quantum depth 32 is missing from backend, you will not be able to handle 32-bit images."));
            settings.setValue("nowarning_bitdepth",true);
        }
    }
    settings.endGroup();

    QStringList args = qApp->arguments();
    bool foundArg1 = false;
    for (int i = 1; i < args.size(); ++i) {
        QString file = args.at(i);
        if (!file.isEmpty()) {
            if (foundArg1) {
                lockedSaveFileName = file;
                foundArg1 = false;
            } else if (file == "-o") {
                foundArg1 = true;
            } else {
                QFile isFile(file);
                if (isFile.exists()) {
                    openImage(file);
                    break;
                }
        }
        }

    }
}

void Cyan::writeConfig()
{
    QSettings settings;
    settings.beginGroup("color");
    if (proc.supportBlackPoint()) {
        settings.setValue("black", blackPoint->isChecked());
    }
    settings.setValue("render", renderingIntent->itemData(renderingIntent->currentIndex()).toInt());
    settings.endGroup();

    settings.beginGroup("ui");
    settings.setValue( "state", saveState());
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    if (this->isMaximized()) {
        settings.setValue("max", "true");
    } else {
        settings.setValue("max", "false");
    }
    settings.endGroup();
    settings.sync();

    saveDefaultProfiles();
}

void Cyan::aboutCyan()
{
    QMessageBox aboutCyan;
    aboutCyan.setTextFormat(Qt::RichText);
    aboutCyan.setWindowTitle(tr("About")+" " + qApp->applicationName() + " " + qApp->applicationVersion());
    aboutCyan.setIconPixmap(QPixmap::fromImage(QImage(":/cyan.png")));
    aboutCyan.setText("<h1>" + qApp->applicationName() + " " + qApp->applicationVersion() + "</h1><p>Prepress Toolkit.</p>");

    QString infoText = "<p>Copyright &copy;2016, 2017 <a href=\"mailto:olear@fxarena.net\">Ole-Andr&eacute; Rodlie</a>. All rights reserved.</p><p>Cyan is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2 as published by the Free Software Foundation.<br><br>Cyan is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.</p>";
    infoText.append("<p>Includes ICC color profiles from <a href=\"http://www.basiccolor.de/\">basICColor GmbH</a>, licensed under a <a href=\"http://creativecommons.org/licenses/by-nd/3.0/\">Creative Commons Attribution-No Derivative Works 3.0</a> License.</p><p>Includes icons from the <a href=\"http://tango.freedesktop.org/Tango_Icon_Library\">Tango Icon Library</a>.</p>");
    infoText.append(proc.version());
    infoText.append("<p><img src=\":/cyan-icc2.png\">&nbsp;<img src=\":/cyan-icc4.png\"></p>");
    infoText.append("<p>Visit <a href=\"http://cyan.fxarena.net\">cyan.fxarena.net</a> or <a href=\"https://github.com/olear/cyan\">github.com/olear/cyan</a> for news and updates.</p>");
    aboutCyan.setInformativeText(infoText);

    QString detailedText;
    QFile changes(":/CHANGES");
    if (changes.open(QIODevice::ReadOnly)) {
        detailedText.append("==============================\n");
        detailedText.append(tr("CHANGELOG")+"\n");
        detailedText.append("==============================\n\n");
        detailedText.append(QString::fromUtf8(changes.readAll()));
        detailedText.append("\n\n");
        changes.close();
    }
    QFile license(":/COPYING");
    if (license.open(QIODevice::ReadOnly)) {
        detailedText.append("==============================\n");
        detailedText.append(tr("LICENSE")+"\n");
        detailedText.append("==============================\n\n");
        detailedText.append(QString::fromUtf8(license.readAll()));
        detailedText.append("\n\n");
        license.close();
    }
    QFile licenseOther(":/3rdparty.txt");
    if (licenseOther.open(QIODevice::ReadOnly)) {
        detailedText.append("==============================\n");
        detailedText.append(tr("THIRD-PARTY")+"\n");
        detailedText.append("==============================\n\n");
        detailedText.append(QString::fromUtf8(licenseOther.readAll()));
        detailedText.append("\n\n");
        licenseOther.close();
    }
    aboutCyan.setDetailedText(detailedText);

    aboutCyan.exec();
}

void Cyan::openImageDialog()
{
    QSettings settings;
    settings.beginGroup("default");

    QString file;
    QString dir;

    if (settings.value("lastDir").isValid()) {
        dir = settings.value("lastDir").toString();
    } else {
        dir = QDir::homePath();
    }

    file = QFileDialog::getOpenFileName(this, tr("Open image"), dir, tr("Image files (*.png *.jpg *.jpeg *.tif *.tiff *.psd *.icc *.icm)"));
    if (!file.isEmpty()) {
        QString suffix = QFileInfo(file).suffix().toUpper();
        if (suffix == "ICC" || suffix == "ICM") {
            openProfile(file);
        } else if (suffix == "PNG" || suffix == "JPG" || suffix == "JPEG" || suffix == "TIF" || suffix == "TIFF" || suffix == "PSD") {
            openImage(file);
        }
        QFileInfo imageFile(file);
        settings.setValue("lastDir", imageFile.absoluteDir().absolutePath());
    }

    settings.endGroup();
    settings.sync();
}

void Cyan::saveImageDialog()
{
    if (!lockedSaveFileName.isEmpty()) {
        saveImage(lockedSaveFileName);
    } else {
        QSettings settings;
        settings.beginGroup("default");

        QString file;
        QString dir;

        if (settings.value("lastSaveDir").isValid()) {
            dir = settings.value("lastSaveDir").toString();
        } else {
            dir = QDir::homePath();
        }

        file = QFileDialog::getSaveFileName(this, tr("Save image"), dir, tr("Image files (*.tif *.psd)"));
        if (!file.isEmpty()) {
            QFileInfo imageFile(file);
            if (imageFile.suffix().isEmpty()) {
                QMessageBox::warning(this, tr("File suffix is missing"), tr("File suffix is missing, please add .tif or .psd after your filename."));
                return;
            }
            saveImage(file);
            settings.setValue("lastSaveDir", imageFile.absoluteDir().absolutePath());
        }

        settings.endGroup();
        settings.sync();
    }
}

void Cyan::openImage(QString file)
{
    if (rgbProfile->itemData(rgbProfile->currentIndex()).isNull() || cmykProfile->itemData(cmykProfile->currentIndex()).isNull() || grayProfile->itemData(grayProfile->currentIndex()).isNull()) {
        QMessageBox::warning(this, "Default Color Profiles", "Please set the default color profiles (RGB/CMYK/GRAY) before loading images.");
        return;
    }

    if (!file.isEmpty()) {
        disableUI();
        QByteArray empty;
        magentaAdjust adjust;
        adjust.black = false;
        adjust.brightness = 100;
        adjust.hue = 100;
        adjust.intent = 0;
        adjust.saturation = 100;
        view->fit = true;
        proc.requestImage(false , false, file, empty, empty, empty, empty, adjust);
    }
}

void Cyan::saveImage(QString file)
{
    if (!file.isEmpty()) {
        disableUI();
        QByteArray empty;
        magentaAdjust adjust;
        adjust.black = blackPoint->isChecked();
        adjust.brightness = 100;
        adjust.hue = 100;
        adjust.intent = renderingIntent->itemData(renderingIntent->currentIndex()).toInt();
        adjust.saturation = 100;
        adjust.depth = bitDepth->itemData(bitDepth->currentIndex()).toInt();
        if (cmyLevel->value() > 0.0) {
            adjust.cmyLevel = cmyLevel->value();
        }
        if (kLevel->value() > 0.0) {
            adjust.kLevel = kLevel->value();
        }
        QListWidgetItem *colorFilterItem = colorFilterList->currentItem();
        if (colorFilterItem) {
            if (!colorFilterItem->data(COLOR_FILTER_ITEM_DATA).toString().isEmpty()) {
                adjust.clut = colorFilterItem->data(COLOR_FILTER_ITEM_DATA).toString();
            }
        }
        QByteArray currentInputProfile;
        QString selectedInputProfile = inputProfile->itemData(inputProfile->currentIndex()).toString();
        if (!selectedInputProfile.isEmpty()) {
            currentInputProfile = currentImageNewProfile;
        } else {
            currentInputProfile = currentImageProfile;
        }
        proc.requestImage(false , true, file, currentImageData, currentInputProfile, getOutputProfile(), empty, adjust);
    }
}

void Cyan::getColorProfiles(int colorspace, QComboBox *box, bool isMonitor)
{
    QString defaultProfile;
    int defaultIndex = -1;
    QSettings settings;
    settings.beginGroup("profiles");
    QString profileType;
    if (isMonitor) {
        profileType = "monitor";
    } else {
        profileType = QString::number(colorspace);
    }
    if (!settings.value(profileType).toString().isEmpty()) {
        defaultProfile = settings.value(profileType).toString();
    }
    settings.endGroup();

    QStringList profiles = cms.genProfiles(colorspace);
    if (profiles.size() > 0) {
        box->clear();
        QIcon itemIcon(":/cyan-wheel.png");
        QString noProfileText = tr("Select ...");
        if (isMonitor) {
            noProfileText = tr("None");
        }
        box->addItem(itemIcon, noProfileText);
        box->addItem("----------");
        for (int i = 0; i < profiles.size(); ++i) {
            QStringList profile = profiles.at(i).split("|");
            QString file;
            QString desc;
            if (!profile.isEmpty()) {
                file = profile.takeFirst();
                if (!profile.isEmpty()) {
                    desc = profile.takeFirst();
                }
            }
            if (!file.isEmpty()&&!desc.isEmpty()) {
                box->addItem(itemIcon, desc, file);
                if (file == defaultProfile) {
                    defaultIndex = i+2;
                }
            }
        }
        if (defaultIndex >= 0) {
            box->setCurrentIndex(defaultIndex);
        }
    }
}

void Cyan::loadDefaultProfiles()
{
    QString cyanICC1 = QDir::homePath() + "/.config";
    QString cyanICC2 = QDir::homePath() + "/.config/Cyan";
    QString cyanICC3 = QDir::homePath() + "/.config/Cyan/icc";
    QDir cyanDir(cyanICC1);
    if (!cyanDir.exists(cyanICC1)) {
        cyanDir.mkdir(cyanICC1);
    }
    if (!cyanDir.exists(cyanICC2)) {
        cyanDir.mkdir(cyanICC2);
    }
    if (!cyanDir.exists(cyanICC3)) {
        cyanDir.mkdir(cyanICC3);
    }

    QFile defRGB(cyanICC3+"/rgb.icc");
    if (!defRGB.exists()) {
        QFile::copy(":/icc/rgb.icc", cyanICC3+"/rgb.icc");
    }

    QFile defCMYK(cyanICC3+"/cmyk.icc");
    if (!defCMYK.exists()) {
        QFile::copy(":/icc/cmyk.icc", cyanICC3+"/cmyk.icc");
    }

    QFile defGRAY(cyanICC3+"/gray.icc");
    if (!defGRAY.exists()) {
        QFile::copy(":/icc/gray.icc", cyanICC3+"/gray.icc");
    }

    getColorProfiles(1, rgbProfile, false);
    getColorProfiles(2, cmykProfile, false);
    getColorProfiles(3, grayProfile, false);
    getColorProfiles(1, monitorProfile, true /*isMonitor*/);
}

void Cyan::saveDefaultProfiles()
{
    QSettings settings;
    settings.beginGroup("profiles");
    if (rgbProfile->count() > 0) {
        settings.setValue("1", rgbProfile->itemData(rgbProfile->currentIndex()));
    }
    if (cmykProfile->count() > 0) {
        settings.setValue("2", cmykProfile->itemData(cmykProfile->currentIndex()));
    }
    if (grayProfile->count() > 0) {
        settings.setValue("3", grayProfile->itemData(grayProfile->currentIndex()));
    }
    if (monitorProfile->count() > 0) {
        settings.setValue("monitor", monitorProfile->itemData(monitorProfile->currentIndex()));
    }
    settings.endGroup();
    settings.sync();
}

void Cyan::updateRgbDefaultProfile(int index)
{
    QSettings settings;
    settings.beginGroup("profiles");
    if (!rgbProfile->itemData(index).isNull()) {
        QString currentProfile = rgbProfile->itemData(index).toString();
        QString savedProfile = settings.value("1").toString();
        if (currentProfile != savedProfile) {
            settings.setValue("1", currentProfile);
        }
    }
    settings.endGroup();
    settings.sync();
}

void Cyan::updateCmykDefaultProfile(int index)
{
    QSettings settings;
    settings.beginGroup("profiles");
    if (!cmykProfile->itemData(index).isNull()) {
        QString currentProfile = cmykProfile->itemData(index).toString();
        QString savedProfile = settings.value("2").toString();
        if (currentProfile != savedProfile) {
            settings.setValue("2", currentProfile);
        }
    }
    settings.endGroup();
    settings.sync();
}

void Cyan::updateGrayDefaultProfile(int index)
{
    QSettings settings;
    settings.beginGroup("profiles");
    if (!grayProfile->itemData(index).isNull()) {
        QString currentProfile = grayProfile->itemData(index).toString();
        QString savedProfile = settings.value("3").toString();
        if (currentProfile != savedProfile) {
            settings.setValue("3", currentProfile);
        }
    }
    settings.endGroup();
    settings.sync();
}

void Cyan::updateMonitorDefaultProfile(int index)
{
    QSettings settings;
    settings.beginGroup("profiles");
    if (!monitorProfile->itemData(index).isNull()) {
        QString currentProfile = monitorProfile->itemData(index).toString();
        QString savedProfile = settings.value("monitor").toString();
        if (currentProfile != savedProfile) {
            settings.setValue("monitor", currentProfile);
        }
    }
    settings.endGroup();
    settings.sync();

    updateImage();
}

void Cyan::getImage(magentaImage result)
{
    enableUI();
    if (result.colorspace == 0) {
        QMessageBox::warning(this, tr("Unsupported Colorspace"), tr("Image has an unsupported colorspace, unable to load."));
        imageClear();
        return;
    }
    if (result.saved && result.error.isEmpty() /*&& result.warning.isEmpty()*/) {
        QFileInfo imageFile(result.filename);
        if (imageFile.exists()) {
            if (lockedSaveFileName.isEmpty()) {
                QMessageBox::information(this, tr("Image saved"), imageFile.completeBaseName() + tr(" saved to disk."));
            } else {
                qApp->quit();
            }
        } else {
            QMessageBox::warning(this, tr("Failed to save image"), tr("Failed to save image to disk"));
        }
    }
    if (!result.warning.isEmpty()) {
        QMessageBox::warning(this, tr("Cyan Warning"), result.warning);
    }
    if (result.error.isEmpty() && result.data.length() > 0 && result.profile.length() > 0) {
        if (result.inkDensity>0.0) {
            QString inkValue = QString::number(result.inkDensity);
            inkValue.append("%");
            inkDensity->setText(inkValue);
        } else {
            inkDensity->setText(tr("N/A"));
        }
        if (result.cyanMin > -1 && result.cyanMax > -1) {
            colorCyanMin->setText(tr("Min: ") + QString::number(result.cyanMin));
            colorCyanMax->setText(tr("Max: ") + QString::number(result.cyanMax));
        } else {
            colorCyanMin->setText(tr("Min: N/A"));
            colorCyanMax->setText(tr("Max: N/A"));
        }
        if (result.magentaMin > -1 && result.magentaMax > -1) {
            colorMagentaMin->setText(tr("Min: ") + QString::number(result.magentaMin));
            colorMagentaMax->setText(tr("Max: ") + QString::number(result.magentaMax));
        } else {
            colorMagentaMin->setText(tr("Min: N/A"));
            colorMagentaMax->setText(tr("Max: N/A"));
        }
        if (result.yellowMin > -1 && result.yellowMax > -1) {
            colorYellowMin->setText(tr("Min: ") + QString::number(result.yellowMin));
            colorYellowMax->setText(tr("Max: ") + QString::number(result.yellowMax));
        } else {
            colorYellowMin->setText(tr("Min: N/A"));
            colorYellowMax->setText(tr("Max: N/A"));
        }
        if (result.blackMin > -1 && result.blackMax > -1) {
            colorBlackMin->setText(tr("Min: ") + QString::number(result.blackMin));
            colorBlackMax->setText(tr("Max: ") + QString::number(result.blackMax));
        } else {
            colorBlackMin->setText(tr("Min: N/A"));
            colorBlackMax->setText(tr("Max: N/A"));
        }
        if (!result.preview) {
            imageClear();
            currentImageData = result.data;
            currentImageProfile = result.profile;
            if (result.thumb.length()>0) {
                currentImageThumbnail = result.thumb;
            }
            QFileInfo imageFile(result.filename);
            QString imageColorspace;
            switch (result.colorspace) {
            case RGB_COLORSPACE:
                imageColorspace = "RGB";
                break;
            case CMYK_COLORSPACE:
                imageColorspace = "CMYK";
                break;
            case GRAY_COLORSPACE:
                imageColorspace = "GRAY";
                break;
            }
            QString newWindowTitle = qApp->applicationName() + " - " + imageFile.fileName() + " [" + imageColorspace+"]" + " [" + cms.profileDescFromData(currentImageProfile) + "] [" + QString::number(result.width) + "x" + QString::number(result.height) + "]";
            setWindowTitle(newWindowTitle);
            getConvertProfiles();
            if (!result.hasProfile) {
                saveImageAction->setEnabled(true);
                mainBarSaveButton->setEnabled(true);
            }
            exportEmbeddedProfileAction->setEnabled(true);
            colorFilterListUpdate();
            updateImage();
        } else {
            setImage(result.data);
        }
    } else {
        if (!result.error.isEmpty()) {
            QMessageBox::warning(this, tr("Cyan Error"), result.error);
        }
        if (!result.saved) {
            imageClear();
        }
    }
}

void Cyan::imageClear()
{
    setWindowTitle(qApp->applicationName());
    currentImageData.clear();
    currentImageProfile.clear();
    currentImageNewProfile.clear();
    currentImageThumbnail.clear();
    scene->clear();
    resetImageZoom();
    mainBarSaveButton->setDisabled(true);
    saveImageAction->setDisabled(true);
    exportEmbeddedProfileAction->setDisabled(true);
    colorCyanMin->setText(tr("Min: N/A"));
    colorCyanMax->setText(tr("Max: N/A"));
    colorMagentaMin->setText(tr("Min: N/A"));
    colorMagentaMax->setText(tr("Max: N/A"));
    colorYellowMin->setText(tr("Min: N/A"));
    colorYellowMax->setText(tr("Max: N/A"));
    colorBlackMin->setText(tr("Min: N/A"));
    colorBlackMax->setText(tr("Max: N/A"));
    inkDensity->setText(tr("N/A"));
    cmyLevel->setValue(0.0);
    kLevel->setValue(0.0);
}

void Cyan::resetImageZoom()
{
    QMatrix matrix;
    matrix.scale(1.0, 1.0);
    view->setMatrix(matrix);
}

void Cyan::setImage(QByteArray image)
{
    if (image.length() > 0) {
        QPixmap pixmap(QPixmap::fromImage(QImage::fromData(image)));
        if (!pixmap.isNull()) {
            scene->clear();
            scene->addPixmap(pixmap);
            scene->setSceneRect(0, 0, pixmap.width(), pixmap.height());
        }
    }
}

void Cyan::updateImage()
{
    if (currentImageData.length() > 0 && currentImageProfile.length() > 0) {
        disableUI();
        magentaAdjust adjust;
        adjust.black = blackPoint->isChecked();
        adjust.brightness = 100;
        adjust.hue = 100;
        adjust.intent = renderingIntent->itemData(renderingIntent->currentIndex()).toInt();
        adjust.saturation = 100;
        if (cmyLevel->value() > 0.0) {
            adjust.cmyLevel = cmyLevel->value();
        }
        if (kLevel->value() > 0.0) {
            adjust.kLevel = kLevel->value();
        }
        QListWidgetItem *colorFilterItem = colorFilterList->currentItem();
        if (colorFilterItem) {
            if (!colorFilterItem->data(COLOR_FILTER_ITEM_DATA).toString().isEmpty()) {
                adjust.clut = colorFilterItem->data(COLOR_FILTER_ITEM_DATA).toString();
            }
        }
        QByteArray currentInputProfile;
        QString selectedInputProfile = inputProfile->itemData(inputProfile->currentIndex()).toString();
        if (!selectedInputProfile.isEmpty()) {
            currentInputProfile = currentImageNewProfile;
        } else {
            currentInputProfile = currentImageProfile;
        }
        proc.requestImage(true, false, "", currentImageData, currentInputProfile, getOutputProfile(), getMonitorProfile(), adjust);
    }
}

QByteArray Cyan::getMonitorProfile()
{
    QByteArray result;
    if (!monitorProfile->itemData(monitorProfile->currentIndex()).toString().isEmpty()) {
        QFile outProfileName(monitorProfile->itemData(monitorProfile->currentIndex()).toString());
        if (outProfileName.open(QIODevice::ReadOnly)) {
            result = outProfileName.readAll();
            outProfileName.close();
        }
    }
    return result;
}

QByteArray Cyan::getOutputProfile()
{
    QByteArray result;
    if (!outputProfile->itemData(outputProfile->currentIndex()).toString().isEmpty()) {
        QFile outProfileName(outputProfile->itemData(outputProfile->currentIndex()).toString());
        if (outProfileName.open(QIODevice::ReadOnly)) {
            result = outProfileName.readAll();
            outProfileName.close();
        }
    }
    return result;
}

void Cyan::getConvertProfiles()
{
    if (currentImageProfile.length() > 0) {
        int currentImageColorspace = cms.profileColorSpaceFromData(currentImageProfile);
        QStringList inputProfiles;
        QStringList outputProfiles;
        switch (currentImageColorspace) {
        case RGB_COLORSPACE:
            outputProfiles << cms.genProfiles(CMYK_COLORSPACE);
            outputProfiles << cms.genProfiles(GRAY_COLORSPACE);
            break;
        case CMYK_COLORSPACE:
            outputProfiles << cms.genProfiles(RGB_COLORSPACE);
            outputProfiles << cms.genProfiles(GRAY_COLORSPACE);
            break;
        case GRAY_COLORSPACE:
            outputProfiles << cms.genProfiles(RGB_COLORSPACE);
            outputProfiles << cms.genProfiles(CMYK_COLORSPACE);
            break;
        }
        inputProfiles << cms.genProfiles(currentImageColorspace);

        inputProfile->clear();
        outputProfile->clear();

        QIcon itemIcon(":/cyan-wheel.png");
        QString embeddedProfile = cms.profileDescFromData(currentImageProfile);
        //embeddedProfile.append(tr(" (embedded)"));

        inputProfile->addItem(itemIcon, embeddedProfile);
        inputProfile->addItem("----------");

        for (int i = 0; i < inputProfiles.size(); ++i) {
            QStringList profile = inputProfiles.at(i).split("|");
            QString file;
            QString desc;
            if (!profile.isEmpty()) {
                file = profile.takeFirst();
                if (!profile.isEmpty()) {
                    desc = profile.takeFirst();
                }
            }
            if (!file.isEmpty() && !desc.isEmpty()) {
                inputProfile->addItem(itemIcon, desc, file);
            }
        }

        outputProfile->addItem(itemIcon, tr("None"));
        outputProfile->addItem("----------");

        for (int i = 0; i < outputProfiles.size(); ++i) {
            QStringList profile = outputProfiles.at(i).split("|");
            QString file;
            QString desc;
            if (!profile.isEmpty()) {
                file = profile.takeFirst();
                if (!profile.isEmpty()) {
                    desc = profile.takeFirst();
                }
            }
            if (!file.isEmpty()&&!desc.isEmpty()) {
                outputProfile->addItem(itemIcon, desc, file);
            }
        }
    }
}

void Cyan::inputProfileChanged(int)
{
    QFile inputProfileName(inputProfile->itemData(inputProfile->currentIndex()).toString());
    if (inputProfileName.open(QIODevice::ReadOnly)) {
        currentImageNewProfile = inputProfileName.readAll();
        inputProfileName.close();
    }
    handleSaveState();
    updateImage();
}

void Cyan::outputProfileChanged(int)
{
    handleSaveState();
    updateImage();
}

void Cyan::enableUI()
{
    menuBar->setEnabled(true);
    mainBar->setEnabled(true);
    convertBar->setEnabled(true);
    profileBar->setEnabled(true);
    cmykBar->setEnabled(true);
    profileBar->actions().at(BLACKPOINT_CHECKBOX_LOCATION)->setVisible(false);
    colorFilterDock->setEnabled(true);
}

void Cyan::disableUI()
{
    menuBar->setDisabled(true);
    mainBar->setDisabled(true);
    convertBar->setDisabled(true);
    profileBar->setDisabled(true);
    cmykBar->setDisabled(true);
    profileBar->actions().at(BLACKPOINT_CHECKBOX_LOCATION)->setVisible(true);
    colorFilterDock->setDisabled(true);
}

void Cyan::exportEmbeddedProfileDialog()
{
    QSettings settings;
    settings.beginGroup("default");

    QString file;
    QString dir;

    if (settings.value("lastSaveDir").isValid()) {
        dir = settings.value("lastSaveDir").toString();
    } else {
        dir = QDir::homePath();
    }

    dir.append("/" + cms.profileDescFromData(currentImageProfile) + ".icc");

    file = QFileDialog::getSaveFileName(this, tr("Save profile"), dir, tr("Color Profiles (*.icc)"));
    if (!file.isEmpty()) {
        QFileInfo proFile(file);
        if (proFile.suffix().isEmpty()) {
            file.append(".icc");
        }
        exportEmbeddedProfile(file);
        settings.setValue("lastSaveDir", proFile.absoluteDir().absolutePath());
    }

    settings.endGroup();
    settings.sync();
}

void Cyan::exportEmbeddedProfile(QString file)
{
    if (!file.isEmpty() && currentImageProfile.length() > 0) {
        QFile proFile(file);
        if (proFile.open(QIODevice::WriteOnly)) {
            if (proFile.write(currentImageProfile) == -1) {
                QMessageBox::warning(this, tr("Unable to save profile"), tr("Unable to save profile, please check write permissions."));
            } else {
                QFileInfo proFileInfo(file);
                QMessageBox::information(this, tr("Color profile saved"), proFileInfo.completeBaseName() + tr(" Saved to disk."));
            }
            proFile.close();
        } else {
            QMessageBox::warning(this, tr("Unable to save profile"), tr("Unable to save profile, please check write permissions."));
        }
    }
}

bool Cyan::imageModified()
{
    QListWidgetItem *colorFilterItem = colorFilterList->currentItem();
    if (colorFilterItem) {
        if (!colorFilterItem->data(COLOR_FILTER_ITEM_DATA).toString().isEmpty()) {
            QString clut = colorFilterItem->data(COLOR_FILTER_ITEM_DATA).toString();
            if (!clut.isEmpty()) {
                return true;
            }
        }
    }
    if (!inputProfile->itemData(inputProfile->currentIndex()).toString().isEmpty()) {
        return true;
    }
    if (!outputProfile->itemData(outputProfile->currentIndex()).toString().isEmpty()) {
        return true;
    }
    if (bitDepth->itemData(bitDepth->currentIndex()).toString().toInt() > 0) {
        return true;
    }
    return false;
}

void Cyan::handleSaveState()
{
    if (!imageModified()) {
        saveImageAction->setDisabled(true);
        mainBarSaveButton->setDisabled(true);
    } else {
        saveImageAction->setEnabled(true);
        mainBarSaveButton->setEnabled(true);
    }
}

bool Cyan::hasProfiles()
{
    if (cms.genProfiles(RGB_COLORSPACE).size()>0) {
        return true;
    }
    return false;
}

bool Cyan::hasRGB()
{
    return hasProfiles();
}

bool Cyan::hasCMYK()
{
    if (cms.genProfiles(CMYK_COLORSPACE).size()>0) {
        return true;
    }
    return false;
}

bool Cyan::hasGRAY()
{
    if (cms.genProfiles(GRAY_COLORSPACE).size()>0) {
        return true;
    }
    return false;
}

void Cyan::bitDepthChanged(int index)
{
    Q_UNUSED(index)
    handleSaveState();
    updateImage();
}

void Cyan::gimpPlugin()
{
    QStringList versions,folders;
    versions << "2.4" << "2.6" << "2.7" << "2.8" << "2.9" << "2.10";
    foreach (QString version, versions) {
        bool hasDir = false;
        QDir gimpDir;
        QString gimpPath;
        gimpPath.append(QDir::homePath());
        gimpPath.append(QDir::separator());
#ifndef __APPLE__
        gimpPath.append(".gimp-"+version);
        if (gimpDir.exists(gimpPath)) {
            hasDir = true;
        }
#else
        gimpPath.append("Library/Application Support/GIMP/"+version);
        if (gimpDir.exists(gimpPath)) {
            hasDir = true;
        }
#endif
        if (hasDir) {
            gimpPath.append(QDir::separator());
            gimpPath.append("plug-ins");
            if (!gimpDir.exists(gimpPath)) {
                gimpDir.mkdir(gimpPath);
            }
            QString result = gimpPath;
            result.append(QDir::separator());
            result.append("cyan.py");
            folders << result;
        }
    }

    QString appPath = QString("cyanbin = \"%1\"").arg(qApp->applicationFilePath());
    foreach (QString filepath, folders) {
        QFile file(filepath);
        if (file.exists(filepath)) {
            bool rmFile = false;
            if (file.open(QIODevice::ReadOnly | QFile::Text)) {
                QTextStream s(&file);
                while (!s.atEnd()) {
                    QString line = s.readLine();
                    if (line.contains("cyanbin =")) {
                        if (line != appPath) {
                            rmFile = true;
                            break;
                        }
                    }
                }
                file.close();
            }
            if (rmFile) {
                file.remove(filepath);
            }
        } else {
            QFile sourcePy(":/gimp.py");
            if (sourcePy.open(QIODevice::ReadOnly | QFile::Text)) {
                QTextStream s(&sourcePy);
                if (file.open(QIODevice::WriteOnly | QFile::Text)) {
                    QTextStream o(&file);
                    while (!s.atEnd()) {
                        QString line = s.readLine();
                        if (line.contains("cyanbin = \"cyan\"")) {
                            line = appPath;
                        }
                        o << line << "\n";
                    }
#if QT_VERSION >= 0x050000
                    file.setPermissions(QFileDevice::ExeUser|QFileDevice::ExeGroup|QFileDevice::ExeOther|QFileDevice::ReadOwner|QFileDevice::ReadGroup|QFileDevice::ReadOther|QFileDevice::WriteUser);
#else
                    file.setPermissions(QFile::ExeUser|QFile::ExeGroup|QFile::ExeOther|QFile::ReadOwner|QFile::ReadGroup|QFile::ReadOther|QFile::WriteUser);
#endif
                    file.close();
                }
                sourcePy.close();
            }
        }
    }
}

void Cyan::openProfile(QString file)
{
    if (!file.isEmpty()) {
        profileDialog.profileCopyright->clear();
        profileDialog.profileDescription->clear();
        profileDialog.profileFileName->clear();

        profileDialog.profileCopyright->setText(cms.profileCopyrightFromFile(file));
        profileDialog.profileDescription->setText(cms.profileDescFromFile(file));
        profileDialog.profileFileName->setText(file);

        if (!profileDialog.profileDescription->text().isEmpty()) {
            profileDialog.show();
        } else {
            QMessageBox::warning(this, tr("Unsupported Color Profile"), tr("Unable to read the requested color profile."));
        }
    }
}

void Cyan::saveProfile()
{
    if (!profileDialog.profileDescription->text().isEmpty() && !profileDialog.profileFileName->text().isEmpty()) {
        QSettings settings;
        settings.beginGroup("default");

        QString file;
        QString dir;

        if (settings.value("lastSaveDir").isValid()) {
            dir = settings.value("lastSaveDir").toString();
        } else {
            dir = QDir::homePath();
        }
        QString output = QFileDialog::getSaveFileName(this, tr("Save Color Profile"), dir, tr("Color profiles (*.icc)"));
        if (!output.isEmpty()) {
            QFileInfo outFile(output);
            if (outFile.suffix().isEmpty() || outFile.suffix() != "icc") {
                output.append(".icc");
            }
            if (cms.editProfile(profileDialog.profileFileName->text(), output, profileDialog.profileDescription->text(), profileDialog.profileCopyright->text())) {
                QMessageBox::information(this, tr("Saved Color Profile"), tr("Color profile saved to disk."));
                profileDialog.profileCopyright->clear();
                profileDialog.profileDescription->clear();
                profileDialog.profileFileName->clear();
                profileDialog.hide();
            } else {
                QMessageBox::warning(this, tr("Unable to save"), tr("Failed to save color profile."));
            }
            settings.setValue("lastSaveDir", outFile.absoluteDir().absolutePath());
        }
        settings.endGroup();
        settings.sync();
    } else {
        QMessageBox::warning(this, tr("Unable to save"), tr("Nothing to save, please check the profile information."));
    }
}

void Cyan::handleConvertAdv()
{
    updateImage();
}

void Cyan::renderingIntentUpdated(int)
{
    updateImage();
}

void Cyan::blackPointUpdated(int)
{
    updateImage();
}

void Cyan::loadColorFilters()
{
    if (proc.colorFiltersPath().isEmpty()) {
        return;
    }
    /*colorFilters->clear();
    QIcon icon(":/cyan-display.png");
    colorFilters->addItem(icon, tr("None"));
    colorFilters->addItem("----------");*/
    colorFilterCategory->clear();
    QIcon icon(":/cyan-display.png");

    QDomDocument doc;
    QFile xml(":/looks.xml");
    if (!xml.open(QIODevice::ReadOnly) || !doc.setContent(&xml)) {
        return;
    }
    QDomNodeList preset = doc.elementsByTagName("preset");
    QStringList categories;
    categories << tr("None");
    for (int i = 0; i < preset.size(); i++) {
        QDomNode node = preset.item(i);
        QDomElement title = node.firstChildElement("title");
        QDomElement category = node.firstChildElement("category");
        QDomElement filename = node.firstChildElement("file");
        if (!title.isNull() && !category.isNull() && !filename.isNull()) {
            //colorFilters->addItem(icon, title.text(), category.text()+QDir::separator()+filename.text());
            categories << category.text();
        }
    }
    categories.removeDuplicates();
    for (int i = 0; i < categories.size(); i++) {
        QString catData = categories.at(i);
        QString catTitle = catData;
        if (catTitle == "bw") {
            catTitle = "Black and White";
        } else if (catTitle == "instant_consumer") {
            catTitle = "Instant (consumer)";
        } else if (catTitle == "instant_pro") {
            catTitle = "Instant (pro)";
        } else if (catTitle == "negative_color") {
            catTitle = "Negative (color)";
        } else if (catTitle == "negative_new") {
            catTitle = "Negative (new)";
        } else if (catTitle == "negative_old") {
            catTitle = "Negative (old)";
        } else if (catTitle == "picturefx") {
            catTitle = "PictureFX";
        } else if (catTitle == "print") {
            catTitle = "Print films";
        } else if (catTitle == "colorslide") {
            catTitle = "Slide (color)";
        } else if (catTitle == "various") {
            catTitle = "Various";
        }
        colorFilterCategory->addItem(icon, catTitle, catData);
    }
}

void Cyan::colorFilterListUpdate()
{
    QString currentCat = colorFilterCategory->itemData(colorFilterCategory->currentIndex()).toString();
    if (!currentCat.isEmpty() && currentImageThumbnail.length() > 0) {
        colorFilterList->clear();
        QDomDocument doc;
        QFile xml(":/looks.xml");
        if (!xml.open(QIODevice::ReadOnly) || !doc.setContent(&xml)) {
            return;
        }
        QDomNodeList preset = doc.elementsByTagName("preset");
        for (int i = 0; i < preset.size(); i++) {
            QDomNode node = preset.item(i);
            QDomElement title = node.firstChildElement("title");
            QDomElement category = node.firstChildElement("category");
            QDomElement filename = node.firstChildElement("file");
            if (!title.isNull() && !category.isNull() && !filename.isNull()) {
                if (currentCat == category.text() && colorFilterDock->isVisible()) {
                    QListWidgetItem *item = new QListWidgetItem();
                    item->setToolTip(title.text());
                    /*QString itemText = title.text().left(25);
                    if (title.text().length()>25) {
                        itemText.append("...");
                    }
                    item->setText(itemText);*/
                    item->setData(COLOR_FILTER_ITEM_DATA,category.text()+QDir::separator()+filename.text());
                    colorFilterList->addItem(item);
                    item->setHidden(true);

                    proc.requestColorPreview(item->data(COLOR_FILTER_ITEM_DATA).toString(), currentImageThumbnail);
                }
            }
        }
    }
}

void Cyan::applyColorFilterThumb(magentaImage result)
{
    if (!result.filename.isEmpty()) {
        for(int i = 0; i < colorFilterList->count(); ++i) {
            QListWidgetItem* item = colorFilterList->item(i);
            if (item) {
                if (item->data(COLOR_FILTER_ITEM_DATA).toString() == result.filename) {
                    if (result.thumb.length() > 0) {
                        QPixmap thumb( QPixmap::fromImage( QImage::fromData(result.thumb) ) );
                        item->setIcon(QIcon(thumb));
                        if (item->isHidden()) {
                            item->setHidden(false);
                        }
                    } else {
                        if (!item->isHidden()) {
                            item->setHidden(true);
                        }
                    }
                }
            }
        }
    }
}

void Cyan::colorFilterListClicked(QListWidgetItem *item)
{
    bool hasOutputProfile = false;
    QString outputProfileData = outputProfile->itemData(outputProfile->currentIndex()).toString();
    if (!outputProfileData.isEmpty()) {
        hasOutputProfile = true;
    }
    if (cms.profileColorSpaceFromData(currentImageProfile) == CMYK_COLORSPACE && !hasOutputProfile) {
        QMessageBox::warning(this, tr("Color Filters"), tr("CMYK images must be converted to RGB for the Color Filters to work, select an RGB profile in the output combobox."));
        return;
    }
    if (item) {
        if (!item->data(COLOR_FILTER_ITEM_DATA).toString().isEmpty()) {
            handleSaveState();
            updateImage();
            colorFilterList->setFocus();
        }
    }
}

void Cyan::colorFilterCategoryChanged(int index)
{
    if (index>0) {
        colorFilterListUpdate();
    } else {
        colorFilterList->clear();
        updateImage();
    }
}
