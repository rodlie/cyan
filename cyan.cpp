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
#include <QSettings>
#include <QTimer>
#include <QDebug>
#include <QFileInfo>
#include <QFileDialog>
#include <QMatrix>
#include <QMessageBox>
#include <QIcon>

CyanView::CyanView(QWidget* parent) : QGraphicsView(parent) {
}

void CyanView::wheelEvent(QWheelEvent* event) {
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    double scaleFactor = 1.15;
    if(event->delta() > 0) {
        scale(scaleFactor, scaleFactor);
        emit myZoom(scaleFactor,scaleFactor);
    }
    else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        emit myZoom(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}

void CyanView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        emit resetZoom();
    } else {
        if (event->button() == Qt::RightButton) {
            emit proof();
        } else {
            QGraphicsView::mousePressEvent(event);
        }
    }
}

void CyanView::doZoom(double scaleX, double scaleY)
{
    scale(scaleX,scaleY);
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
    , monitorCheckBox(0)
{
    setWindowTitle(qApp->applicationName());
    setWindowIcon(QIcon(":/cyan.png"));

    scene = new QGraphicsScene();
    view = new CyanView();

    view->setAcceptDrops(true);
    view->setBackgroundBrush(Qt::darkGray);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setScene(scene);

    setCentralWidget(view);

    mainBar = new QToolBar();
    convertBar = new QToolBar();
    profileBar = new QToolBar();

    mainBar->setObjectName("MainToolbar");
    mainBar->setWindowTitle(tr("Main Toolbar"));

    convertBar->setObjectName("ConvertToolbar");
    convertBar->setWindowTitle(tr("Convert Toolbar"));

    profileBar->setObjectName("ProfileToolbar");
    profileBar->setWindowTitle(tr("Profiles Toolbar"));

    addToolBar(Qt::TopToolBarArea,mainBar);
    addToolBar(Qt::TopToolBarArea,convertBar);
    addToolBar(Qt::BottomToolBarArea,profileBar);

    rgbProfile = new QComboBox();
    cmykProfile = new QComboBox();
    grayProfile = new QComboBox();
    inputProfile = new QComboBox();
    outputProfile = new QComboBox();
    monitorProfile = new QComboBox();
    renderingIntent = new QComboBox();
    blackPoint = new QCheckBox();

    rgbProfile->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    cmykProfile->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    grayProfile->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    inputProfile->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    outputProfile->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    monitorProfile->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    renderingIntent->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    QLabel *inputLabel = new QLabel();
    QLabel *outputLabel = new QLabel();
    QLabel *monitorLabel = new QLabel();
    QLabel *renderLabel = new QLabel();
    QLabel *blackLabel = new QLabel();
    QLabel *rgbLabel = new QLabel();
    QLabel *cmykLabel = new QLabel();
    QLabel *grayLabel = new QLabel();

    inputLabel->setText(tr("Input"));
    outputLabel->setText(tr("Output"));
    monitorLabel->setText(tr("Monitor"));
    renderLabel->setText(tr("Intent"));
    blackLabel->setText(tr("Black Point"));
    rgbLabel->setText(tr("RGB"));
    cmykLabel->setText(tr("CMYK"));
    grayLabel->setText(tr("GRAY"));

    inputLabel->setToolTip(tr("Input profile for image"));
    outputLabel->setToolTip(tr("Profile used to convert image"));
    monitorLabel->setToolTip(tr("Monitor profile, used for proofing"));
    renderLabel->setToolTip(tr("Rendering intent used"));
    blackLabel->setToolTip(tr("Enable/Disable black point compensation"));
    rgbLabel->setToolTip(tr("Default RGB profile, used when image don't have an embedded profile"));
    cmykLabel->setToolTip(tr("Default CMYK profile, used when image don't have an embedded profile"));
    grayLabel->setToolTip(tr("Default GRAY profile, used when image don't have an embedded profile"));

    monitorCheckBox = new QCheckBox();
    monitorCheckBox->setToolTip(tr("Enable/Disable proofing"));

    convertBar->addWidget(inputLabel);
    convertBar->addWidget(inputProfile);
    convertBar->addSeparator();
    convertBar->addWidget(outputLabel);
    convertBar->addWidget(outputProfile);
    convertBar->addSeparator();
    convertBar->addWidget(monitorLabel);
    convertBar->addWidget(monitorProfile);
    convertBar->addWidget(monitorCheckBox);

    profileBar->addWidget(rgbLabel);
    profileBar->addWidget(rgbProfile);
    profileBar->addSeparator();
    profileBar->addWidget(cmykLabel);
    profileBar->addWidget(cmykProfile);
    profileBar->addSeparator();
    profileBar->addWidget(grayLabel);
    profileBar->addWidget(grayProfile);
    profileBar->addSeparator();
    profileBar->addWidget(renderLabel);
    profileBar->addWidget(renderingIntent);
    profileBar->addSeparator();
    profileBar->addWidget(blackLabel);
    profileBar->addWidget(blackPoint);

    mainBarLoadButton = new QPushButton();
    mainBarSaveButton = new QPushButton();

    mainBarLoadButton->setText(tr("Load"));
    mainBarSaveButton->setText(tr("Save"));
    mainBarSaveButton->setDisabled(true);

    mainBar->addWidget(mainBarLoadButton);
    mainBar->addWidget(mainBarSaveButton);

    menuBar = new QMenuBar();
    setMenuBar(menuBar);

    fileMenu = new QMenu(tr("File"));
    helpMenu = new QMenu(tr("Help"));
    menuBar->addMenu(fileMenu);
    menuBar->addMenu(helpMenu);

    QAction *aboutAction = new QAction(tr("About ")+qApp->applicationName(), this);
    aboutAction->setIcon(QIcon(":/cyan.png"));
    helpMenu->addAction(aboutAction);

    QAction *aboutQtAction = new QAction(tr("About Qt"), this);
    helpMenu->addAction(aboutQtAction);

    openImageAction = new QAction(tr("Open image"), this);
    openImageAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_O));
    fileMenu->addAction(openImageAction);

    saveImageAction = new QAction(tr("Save image"), this);
    saveImageAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_S));
    saveImageAction->setDisabled(true);
    fileMenu->addAction(saveImageAction);

    quitAction = new QAction(tr("Quit"),this);
    quitAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Q));
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
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    connect(rgbProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(updateRgbDefaultProfile(int)));
    connect(cmykProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCmykDefaultProfile(int)));
    connect(grayProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(updateGrayDefaultProfile(int)));
    connect(monitorProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(updateMonitorDefaultProfile(int)));

    connect(inputProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(inputProfileChanged(int)));
    connect(outputProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(outputProfileChanged(int)));
    connect(monitorCheckBox, SIGNAL(toggled(bool)), this, SLOT(monitorCheckBoxChanged(bool)));

    connect(view, SIGNAL(resetZoom()), this, SLOT(resetImageZoom()));
    connect(view, SIGNAL(proof()), this, SLOT(triggerMonitor()));

    setStyleSheet("QLabel {margin-left:10px;}");

    QTimer::singleShot(0, this, SLOT(readConfig()));
}

