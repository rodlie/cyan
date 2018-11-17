/*
# Cyan <http://prepress.sf.net> <https://cyan.fxarena.net>,
# Copyright (C) 2016, 2017, 2018 Ole-André Rodlie<ole.andre.rodlie@gmail.com>
#
# Cyan is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.
#
# Cyan is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Cyan.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>
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
#include <QHeaderView>
#include <QMimeData>
#include <QMimeDatabase>
#include <QMimeType>

#include "helpdialog.h"

#ifdef Q_OS_MAC
#define CYAN_FONT_SIZE 10
#else
#define CYAN_FONT_SIZE 8
#endif

Cyan::Cyan(QWidget *parent)
    : QMainWindow(parent)
    , scene(Q_NULLPTR)
    , view(Q_NULLPTR)
    , mainBar(Q_NULLPTR)
    , convertBar(Q_NULLPTR)
    , profileBar(Q_NULLPTR)
    , rgbProfile(Q_NULLPTR)
    , cmykProfile(Q_NULLPTR)
    , grayProfile(Q_NULLPTR)
    , inputProfile(Q_NULLPTR)
    , outputProfile(Q_NULLPTR)
    , monitorProfile(Q_NULLPTR)
    , renderingIntent(Q_NULLPTR)
    , blackPoint(Q_NULLPTR)
    , mainBarLoadButton(Q_NULLPTR)
    , mainBarSaveButton(Q_NULLPTR)
    , menuBar(Q_NULLPTR)
    , fileMenu(Q_NULLPTR)
    , helpMenu(Q_NULLPTR)
    , openImageAction(Q_NULLPTR)
    , saveImageAction(Q_NULLPTR)
    , quitAction(Q_NULLPTR)
    , exportEmbeddedProfileAction(Q_NULLPTR)
    , bitDepth(Q_NULLPTR)
    , imageInfoDock(Q_NULLPTR)
    , imageInfoTree(Q_NULLPTR)
    , helpAction(Q_NULLPTR)
    , ignoreAction(false)
    , progBar(Q_NULLPTR)
{
    // style app
    qApp->setStyle(QStyleFactory::create("fusion"));
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
    palette.setColor(QPalette::Highlight, QColor(0,124,151));
    palette.setColor(QPalette::HighlightedText, Qt::black);
    palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    qApp->setPalette(palette);
    setStyleSheet(QString("*{font-size: %1pt;}").arg(QString::number(CYAN_FONT_SIZE)));
    QString padding = "margin-right:5px;";
    setWindowTitle(qApp->applicationName());
    setWindowIcon(QIcon(":/cyan.png"));

    // add widgets
    scene = new QGraphicsScene(this);
    view = new ImageView(this);
    view->setScene(scene);
    view->fit = true;

    setCentralWidget(view);

    mainBar = new QToolBar(this);
    convertBar = new QToolBar(this);
    profileBar = new QToolBar(this);

    mainBar->setObjectName("MainToolbar");
    mainBar->setWindowTitle(tr("Toolbar"));
    mainBar->setOrientation(Qt::Vertical);
    mainBar->setAllowedAreas(Qt::LeftToolBarArea);
    mainBar->setFloatable(false);
    mainBar->setMovable(false);
    mainBar->setIconSize(QSize(22,22));

    convertBar->setObjectName("ColorConverter");
    convertBar->setWindowTitle(tr("Color Converter"));
    convertBar->setFloatable(false);
    convertBar->setMovable(false);

    profileBar->setObjectName("ColorManagement");
    profileBar->setWindowTitle(tr("Color Management"));
    profileBar->setFloatable(false);
    profileBar->setMovable(false);
    profileBar->setAllowedAreas(Qt::BottomToolBarArea);

    imageInfoDock = new QDockWidget(this);
    imageInfoDock->setWindowTitle("Image Information");
    imageInfoDock->setObjectName("imageInformation");
    imageInfoDock->setContentsMargins(0,0,0,0);
    imageInfoDock->setFeatures(QDockWidget::DockWidgetMovable|
                               QDockWidget::DockWidgetClosable);

    addToolBar(Qt::TopToolBarArea, mainBar);
    addToolBar(Qt::TopToolBarArea, convertBar);
    addToolBar(Qt::BottomToolBarArea, profileBar);

    addDockWidget(Qt::RightDockWidgetArea, imageInfoDock);

    imageInfoTree = new QTreeWidget(this);
    QStringList imageInfoTreeLabels;
    imageInfoTreeLabels << tr("Property") << tr("Value");
    imageInfoTree->setHeaderLabels(imageInfoTreeLabels);

    imageInfoDock->setWidget(imageInfoTree);

    rgbProfile = new QComboBox(this);
    cmykProfile = new QComboBox(this);
    grayProfile = new QComboBox(this);
    inputProfile = new QComboBox(this);
    outputProfile = new QComboBox(this);
    monitorProfile = new QComboBox(this);
    renderingIntent = new QComboBox(this);
    blackPoint = new QCheckBox(this);
    bitDepth = new QComboBox(this);

    rgbProfile->setSizePolicy(QSizePolicy::Expanding,
                              QSizePolicy::Fixed);
    cmykProfile->setSizePolicy(QSizePolicy::Expanding,
                               QSizePolicy::Fixed);
    grayProfile->setSizePolicy(QSizePolicy::Expanding,
                               QSizePolicy::Fixed);
    inputProfile->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Fixed);
    outputProfile->setSizePolicy(QSizePolicy::Expanding,
                                 QSizePolicy::Fixed);
    monitorProfile->setSizePolicy(QSizePolicy::Expanding,
                                  QSizePolicy::Fixed);
    renderingIntent->setSizePolicy(QSizePolicy::Expanding,
                                   QSizePolicy::Fixed);
    bitDepth->setSizePolicy(QSizePolicy::Expanding,
                            QSizePolicy::Fixed);

    renderingIntent->setMaximumWidth(130);

    QIcon bitDepthIcon(":/cyan-display.png");
    bitDepth->addItem(bitDepthIcon, tr("Source"));
    if (supportedDepth()>=8) {
        bitDepth->addItem(bitDepthIcon, tr("8-bit"), 8);
    }
    if (supportedDepth()>=16) {
        bitDepth->addItem(bitDepthIcon, tr("16-bit"), 16);
    }
    if (supportedDepth()>=32) {
        bitDepth->addItem(bitDepthIcon, tr("32-bit"), 32);
    }
    bitDepth->setMaximumWidth(150);

    QIcon renderIcon(":/cyan-display.png");
    renderingIntent->addItem(renderIcon, tr("Undefined"),
                             FXX::UndefinedRenderingIntent);
    renderingIntent->addItem(renderIcon, tr("Saturation"),
                             FXX::SaturationRenderingIntent);
    renderingIntent->addItem(renderIcon, tr("Perceptual"),
                             FXX::PerceptualRenderingIntent);
    renderingIntent->addItem(renderIcon, tr("Absolute"),
                             FXX::AbsoluteRenderingIntent);
    renderingIntent->addItem(renderIcon, tr("Relative"),
                             FXX::RelativeRenderingIntent);

    QLabel *inputLabel = new QLabel(this);
    QLabel *outputLabel = new QLabel(this);
    QLabel *monitorLabel = new QLabel(this);
    QLabel *renderLabel = new QLabel(this);
    QLabel *blackLabel = new QLabel(this);
    QLabel *rgbLabel = new QLabel(this);
    QLabel *cmykLabel = new QLabel(this);
    QLabel *grayLabel = new QLabel(this);
    QLabel *bitDepthLabel = new QLabel(this);

    inputLabel->setText(tr("Input"));
    outputLabel->setText(tr("Output"));
    monitorLabel->setText(tr("Monitor"));
    renderLabel->setText(tr("Intent"));
    blackLabel->setText(tr("Black Point"));
    rgbLabel->setText(tr("RGB"));
    cmykLabel->setText(tr("CMYK"));
    grayLabel->setText(tr("GRAY"));
    bitDepthLabel->setText(tr("Depth"));

    inputLabel->setStyleSheet(padding);
    outputLabel->setStyleSheet(padding);
    monitorLabel->setStyleSheet(padding);
    renderLabel->setStyleSheet(padding);
    blackLabel->setStyleSheet(padding);
    rgbLabel->setStyleSheet(padding);
    cmykLabel->setStyleSheet(padding);
    grayLabel->setStyleSheet(padding);
    bitDepthLabel->setStyleSheet(padding);

    inputLabel->setToolTip(tr("Input profile for image"));
    outputLabel->setToolTip(tr("Profile used to convert image"));
    monitorLabel->setToolTip(tr("Monitor profile, used for proofing"));
    renderLabel->setToolTip(tr("Rendering intent used"));
    blackLabel->setToolTip(tr("Enable/Disable black point compensation"));
    rgbLabel->setToolTip(tr("Default RGB profile, "
                            "used when image don't have an embedded profile"));
    cmykLabel->setToolTip(tr("Default CMYK profile, "
                             "used when image don't have an embedded profile"));
    grayLabel->setToolTip(tr("Default GRAY profile, "
                             "used when image don't have an embedded profile"));
    bitDepthLabel->setToolTip(tr("Adjust image output bit depth"));

    progBar = new QProgressBar(this);
    progBar->setTextVisible(false);
    progBar->setRange(0,1);
    progBar->setValue(1);
    progBar->setMaximumWidth(70);

    convertBar->addWidget(inputLabel);
    convertBar->addWidget(inputProfile);
    convertBar->addSeparator();
    convertBar->addWidget(outputLabel);
    convertBar->addWidget(outputProfile);
    convertBar->addWidget(bitDepthLabel);
    convertBar->addWidget(bitDepth);
    convertBar->addWidget(progBar);

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
    profileBar->addWidget(blackLabel);
    profileBar->addWidget(blackPoint);

    profileBar->addSeparator();

    mainBarLoadButton = new QPushButton(this);
    mainBarSaveButton = new QPushButton(this);

    mainBarLoadButton->setToolTip(tr("Open image"));
    mainBarLoadButton->setIcon(QIcon(":/cyan-open.png"));
    mainBarSaveButton->setToolTip(tr("Save image"));
    mainBarSaveButton->setIcon(QIcon(":/cyan-save.png"));

    mainBar->addWidget(mainBarLoadButton);
    mainBar->addWidget(mainBarSaveButton);

    menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    fileMenu = new QMenu(tr("File"));
    helpMenu = new QMenu(tr("Help"));
    menuBar->addMenu(fileMenu);
    menuBar->addMenu(helpMenu);
    menuBar->setMaximumHeight(20);

    QAction *helpAction = new QAction(tr("Documentation"), this);
    helpAction->setIcon(QIcon(":/cyan.png"));
    helpMenu->addAction(helpAction);

    QAction *aboutAction = new QAction(tr("About %1")
                                       .arg(qApp->applicationName()), this);
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
    fileMenu->addAction(saveImageAction);

    fileMenu->addSeparator();

    exportEmbeddedProfileAction = new QAction(tr("Save embedded profile"), this);
    exportEmbeddedProfileAction->setIcon(QIcon(":/cyan-save.png"));
    exportEmbeddedProfileAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
    exportEmbeddedProfileAction->setDisabled(true);
    fileMenu->addAction(exportEmbeddedProfileAction);

    fileMenu->addSeparator();

    quitAction = new QAction(tr("Quit"),this);
    quitAction->setIcon(QIcon(":/cyan-quit.png"));
    quitAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    fileMenu->addAction(quitAction);

    qRegisterMetaType<FXX::Image>("FXX::Image");

    connect(&loader, SIGNAL(loadedImage(FXX::Image)),
            this, SLOT(loadedImage(FXX::Image)));
    connect(&loader, SIGNAL(convertedImage(FXX::Image)),
            this, SLOT(convertedImage(FXX::Image)));
    connect(helpAction, SIGNAL(triggered()),
            this, SLOT(openHelp()));
    connect(aboutAction, SIGNAL(triggered()),
            this, SLOT(aboutCyan()));
    connect(aboutQtAction, SIGNAL(triggered()),
            qApp, SLOT(aboutQt()));
    connect(openImageAction, SIGNAL(triggered()),
            this, SLOT(openImageDialog()));
    connect(saveImageAction, SIGNAL(triggered()),
            this, SLOT(saveImageDialog()));
    connect(mainBarLoadButton, SIGNAL(released()),
            this, SLOT(openImageDialog()));
    connect(mainBarSaveButton, SIGNAL(released()),
            this, SLOT(saveImageDialog()));
    connect(exportEmbeddedProfileAction, SIGNAL(triggered()),
            this, SLOT(exportEmbeddedProfileDialog()));
    connect(quitAction, SIGNAL(triggered()),
            qApp, SLOT(quit()));
    connect(rgbProfile, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateRgbDefaultProfile(int)));
    connect(cmykProfile, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateCmykDefaultProfile(int)));
    connect(grayProfile, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateGrayDefaultProfile(int)));
    connect(monitorProfile, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateMonitorDefaultProfile(int)));
    connect(inputProfile, SIGNAL(currentIndexChanged(int)),
            this, SLOT(inputProfileChanged(int)));
    connect(outputProfile, SIGNAL(currentIndexChanged(int)),
            this, SLOT(outputProfileChanged(int)));
    connect(bitDepth, SIGNAL(currentIndexChanged(int)),
            this, SLOT(bitDepthChanged(int)));
    connect(view, SIGNAL(resetZoom()),
            this, SLOT(resetImageZoom()));
    connect(view, SIGNAL(openImage(QString)),
            this, SLOT(openImage(QString)));
    connect(view, SIGNAL(openProfile(QString)),
            this, SLOT(openProfile(QString)));
    connect(renderingIntent, SIGNAL(currentIndexChanged(int)),
            this, SLOT(renderingIntentUpdated(int)));
    connect(blackPoint, SIGNAL(stateChanged(int)),
            this, SLOT(blackPointUpdated(int)));

    clearImageBuffer();
    QTimer::singleShot(0, this,
                       SLOT(readConfig()));
}

Cyan::~Cyan()
{
    writeConfig();
}

void Cyan::readConfig()
{
    QSettings settings;
    bool firstrun = false;

    settings.beginGroup("color");
    blackPoint->setChecked(settings.value("black").toBool());

    if (settings.value("render").isValid()) {
        renderingIntent->setCurrentIndex(settings.value("render").toInt());
    }
    settings.endGroup();

    settings.beginGroup("ui");
    if (settings.value("state").isValid()) {
        restoreState(settings.value("state").toByteArray());
    } else { firstrun = true; }

    if (settings.value("size").isValid()) {
        resize(settings.value("size", QSize(320, 256)).toSize());
    }
    if (settings.value("pos").isValid()) {
        move(settings.value("pos", QPoint(0,0)).toPoint());
    }
    if (settings.value("max").toBool() == true) {
        this->showMaximized();
    }
    if (settings.value("info_header").isValid()) {
        imageInfoTree->header()->restoreState(settings.value("info_header")
                                              .toByteArray());
    }
    settings.endGroup();

    if (firstrun) { openHelp(); }

    loadDefaultProfiles();
    gimpPlugin();

    QStringList args = qApp->arguments();
    bool foundArg1 = false;
    for (int i = 1; i < args.size(); ++i) {
        QString file = args.at(i);
        if (file.isEmpty()) { continue; }
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

void Cyan::writeConfig()
{
    QSettings settings;
    settings.beginGroup("color");
    settings.setValue("black", blackPoint->isChecked());

    settings.setValue("render", renderingIntent->itemData(renderingIntent->currentIndex())
                                                          .toInt());
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
    settings.setValue("info_header",
                      imageInfoTree->header()->saveState());
    settings.endGroup();
    settings.sync();

    saveDefaultProfiles();
}

void Cyan::aboutCyan()
{
    QMessageBox aboutCyan;
    aboutCyan.setTextFormat(Qt::RichText);
    aboutCyan.setWindowTitle(tr("About %1").arg(qApp->applicationName()));
    aboutCyan.setIconPixmap(QPixmap::fromImage(QImage(":/cyan.png")));
    aboutCyan.setText(QString("<h1 style=\"font-weight:normal;\">%1 %2</h1>"
                      "<h3 style=\"font-weight:normal;\">%3.</h3>")
                      .arg(qApp->applicationName())
                      .arg(qApp->applicationVersion())
                      .arg(tr("Prepress image viewer and converter")));

    QString infoText = "<p>Copyright &copy;2016-2018 Ole-Andr&eacute; Rodlie. All rights reserved.</p>"
                       "<p>Cyan is free software: you can redistribute it and/or modify it "
                       "under the terms of the "
                       "<a href=\"https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html\">"
                       "GNU General Public License version 2</a> as published by the "
                       "Free Software Foundation."
                       "<br><br>"
                       "Cyan is distributed in the hope that it will be useful, "
                       "but WITHOUT ANY WARRANTY; without even the implied warranty "
                       "of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. "
                       "See the GNU General Public License for more details.</p>";
    infoText.append("<p>Includes ICC color profiles from "
                    "<a href=\"http://www.basiccolor.de/\">basICColor GmbH</a>, "
                    "licensed under a <a href=\"http://creativecommons.org/licenses/by-nd/3.0/\">"
                    "Creative Commons Attribution-No Derivative Works 3.0</a> License. "
                    "Includes icons from the "
                    "<a href=\"http://tango.freedesktop.org/Tango_Icon_Library\">"
                    "Tango Icon Library</a>.</p>");
    infoText.append(QString::fromStdString(fx.backendInfo()));
    infoText.append("<p>Visit <a href=\"http://prepress.sf.net\">prepress.sf.net</a> "
                    "or <a href=\"https://cyan.fxarena.net\">cyan.fxarena.net</a> for news and updates.");
    infoText.append("<p><img src=\":/cyan-icc2.png\">&nbsp;<img src=\":/cyan-icc4.png\"></p>");
    aboutCyan.setInformativeText(infoText);

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

    file = QFileDialog::getOpenFileName(this,
                                        tr("Open image"),
                                        dir,
                                        QString("%1"
                                           " (*.png *.jpg *.jpeg *.tif *.tiff *.psd *.icc *.icm)")
                                        .arg(tr("Image files")));
    if (!file.isEmpty()) {
        QMimeDatabase db;
        QMimeType type = db.mimeTypeForFile(file);
        if (type.name().startsWith("image")) { openImage(file); }
        else if (type.name().contains("iccprofile")) { openProfile(file); }
        QFileInfo imageFile(file);
        settings.setValue("lastDir",
                          imageFile.absoluteDir().absolutePath());
    }

    settings.endGroup();
    settings.sync();
}

void Cyan::saveImageDialog()
{
    if (imageData.imageBuffer.size()==0) { return; }

    if (!lockedSaveFileName.isEmpty()) {
        saveImage(lockedSaveFileName, false /* dont notify */, true /* quit when done */);
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

        file = QFileDialog::getSaveFileName(this, tr("Save image"), dir,
                                            tr("Image files (*.tif *.psd *.jpg)"));
        if (!file.isEmpty()) {
            QFileInfo imageFile(file);
            if (imageFile.suffix().isEmpty()) {
                QMessageBox::warning(this, tr("File suffix is missing"),
                                     tr("File suffix is missing."));
                return;
            }
            saveImage(file);
            settings.setValue("lastSaveDir",
                              imageFile.absoluteDir().absolutePath());
        }

        settings.endGroup();
        settings.sync();
    }
}

