/*
* Cyan <https://github.com/olear/cyan>,
* Copyright (C) 2016 Ole-André Rodlie
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
#include <QLabel>
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

CyanView::CyanView(QWidget* parent) : QGraphicsView(parent)
, fit(false) {
    setAcceptDrops(true);
    setBackgroundBrush(Qt::darkGray);
    setDragMode(QGraphicsView::ScrollHandDrag);
}

void CyanView::wheelEvent(QWheelEvent* event) {
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    double scaleFactor = 1.15;
    if(event->delta() > 0) { // up
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
    , profileManufacturer(0)
    , profileCopyright(0)
    , profileColorspace(0)
    , profileSaveButton(0)
    , profileCloseButton(0)
{
    setWindowTitle(tr("Cyan Color Profile"));
    setWindowIcon(QIcon(":/cyan.png"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    profileFileName = new QLineEdit(this);
    profileDescription = new QLineEdit(this);
    profileManufacturer = new QLineEdit(this);
    profileCopyright = new QLineEdit(this);
    profileColorspace = new QLineEdit(this);

    profileSaveButton = new QPushButton(this);
    profileSaveButton->setText(tr("Save"));
    profileCloseButton = new QPushButton(this);
    profileCloseButton->setText(tr("Close"));
    profileCloseButton->setFocus();

    mainLayout->addWidget(profileFileName);
    mainLayout->addWidget(profileDescription);
    mainLayout->addWidget(profileManufacturer);
    mainLayout->addWidget(profileCopyright);
    mainLayout->addWidget(profileColorspace);
    mainLayout->addWidget(profileSaveButton);
    mainLayout->addWidget(profileCloseButton);

    connect(profileCloseButton, SIGNAL(released()), this, SLOT(closeDialog()));
}

CyanProfile::~CyanProfile()
{
}

void CyanProfile::closeDialog()
{
    profileColorspace->clear();
    profileCopyright->clear();
    profileDescription->clear();
    profileFileName->clear();
    profileManufacturer->clear();
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
    , exportEmbeddedProfileAction(0)
    , bitDepth(0)
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

    mainBar->setObjectName("MainToolbar");
    mainBar->setWindowTitle(tr("Toolbar"));

    convertBar->setObjectName("ColorConverter");
    convertBar->setWindowTitle(tr("Color Converter"));

    profileBar->setObjectName("ColorManagement");
    profileBar->setWindowTitle(tr("Color Management"));

    addToolBar(Qt::TopToolBarArea, mainBar);
    addToolBar(Qt::TopToolBarArea, convertBar);
    addToolBar(Qt::BottomToolBarArea, profileBar);

    rgbProfile = new QComboBox();
    cmykProfile = new QComboBox();
    grayProfile = new QComboBox();
    inputProfile = new QComboBox();
    outputProfile = new QComboBox();
    monitorProfile = new QComboBox();
    renderingIntent = new QComboBox();
    blackPoint = new QCheckBox();
    bitDepth = new QComboBox();

    rgbProfile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    cmykProfile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    grayProfile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    inputProfile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    outputProfile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    monitorProfile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    renderingIntent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    bitDepth->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QIcon bitDepthIcon(":/cyan-display.png");
    bitDepth->addItem(bitDepthIcon, tr("Default"));
    if (proc.quantumDepth()>=8) {
        bitDepth->addItem(bitDepthIcon, tr("8-bit"),8);
    }
    if (proc.quantumDepth()>=16) {
        bitDepth->addItem(bitDepthIcon, tr("16-bit"),16);
    }
    if (proc.quantumDepth()>=32) {
        bitDepth->addItem(bitDepthIcon, tr("32-bit"),32);
    }
    bitDepth->setMaximumWidth(150);

    QIcon renderIcon(":/cyan-display.png");
    renderingIntent->addItem(renderIcon, tr("Undefined"), 0);
    renderingIntent->addItem(renderIcon, tr("Saturation"), 1);
    renderingIntent->addItem(renderIcon, tr("Perceptual"), 2);
    renderingIntent->addItem(renderIcon, tr("Absolute"), 3);

    QLabel *inputLabel = new QLabel();
    QLabel *outputLabel = new QLabel();
    QLabel *monitorLabel = new QLabel();
    QLabel *renderLabel = new QLabel();
    QLabel *blackLabel = new QLabel();
    QLabel *rgbLabel = new QLabel();
    QLabel *cmykLabel = new QLabel();
    QLabel *grayLabel = new QLabel();
    QLabel *bitDepthLabel = new QLabel();

    inputLabel->setText(tr("Input"));
    outputLabel->setText(tr("Output"));
    monitorLabel->setText(tr("Monitor"));
    renderLabel->setText(tr("Intent"));
    blackLabel->setText(tr("Black Point"));
    rgbLabel->setText(tr("RGB"));
    cmykLabel->setText(tr("CMYK"));
    grayLabel->setText(tr("GRAY"));
    bitDepthLabel->setText(tr("Depth"));

    inputLabel->setToolTip(tr("Input profile for image"));
    outputLabel->setToolTip(tr("Profile used to convert image"));
    monitorLabel->setToolTip(tr("Monitor profile, used for proofing"));
    renderLabel->setToolTip(tr("Rendering intent used"));
    blackLabel->setToolTip(tr("Enable/Disable black point compensation"));
    rgbLabel->setToolTip(tr("Default RGB profile, used when image don't have an embedded profile"));
    cmykLabel->setToolTip(tr("Default CMYK profile, used when image don't have an embedded profile"));
    grayLabel->setToolTip(tr("Default GRAY profile, used when image don't have an embedded profile"));
    bitDepthLabel->setToolTip(tr("Adjust image output bit depth"));

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

    setStyleSheet("QLabel {margin-left:5px;margin-right:5px;} QComboBox {padding:3px;}");

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
    gimpPlugin();

    if (proc.quantumDepth() < 32) {
        QMessageBox::warning(this, tr("Cyan Quantum Depth"), tr("Quantum depth 32 is missing from backend, you will not be able to handle 32-bit images."));
    }

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
    aboutCyan.setText("<h1>" + qApp->applicationName() + " " + qApp->applicationVersion() + "</h1><p>Prepress image viewer and converter.</p>");

    QString infoText = "<p>Copyright &copy;2016, 2017 <a href=\"mailto:olear@fxarena.net\">Ole-Andr&eacute; Rodlie</a>. All rights reserved.</p><p>Cyan is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2 as published by the Free Software Foundation.</p>";
    infoText.append("<p>Includes ICC color profiles from <a href=\"http://www.basiccolor.de/\">basICColor GmbH</a>, licensed under a <a href=\"http://creativecommons.org/licenses/by-nd/3.0/\">Creative Commons Attribution-No Derivative Works 3.0</a> License.</p>");
    infoText.append(proc.version());
    infoText.append("<p><img src=\":/cyan-icc2.png\">&nbsp;<img src=\":/cyan-icc4.png\"></p>");
    infoText.append("<p>Visit <a href=\"https://github.com/olear/cyan\">github.com/olear/cyan</a> for news and updates.</p>");
    aboutCyan.setInformativeText(infoText);

    QString detailedText;
    QFile license(":/COPYING");
    if (license.open(QIODevice::ReadOnly)) {
        detailedText.append(QString::fromUtf8(license.readAll()));
        license.close();
    }
    QFile licenseOther(":/3rdparty.txt");
    if (licenseOther.open(QIODevice::ReadOnly)) {
        detailedText.append(QString::fromUtf8(licenseOther.readAll()));
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
        if (file.contains(QRegExp("(.icc|.icm)",Qt::CaseInsensitive))) {
            openProfile(file);
        } else {
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
    if (!hasRGB()) {
        QFile defRGB(cyanICC3+"/sRGB.icc");
        if (!defRGB.exists()) {
            QFile::copy(":/icc/sRGB.icc", cyanICC3+"/sRGB.icc");
        }
    }
    if (!hasCMYK()) {
        QFile defCMYK(cyanICC3+"/ISOcoated_v2_bas.ICC");
        if (!defCMYK.exists()) {
            QFile::copy(":/icc/ISOcoated_v2_bas.ICC", cyanICC3+"/ISOcoated_v2_bas.ICC");
        }
    }
    if (!hasGRAY()) {
        QFile defGRAY(cyanICC3+"/ISOcoated_v2_grey1c_bas.ICC");
        if (!defGRAY.exists()) {
            QFile::copy(":/icc/ISOcoated_v2_grey1c_bas.ICC", cyanICC3+"/ISOcoated_v2_grey1c_bas.ICC");
        }
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
    if (result.saved && result.error.isEmpty() && result.warning.isEmpty()) {
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
    if (result.error.isEmpty() && result.warning.isEmpty() && result.data.length() > 0 && result.profile.length() > 0) {
        if (!result.preview) {
            imageClear();
            currentImageData = result.data;
            currentImageProfile = result.profile;
            QFileInfo imageFile(result.filename);
            QString imageColorspace;
            switch (result.colorspace) {
            case 1:
                imageColorspace = "RGB";
                break;
            case 2:
                imageColorspace = "CMYK";
                break;
            case 3:
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
            updateImage();
        } else {
            setImage(result.data);
        }
    } else {
        if (!result.error.isEmpty()) {
            QMessageBox::warning(this, tr("Cyan Error"), result.error);
        }
        if (!result.warning.isEmpty()) {
            QMessageBox::warning(this, tr("Cyan Warning"), result.warning);
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
    scene->clear();
    resetImageZoom();
    mainBarSaveButton->setDisabled(true);
    saveImageAction->setDisabled(true);
    exportEmbeddedProfileAction->setDisabled(true);
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
        case 1:
            outputProfiles << cms.genProfiles(2);
            outputProfiles << cms.genProfiles(3);
            break;
        case 2:
            outputProfiles << cms.genProfiles(1);
            outputProfiles << cms.genProfiles(3);
            break;
        case 3:
            outputProfiles << cms.genProfiles(1);
            outputProfiles << cms.genProfiles(2);
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

void Cyan::inputProfileChanged(int index)
{
    Q_UNUSED(index)
    QFile inputProfileName(inputProfile->itemData(inputProfile->currentIndex()).toString());
    if (inputProfileName.open(QIODevice::ReadOnly)) {
        currentImageNewProfile = inputProfileName.readAll();
        inputProfileName.close();
    }
    handleSaveState();
    updateImage();
}

void Cyan::outputProfileChanged(int index)
{
    Q_UNUSED(index)
    handleSaveState();
    updateImage();
}

void Cyan::enableUI()
{
    menuBar->setEnabled(true);
    mainBar->setEnabled(true);
    convertBar->setEnabled(true);
    profileBar->setEnabled(true);
}

void Cyan::disableUI()
{
    menuBar->setDisabled(true);
    mainBar->setDisabled(true);
    convertBar->setDisabled(true);
    profileBar->setDisabled(true);
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
    if (cms.genProfiles(1).size()>0) {
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
    if (cms.genProfiles(2).size()>0) {
        return true;
    }
    return false;
}

bool Cyan::hasGRAY()
{
    if (cms.genProfiles(3).size()>0) {
        return true;
    }
    return false;
}

void Cyan::bitDepthChanged(int index)
{
    Q_UNUSED(index)
    handleSaveState();
}

void Cyan::gimpPlugin()
{
    QStringList versions,folders;
    versions << "2.4" << "2.6" << "2.8" << "2.9" << "2.10";
    foreach (QString version, versions) {
        QDir gimpDir;
        QString gimpPath;
        gimpPath.append(QDir::homePath());
        gimpPath.append(QDir::separator());
#ifndef __APPLE__
        gimpPath.append(".gimp-"+version);
        if (!gimpDir.exists(gimpPath)) {
            gimpDir.mkdir(gimpPath);
        }
#else
        gimpPath.append("Library/Application Support/GIMP");
        if (!gimpDir.exists(gimpPath)) {
            gimpDir.mkdir(gimpPath);
        }
        gimpPath.append("/"+version);
        if (!gimpDir.exists(gimpPath)) {
            gimpDir.mkdir(gimpPath);
        }
#endif
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
        profileDialog.profileColorspace->clear();
        profileDialog.profileCopyright->clear();
        profileDialog.profileDescription->clear();
        profileDialog.profileFileName->clear();
        profileDialog.profileManufacturer->clear();

        profileDialog.profileColorspace->setText(QString::number(cms.profileColorSpaceFromFile(file)));
        profileDialog.profileCopyright->setText(cms.profileCopyrightFromFile(file));
        profileDialog.profileDescription->setText(cms.profileDescFromFile(file));
        profileDialog.profileFileName->setText(file);
        profileDialog.profileManufacturer->setText(cms.profileManufacturerFromFile(file));

        if (!profileDialog.profileDescription->text().isEmpty() && !profileDialog.profileColorspace->text().isEmpty()) {
            profileDialog.show();
        } else {
            QMessageBox::warning(this, tr("Unsupported Color Profile"), tr("Unable to read the requested color profile."));
        }
    }
}

void Cyan::saveProfile()
{
    if (!profileDialog.profileDescription->text().isEmpty() && !profileDialog.profileColorspace->text().isEmpty()) {
        QString output = QFileDialog::getSaveFileName(this, tr("Save Color Profile"), QDir::homePath(), tr("Color profiles (*.icc *.icm)"));
        if (!output.isEmpty()) {
            if (cms.editProfile(profileDialog.profileFileName->text(), output, profileDialog.profileDescription->text(), profileDialog.profileCopyright->text())) {
                QMessageBox::information(this, tr("Saved Color Profile"), tr("Color profile saved to disk."));
                profileDialog.profileColorspace->clear();
                profileDialog.profileCopyright->clear();
                profileDialog.profileDescription->clear();
                profileDialog.profileFileName->clear();
                profileDialog.profileManufacturer->clear();
                profileDialog.hide();
            } else {
                QMessageBox::warning(this, tr("Unable to save"), tr("Failed to save color profile."));
            }
        }
    } else {
        QMessageBox::warning(this, tr("Unable to save"), tr("Nothing to save, please check the profile information."));
    }
}