Cyan::~Cyan()
{
    writeConfig();
}

void Cyan::readConfig()
{
    loadDefaultProfiles();
}

void Cyan::writeConfig()
{
    saveDefaultProfiles();
}

void Cyan::aboutCyan()
{
    QMessageBox aboutCyan;
    QPixmap pixmap = QPixmap::fromImage(QImage(":/cyan-header.png"));
    aboutCyan.setIconPixmap(pixmap.scaledToWidth(480, Qt::SmoothTransformation));
    aboutCyan.setTextFormat(Qt::RichText);
    aboutCyan.setWindowTitle(tr("About")+" "+qApp->applicationName()+" "+qApp->applicationVersion());
    aboutCyan.setText("<h2>"+qApp->applicationName()+" "+qApp->applicationVersion()+"</h2><p>Prepress image viewer and converter.</p>");
    aboutCyan.setInformativeText("<p>Copyright &copy;2016 Ole-Andr&eacute; Rodlie. All rights reserved.</p><p>Cyan is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2 as published by the Free Software Foundation.</p><p><img src=\":/cyan-icc2.png\">&nbsp;<img src=\":/cyan-icc4.png\"></p>");

    QString detailedText;

    QFile license(":/COPYING");
    if (license.open(QIODevice::ReadOnly)) {
        detailedText.append(QString::fromAscii(license.readAll()));
        license.close();
    }

    aboutCyan.setDetailedText(detailedText);
    aboutCyan.exec();
}

void Cyan::openImageDialog()
{
    QString file;
    file = QFileDialog::getOpenFileName(this, tr("Open image"), QDir::homePath(), tr("Image files (*.png *.jpg *.jpeg *.tif *.tiff)"));
    if (!file.isEmpty()) {
        openImage(file);
    }
}

void Cyan::saveImageDialog()
{

}