void Cyan::openImage(QString file)
{
    if (file.isEmpty()) { return; }
    if (rgbProfile->itemData(rgbProfile->currentIndex()).isNull() ||
        cmykProfile->itemData(cmykProfile->currentIndex()).isNull() ||
        grayProfile->itemData(grayProfile->currentIndex()).isNull()) {
        QMessageBox::warning(this, tr("Default Color Profiles"),
                             tr("Please set the default color profiles before loading images."));
        return;
    }
    disableUI();

    // add default input profiles
    FXX::Image profiles;
    QByteArray rgbProfile = getDefaultProfile(FXX::RGBColorSpace);
    QByteArray cmykProfile = getDefaultProfile(FXX::CMYKColorSpace);
    QByteArray grayProfile = getDefaultProfile(FXX::GRAYColorSpace);
    profiles.iccRGB = std::vector<unsigned char>(rgbProfile.begin(),
                                                 rgbProfile.end());
    profiles.iccCMYK = std::vector<unsigned char>(cmykProfile.begin(),
                                                  cmykProfile.end());
    profiles.iccGRAY = std::vector<unsigned char>(grayProfile.begin(),
                                                  grayProfile.end());

    // load image
    loader.requestImage(file, profiles);
}

void Cyan::saveImage(QString file, bool notify, bool closeOnSave)
{
    if (file.isEmpty()) { return; }
    bool hasWork = imageData.workBuffer.size()>0;
    bool hasMaster = imageData.imageBuffer.size()>0;

    FXX::Image output;
    output.filename = QString(file).toStdString();
    if (hasWork) {
        output.imageBuffer = imageData.workBuffer;
    } else if (hasMaster) {
        output.imageBuffer = imageData.imageBuffer;
    } else {
        QMessageBox::warning(this, tr("No input image"),
                             tr("No input image, this should not happen!"));
        return;
    }

    disableUI();
    if (fx.saveImage(output)) {
        if (notify) {
            QMessageBox::information(this, tr("Image save"), tr("Image saved to %1.")
                                                             .arg(file));
        }
        if (closeOnSave) { QTimer::singleShot(0, qApp, SLOT(quit())); }
    } else {
        QMessageBox::warning(this, tr("Failed to save image"),
                             tr("Failed to save image, please file permissions or similar."));
    }
    enableUI();
}

void Cyan::getColorProfiles(FXX::ColorSpace colorspace,
                            QComboBox *box,
                            bool isMonitor)
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

    QMap<QString,QString> profiles = genProfiles(colorspace);
    if (profiles.size() > 0) {
        box->clear();
        QIcon itemIcon(":/cyan-wheel.png");
        QString noProfileText = tr("Select ...");
        if (isMonitor) {
            noProfileText = tr("None");
        }
        box->addItem(itemIcon, noProfileText);
        box->addItem("----------");

        int it = 0;
        QMapIterator<QString, QString> profile(profiles);
        while (profile.hasNext()) {
            profile.next();
            box->addItem(itemIcon, profile.key(), profile.value());
            if (profile.value() == defaultProfile) { defaultIndex = it+2; }
            ++it;
        }
        if (defaultIndex >= 0) {
            box->setCurrentIndex(defaultIndex);
        }
    }
}

void Cyan::loadDefaultProfiles()
{
    QString icc = QString("%1/.config/Cyan/icc")
                  .arg(QDir::homePath());
    QDir dir(icc);
    if (!dir.exists(icc)) { dir.mkpath(icc); }

    QFile defRGB(QString("%1/rgb.icc").arg(icc));
    if (!defRGB.exists()) {
        QFile::copy(":/icc/rgb.icc",
                    QString("%1/rgb.icc").arg(icc));
    }
    QFile defCMYK(QString("%1/cmyk.icc").arg(icc));
    if (!defCMYK.exists()) {
        QFile::copy(":/icc/cmyk.icc",
                    QString("%1/cmyk.icc").arg(icc));
    }
    QFile defGRAY(QString("%1/gray.icc").arg(icc));
    if (!defGRAY.exists()) {
        QFile::copy(":/icc/gray.icc",
                    QString("%1/gray.icc").arg(icc));
    }

    getColorProfiles(FXX::RGBColorSpace, rgbProfile, false);
    getColorProfiles(FXX::CMYKColorSpace, cmykProfile, false);
    getColorProfiles(FXX::GRAYColorSpace, grayProfile, false);
    getColorProfiles(FXX::RGBColorSpace, monitorProfile, true /*isMonitor*/);
}