void Cyan::openImage(QString file)
{
    if (!file.isEmpty()) {
        disableUI();
        QByteArray empty;
        magentaAdjust adjust;
        adjust.black = false;
        adjust.brightness = 100;
        adjust.hue = 100;
        adjust.intent = 0;
        adjust.saturation = 100;
        proc.requestImage(false , false, file, empty, empty, empty, empty, adjust);
    }
}

void Cyan::saveImage(QString file)
{
    Q_UNUSED(file)
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
    if (!settings.value(profileType).isNull()) {
        defaultProfile = settings.value(profileType).toString();
    }
    settings.endGroup();

    QStringList profiles = cms.genProfiles(colorspace);
    if (profiles.size()>0) {
        box->clear();
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
                QIcon itemIcon(":/cyan-wheel.png");
                box->addItem(itemIcon, desc, file);
                if (file == defaultProfile) {
                    defaultIndex = i;
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
    getColorProfiles(1, rgbProfile, false);
    getColorProfiles(2, cmykProfile, false);
    getColorProfiles(3, grayProfile, false);
    getColorProfiles(1, monitorProfile, true);
}

void Cyan::saveDefaultProfiles()
{
    QSettings settings;
    settings.beginGroup("profiles");
    if (rgbProfile->count()>0) {
        settings.setValue("1", rgbProfile->itemData(rgbProfile->currentIndex()));
    }
    if (cmykProfile->count()>0) {
        settings.setValue("2", cmykProfile->itemData(cmykProfile->currentIndex()));
    }
    if (grayProfile->count()>0) {
        settings.setValue("3", grayProfile->itemData(grayProfile->currentIndex()));
    }
    if (monitorProfile->count()>0) {
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

    if (monitorCheckBox->isChecked()) {
        updateImage();
    }
}

void Cyan::getImage(magentaImage result)
{
    enableUI();
    if (result.error.isEmpty() && result.warning.isEmpty() && result.data.length()>0 && result.profile.length()>0) {
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
            QString newWindowTitle = qApp->applicationName()+" - "+imageFile.fileName()+ " [ " + imageColorspace+" ]" + " [ " + cms.profileDescFromData(currentImageProfile)+" ] [ "+QString::number(result.width)+"x"+QString::number(result.height)+" ]";
            setWindowTitle(newWindowTitle);
            getConvertProfiles();
            updateImage();
        } else {
            setImage(result.data);
        }
    } else {
        if (!result.error.isEmpty()) {
            QMessageBox::warning(this,tr("Cyan Error"), result.error);
        }
        if (!result.warning.isEmpty()) {
            QMessageBox::warning(this,tr("Cyan Warning"), result.warning);
        }
        imageClear();
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
}

void Cyan::resetImageZoom()
{
    QMatrix matrix;
    matrix.scale(1.0,1.0);
    view->setMatrix(matrix);
}

void Cyan::setImage(QByteArray image)
{
    if (image.length()>0) {
        QPixmap pixmap(QPixmap::fromImage(QImage::fromData(image)));
        if (!pixmap.isNull()) {
            scene->clear();
            scene->addPixmap(pixmap);
            scene->setSceneRect(0,0,pixmap.width(),pixmap.height());
        }
    }
}

void Cyan::updateImage()
{
    if (currentImageData.length()>0 && currentImageProfile.length()>0) {
        disableUI();
        magentaAdjust adjust;
        adjust.black = blackPoint->isChecked();
        adjust.brightness = 100;
        adjust.hue = 100;
        adjust.intent = 0;
        adjust.saturation = 100;
        QByteArray currentInputProfile;
        QString selectedInputProfile = inputProfile->itemData(inputProfile->currentIndex()).toString();
        if (!selectedInputProfile.isEmpty()) {
            currentInputProfile = currentImageNewProfile;
        } else {
            currentInputProfile = currentImageProfile;
        }
        QByteArray proof;
        if (monitorCheckBox->isChecked()) {
            proof = getMonitorProfile();
        }
        proc.requestImage(true, false, "", currentImageData, currentInputProfile, getOutputProfile(), proof, adjust);
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
    if (currentImageProfile.length()>0) {
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
        embeddedProfile.append(tr(" (embedded)"));

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
            if (!file.isEmpty()&&!desc.isEmpty()) {
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
    updateImage();
}

void Cyan::outputProfileChanged(int index)
{
    Q_UNUSED(index)
    updateImage();
}

void Cyan::monitorCheckBoxChanged(bool triggered)
{
    Q_UNUSED(triggered)
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

void Cyan::triggerMonitor()
{
    if (monitorCheckBox->isChecked()) {
        monitorCheckBox->setChecked(false);
    } else {
        monitorCheckBox->setChecked(true);
    }
}