void Cyan::saveDefaultProfiles()
{
    QSettings settings;
    settings.beginGroup("profiles");
    if (rgbProfile->count() > 0) {
        settings.setValue("1",
                          rgbProfile->itemData(rgbProfile->currentIndex()));
    }
    if (cmykProfile->count() > 0) {
        settings.setValue("2",
                          cmykProfile->itemData(cmykProfile->currentIndex()));
    }
    if (grayProfile->count() > 0) {
        settings.setValue("3",
                          grayProfile->itemData(grayProfile->currentIndex()));
    }
    if (monitorProfile->count() > 0) {
        settings.setValue("monitor",
                          monitorProfile->itemData(monitorProfile->currentIndex()));
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

void Cyan::imageClear()
{
    ignoreAction = true;
    scene->clear();
    resetImageZoom();
    clearImageBuffer();
    bitDepth->setCurrentIndex(0);
    exportEmbeddedProfileAction->setDisabled(true);
    ignoreAction = false;
}

void Cyan::resetImageZoom()
{
    QMatrix matrix;
    matrix.scale(1.0, 1.0);
    view->setMatrix(matrix);
}

void Cyan::setImage(QByteArray image)
{
    if (image.length() == 0) { return; }
    QPixmap pixmap(QPixmap::fromImage(QImage::fromData(image)));
    if (pixmap.isNull()) { return; }
    scene->clear();
    scene->addPixmap(pixmap);
    scene->setSceneRect(0, 0, pixmap.width(), pixmap.height());
}

void Cyan::updateImage()
{
    if (ignoreAction || loader.isConverting() || loader.isLoading()) {
        qDebug() << "ignore action?" << ignoreAction;
        qDebug() << "is loading?" << loader.isLoading();
        qDebug() << "is converting?" << loader.isConverting();
        return;
    }
    FXX::Image image;
    image.imageBuffer = imageData.imageBuffer;
    QString selectedInputProfile = inputProfile->itemData(inputProfile->currentIndex())
                                   .toString();
    QString selectedOutputProfile = outputProfile->itemData(outputProfile->currentIndex())
                                    .toString();
    QString selectedMonitorProfile = monitorProfile->itemData(monitorProfile->currentIndex())
                                     .toString();
    int currentDepth = bitDepth->itemData(bitDepth->currentIndex()).toInt();

    image.intent = (FXX::RenderingIntent)renderingIntent->itemData(renderingIntent->currentIndex()).toInt();
    image.blackpoint = blackPoint->isChecked();
    image.depth = (size_t)currentDepth;

    // add input profile
    if (!selectedInputProfile.isEmpty()) {
        QByteArray profile = readColorProfile(selectedInputProfile);
        if (profile.size()>0) {
            std::vector<unsigned char> buffer(profile.begin(), profile.end());
            image.iccInputBuffer = buffer;
        }
    } else {
        image.iccInputBuffer = imageData.iccInputBuffer;
    }

    // add output profile
    if (!selectedOutputProfile.isEmpty()) {
        QByteArray profile = readColorProfile(selectedOutputProfile);
        if (profile.size()>0) {
            std::vector<unsigned char> buffer(profile.begin(), profile.end());
            image.iccOutputBuffer = buffer;
        }
    } else {
        image.iccOutputBuffer = imageData.iccOutputBuffer;
    }

    // add monitor profile
    if (!selectedMonitorProfile.isEmpty()) {
        QByteArray profile = readColorProfile(selectedMonitorProfile);
        if (profile.size()>0) {
            std::vector<unsigned char> buffer(profile.begin(), profile.end());
            image.iccMonitorBuffer = buffer;
        }
    }

    // check if input profile exists
    if (image.iccInputBuffer.size()==0) { return; }

    // proc
    disableUI();
    loader.requestConvert(image);
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

QByteArray Cyan::getInputProfile()
{
    QByteArray result;
    if (!inputProfile->itemData(inputProfile->currentIndex()).toString().isEmpty()) {
        QFile outProfileName(inputProfile->itemData(inputProfile->currentIndex()).toString());
        if (outProfileName.open(QIODevice::ReadOnly)) {
            result = outProfileName.readAll();
            outProfileName.close();
        }
    }
    return result;
}

QByteArray Cyan::readColorProfile(QString file)
{
    QByteArray result;
    QFile outProfileName(file);
    if (outProfileName.open(QIODevice::ReadOnly)) {
        result = outProfileName.readAll();
        outProfileName.close();
    }
    return result;
}

void Cyan::getConvertProfiles()
{
    if (imageData.iccInputBuffer.size()==0) { return; }
    FXX::ColorSpace inputColorSpace = fx.getProfileColorspace(imageData.iccInputBuffer);
    if (inputColorSpace == FXX::UnknownColorSpace) { return; }

    ignoreAction = true;
    QMap<QString,QString> inputProfiles, outputProfiles;

    switch(inputColorSpace) {
    case FXX::RGBColorSpace:
        outputProfiles = genProfiles(FXX::CMYKColorSpace);
        outputProfiles.unite(genProfiles(FXX::GRAYColorSpace));
        break;
    case FXX::CMYKColorSpace:
        outputProfiles = genProfiles(FXX::RGBColorSpace);
        outputProfiles.unite(genProfiles(FXX::GRAYColorSpace));
        break;
    case FXX::GRAYColorSpace:
        outputProfiles = genProfiles(FXX::RGBColorSpace);
        outputProfiles.unite(genProfiles(FXX::CMYKColorSpace));
        break;
    default:;
    }
    inputProfiles = genProfiles(inputColorSpace);

    QIcon itemIcon(":/cyan-wheel.png");
    QString embeddedProfile = QString::fromStdString(fx.getProfileTag(imageData
                                                                      .iccInputBuffer));
    //embeddedProfile.append(tr(" (embedded)"));

    inputProfile->clear();
    outputProfile->clear();

    inputProfile->addItem(itemIcon, embeddedProfile);
    inputProfile->addItem("----------");
    outputProfile->addItem(itemIcon, tr("None"));
    outputProfile->addItem("----------");

    QMapIterator<QString, QString> inputMap(inputProfiles);
    while (inputMap.hasNext()) {
        inputMap.next();
        inputProfile->addItem(itemIcon, inputMap.key(), inputMap.value());
    }
    QMapIterator<QString, QString> outputMap(outputProfiles);
    while (outputMap.hasNext()) {
        outputMap.next();
        outputProfile->addItem(itemIcon, outputMap.key(), outputMap.value());
    }
    ignoreAction = false;
}

void Cyan::inputProfileChanged(int)
{
    updateImage();
}

void Cyan::outputProfileChanged(int)
{
    updateImage();
}

void Cyan::enableUI()
{
    progBar->setRange(0,1);
    progBar->setValue(1);

    menuBar->setEnabled(true);
    mainBar->setEnabled(true);
    convertBar->setEnabled(true);
    profileBar->setEnabled(true);
}

void Cyan::disableUI()
{
    progBar->setRange(0,0);
    progBar->setValue(0);

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

    dir.append(QString("/%1.icc")
               .arg(QString::fromStdString(fx.getProfileTag(imageData.iccInputBuffer))));

    file = QFileDialog::getSaveFileName(this, tr("Save profile"), dir,
                                        tr("Color Profiles (*.icc)"));
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
    if (!file.isEmpty() && imageData.iccInputBuffer.size() > 0) {
        QFile proFile(file);
        if (proFile.open(QIODevice::WriteOnly)) {
            if (proFile.write((const char*)imageData.iccInputBuffer.data(),
                              (qint64)imageData.iccInputBuffer.size()) == -1)
            {
                QMessageBox::warning(this, tr("Unable to save profile"),
                                     tr("Unable to save profile, please check write permissions."));
            } else {
                QFileInfo proFileInfo(file);
                QMessageBox::information(this, tr("Color profile saved"),
                                         proFileInfo.completeBaseName() + tr(" Saved to disk."));
            }
            proFile.close();
        } else {
            QMessageBox::warning(this, tr("Unable to save profile"),
                                 tr("Unable to save profile, please check write permissions."));
        }
    }
}

bool Cyan::hasProfiles()
{
    if (genProfiles(FXX::RGBColorSpace).size()>0) {
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
    if (genProfiles(FXX::CMYKColorSpace).size()>0) {
        return true;
    }
    return false;
}

bool Cyan::hasGRAY()
{
    if (genProfiles(FXX::GRAYColorSpace).size()>0) {
        return true;
    }
    return false;
}

void Cyan::bitDepthChanged(int index)
{
    Q_UNUSED(index)
    updateImage();
}

void Cyan::gimpPlugin()
{
    QStringList versions,folders;
    versions << "2.4" << "2.6" << "2.7" << "2.8" << "2.9" << "2.10" << "2.11" << "2.12";
    foreach (QString version, versions) {
        bool hasDir = false;
        QDir gimpDir;
        QString gimpPath;
        gimpPath.append(QDir::homePath());
        gimpPath.append(QDir::separator());
#ifndef Q_OS_MAC
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
                    file.setPermissions(QFileDevice::ExeUser|QFileDevice::ExeGroup|QFileDevice::ExeOther|QFileDevice::ReadOwner|QFileDevice::ReadGroup|QFileDevice::ReadOther|QFileDevice::WriteUser);
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
        ProfileDialog *dialog = new ProfileDialog(this, file);
        dialog->exec();
    }
}

void Cyan::renderingIntentUpdated(int)
{
    updateImage();
}

void Cyan::blackPointUpdated(int)
{
    updateImage();
}

int Cyan::supportedDepth()
{
    QString quantum = QString::fromStdString(fx.supportedQuantumDepth());
    quantum.remove("Q");
    return quantum.toInt();
}

void Cyan::loadedImage(FXX::Image image)
{
    enableUI();
    if (image.imageBuffer.size()>0 && image.error.empty()) {
        imageClear();
        if (image.previewBuffer.size()>0) {
            resetImageZoom();
            setImage(QByteArray((char*)image.previewBuffer.data(),
                                (int)image.previewBuffer.size()));
        } else {
            qDebug() << "image preview missing!!!";
        }
        imageData = image;
        exportEmbeddedProfileAction->setDisabled(imageData.iccInputBuffer.size()==0);
        if (!imageData.info.empty()) { parseImageInfo(); }
        getConvertProfiles();
        if (!monitorProfile->currentData().toString().isEmpty()) { updateImage(); }
    } else if (!image.error.empty()) {
        QMessageBox::warning(this, tr("Image error"), QString::fromStdString(image.error));
    }
    if (!image.warning.empty()) {
        QMessageBox::warning(this, tr("Image warning"), QString::fromStdString(image.warning));
    }
}

void Cyan::convertedImage(FXX::Image image)
{
    enableUI();
    if (image.previewBuffer.size()>0 && image.error.empty()) {
        setImage(QByteArray((char*)image.previewBuffer.data(),
                            (int)image.previewBuffer.size()));
        imageData.info = image.info;
        imageData.workBuffer = image.imageBuffer;
        parseImageInfo();
    } else if (!image.error.empty()) {
        QMessageBox::warning(this, tr("Image error"), QString::fromStdString(image.error));
    }
    if (!image.warning.empty()) {
        QMessageBox::warning(this, tr("Image warning"), QString::fromStdString(image.warning));
    }
}

void Cyan::clearImageBuffer()
{
    fx.clearImage(imageData);
    imageInfoTree->clear();
}

void Cyan::parseImageInfo()
{
    QString info = QString::fromStdString(imageData.info);
    if (!info.isEmpty()) {
        imageInfoTree->clear();
        QStringList list = info.split("\n", QString::SkipEmptyParts);
        QVector<QTreeWidgetItem*> level1items;
        QVector<QTreeWidgetItem*> level2items;
        QString level1 = "  ";
        QString level2 = "    ";
        QString level3 = "      ";
        bool foundHistogramTag = false;
        for (int i = 0; i < list.size(); ++i) {
            QString item = list.at(i);
            if (item.startsWith(level1)) {
                QTreeWidgetItem *levelItem = new QTreeWidgetItem();
                QString section1 = item.section(":",0,0).trimmed();
                QString section2 = item.section(":",1).trimmed();
                if (item.startsWith("  Pixels per second:") ||
                    item.startsWith("  User time:") ||
                    item.startsWith("  Elapsed time:") ||
                    item.startsWith("  Version: Image") ||
                    item.startsWith("  Format: ") ||
                    item.startsWith("  Class: "))
                {
                    continue;
                }
                levelItem->setText(0,section1);
                levelItem->setText(1,section2);
                if (item == "  Histogram:") {
                    foundHistogramTag = true;
                }
                if (foundHistogramTag && (item.startsWith("  Rendering intent:") ||
                                          item.startsWith("  Gamma:")))
                {
                    foundHistogramTag = false;
                }
                if (foundHistogramTag) {
                    continue;
                }
                if (item.startsWith(level3)) {
                    int parentID = level2items.size()-1;
                    if (parentID<0) {
                        parentID=0;
                    }
                    QTreeWidgetItem *parentItem = level2items.at(parentID);
                    if (parentItem) {
                        parentItem->addChild(levelItem);
                    }

                } else if(item.startsWith(level2)) {
                    int parentID = level1items.size()-1;
                    if (parentID<0) {
                        parentID=0;
                    }
                    QTreeWidgetItem *parentItem = level1items.at(parentID);
                    if (parentItem) {
                        parentItem->addChild(levelItem);
                        level2items << levelItem;
                    }
                } else if(item.startsWith(level1)) {
                    level1items << levelItem;
                }
                continue;
            }
        }
        imageInfoTree->addTopLevelItems(level1items.toList());
        level2items.clear();
        imageInfoTree->expandAll();
    }
}

QMap<QString, QString> Cyan::genProfiles(FXX::ColorSpace colorspace)
{
    QMap<QString,QString> output;
    QStringList folders;
    folders << QDir::rootPath() + "/WINDOWS/System32/spool/drivers/color";
    folders << "/Library/ColorSync/Profiles";
    folders << QDir::homePath() + "/Library/ColorSync/Profiles";
    folders << "/usr/share/color/icc";
    folders << "/usr/local/share/color/icc";
    folders << QDir::homePath() + "/.color/icc";
    QString cyanICCPath = QDir::homePath() + "/.config/Cyan/icc";
    QDir cyanICCDir(cyanICCPath);
    if (cyanICCDir.exists(cyanICCPath)) {
        folders << cyanICCPath;
    }
    for (int i = 0; i < folders.size(); ++i) {
        QStringList filter;
        filter << "*.icc" << "*.icm";
        QDirIterator it(folders.at(i), filter, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString iccFile = it.next();
            QString profile = QString::fromStdString(fx.getProfileTag(iccFile.toStdString()));
            if (iccFile.isEmpty() || profile.isEmpty()) { continue; }
            if (fx.getProfileColorspace(iccFile.toStdString())!=colorspace) { continue; }
            output[profile] = iccFile;
        }
    }
    return output;
}

QByteArray Cyan::getDefaultProfile(FXX::ColorSpace colorspace)
{
    QByteArray bytes;
    if (colorspace != FXX::UnknownColorSpace) {
        QString fileName;
        QSettings settings;
        settings.beginGroup("profiles");
        if (!settings.value(QString::number(colorspace)).isNull()) {
            fileName = settings.value(QString::number(colorspace)).toString();
        }
        settings.endGroup();
        if (!fileName.isEmpty()) {
            QFile proFile(fileName);
            if (proFile.exists()) {
                if (proFile.open(QIODevice::ReadOnly)) {
                    bytes = proFile.readAll();
                    proFile.close();
                }
            }
        }
    }
    return bytes;
}

void Cyan::openHelp()
{
    QFile htmlFile(":/docs/cyan.html");
    if (htmlFile.open(QIODevice::ReadOnly)) {
        QByteArray data = htmlFile.readAll();
        data.replace("_VERSION_", CYAN_VERSION);
        htmlFile.close();
        HelpDialog *dialog = new HelpDialog(this, data);
        dialog->exec();
    }
}
