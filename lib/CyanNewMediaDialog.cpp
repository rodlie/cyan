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

#include "CyanNewMediaDialog.h"

#include <QIcon>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMapIterator>
#include <QDebug>
#include <QSettings>
#include <QColor>
#include <QDateTime>
#include <QtConcurrent/qtconcurrentrun.h>
#include <QMessageBox>

#include "CyanImageFormat.h"
#include "CyanColorConvert.h"

NewMediaDialog::NewMediaDialog(QWidget *parent,
                               QString title,
                               CyanCommon::newDialogType dialogType,
                               Magick::ColorspaceType colorspace,
                               Magick::Blob profile,
                               QSize size, int depth) :
    QDialog (parent)
  , _type(dialogType)
  , _width(nullptr)
  , _height(nullptr)
  , _ok(nullptr)
  , _cancel(nullptr)
  , _colorspace(colorspace)
  , _label(nullptr)
  , _select(nullptr)
  , _profile(nullptr)
  , _forcedProfile(profile)
  , _colorPickerFrom(nullptr)
  , _colorPickerTo(nullptr)
  , _drawOption(nullptr)
  , _bitBox(nullptr)
  , _progbar(nullptr)
{
    setWindowTitle(title);
    setWindowIcon(_type==CyanCommon::newImageDialogType?QIcon::fromTheme("document-new"):QIcon::fromTheme("layer"));
    setMinimumWidth(250);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    _label = new QLineEdit(this);
    _width  = new QSpinBox(this);
    _height = new QSpinBox(this);
    _ok = new QPushButton(this);
    _cancel = new QPushButton(this);
    _select = new QComboBox(this);
    _profile = new QComboBox(this);

    _progbar = new QProgressBar(this);
    _progbar->setMinimum(0);
    _progbar->setMaximum(0);
    _progbar->setValue(-1);
    _progbar->setHidden(true);

    _bitBox = new QComboBox(this);
    _bitBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    int _depth = CyanImageFormat::supportedQuantumDepth();
    if (_depth >= 8) {
        _bitBox->addItem(tr("8-bit"), 8);
    }
    if (_depth >= 16) {
        _bitBox->addItem(tr("16-bit"), 16);
    }
    if (_depth >= 32) {
        _bitBox->addItem(tr("32-bit"), 32);
    }
    if (_depth == 64) {
        _bitBox->addItem(tr("64-bit"), 64);
    }

    switch (depth) {
    case 8:
        _bitBox->setCurrentIndex(0);
        break;
    case 16:
        _bitBox->setCurrentIndex(1);
        break;
    case 32:
        _bitBox->setCurrentIndex(2);
        break;
    case 64:
        _bitBox->setCurrentIndex(3);
    default:
        break;
    }

    _colorPickerFrom = new QtColorPicker(this, -1, true, false);
    _colorPickerTo = new QtColorPicker(this, -1, true, false);

    _colorPickerFrom->setStandardColors();
    _colorPickerTo->setStandardColors();

    _drawOption = new QComboBox(this);
    _drawOption->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);

    _drawOption->addItem(tr("Transparent"), CyanDrawTextureTransparent);
    _drawOption->addItem(tr("Solid color"), CyanDrawTextureSolid);
    _drawOption->addItem(tr("Gradient"), CyanDrawTextureGradient);
    _drawOption->addItem(tr("Gradient (Color From/To)"), CyanDrawTextureGradientColor);
    _drawOption->addItem(tr("Radial Gradient"), CyanDrawTextureRadial);
    _drawOption->addItem(tr("Radial Gradient (Color From/To)"), CyanDrawTextureRadialColor);
    _drawOption->addItem(tr("Plasma"), CyanDrawTexturePlasma);
    _drawOption->addItem(tr("Plasma Fractal"), CyanDrawTexturePlasmaFractal);
    _drawOption->addItem(tr("Plasma (Color From/To)"), CyanDrawTexturePlasmaColor);
    _drawOption->addItem(tr("Gaussian Noise"), CyanDrawTextureGaussianNoise);
    _drawOption->addItem(tr("Impulse Noise"), CyanDrawTextureImpulseNoise);
    _drawOption->addItem(tr("Laplacian Noise"), CyanDrawTextureLaplacianNoise);
    _drawOption->addItem(tr("Loops (Gaussian)"), CyanDrawTextureLoopsGaussian);
    _drawOption->addItem(tr("Loops (Impulse)"), CyanDrawTextureLoopsImpulse);
    _drawOption->addItem(tr("Loops (Laplacian)"), CyanDrawTextureLoopsImpulse);

    _profile->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);

    _ok->setIcon(QIcon::fromTheme("document-new"));
    _cancel->setIcon(QIcon::fromTheme("process-stop"));

    QWidget *buttonWidget = new QWidget(this);
    buttonWidget->setContentsMargins(0,0,0,0);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0,0,0,0);

    QWidget *sizeWidget = new QWidget(this);
    sizeWidget->setContentsMargins(0,0,0,0);
    QHBoxLayout *sizeLayout = new QHBoxLayout(sizeWidget);
    sizeLayout->setContentsMargins(0,0,0,0);

    QWidget *colorWidget = new QWidget(this);
    colorWidget->setContentsMargins(0,0,0,0);
    QHBoxLayout *colorLayout = new QHBoxLayout(colorWidget);
    colorLayout->setContentsMargins(0,0,0,0);

    colorLayout->addStretch();
    colorLayout->addWidget(_colorPickerFrom);
    colorLayout->addWidget(_colorPickerTo);

    QWidget *backgroundWidget = new QWidget(this);
    backgroundWidget->setContentsMargins(0,0,0,0);
    QHBoxLayout *backgroundLayout = new QHBoxLayout(backgroundWidget);
    backgroundLayout->setContentsMargins(0,0,0,0);

    QLabel *backgroundLabel = new QLabel(this);
    backgroundLabel->setText(tr("Background"));

    backgroundLayout->addWidget(backgroundLabel);
    backgroundLayout->addWidget(_drawOption);

    _width->setRange(1, 10000);
    _height->setRange(1, 10000);

    if (size.width()>0 && size.height()>0) {
        _width->setValue(size.width());
        _height->setValue(size.height());
    } else {
        _width->setValue(1024);
        _height->setValue(1024);
    }

    _label->setPlaceholderText(title);

    _ok->setText(tr("New"));
    _cancel->setText(tr("Cancel"));

    QIcon colorsIcon = QIcon::fromTheme("colors");

    _select->addItem(colorsIcon, tr("RGB"), 0);
    _select->addItem(colorsIcon, tr("CMYK"), 1);
    _select->addItem(colorsIcon, tr("GRAY"), 2);

    QLabel *widthLabel = new QLabel(this);
    widthLabel->setText(tr("Width"));
    QLabel *heightLabel = new QLabel(this);
    heightLabel->setText(tr("Height"));

    sizeLayout->addWidget(widthLabel);
    sizeLayout->addWidget(_width);
    sizeLayout->addWidget(heightLabel);
    sizeLayout->addWidget(_height);

    buttonLayout->addStretch();
    buttonLayout->addWidget(_ok);
    buttonLayout->addWidget(_cancel);

    //
    QWidget *colorspaceWidget = new QWidget(this);
    colorspaceWidget->setContentsMargins(0,0,0,0);
    QHBoxLayout *colorspaceLayout = new QHBoxLayout(colorspaceWidget);
    colorspaceLayout->setContentsMargins(0,0,0,0);

    QLabel *colorspaceLabel = new QLabel(this);
    colorspaceLabel->setText(tr("Color space"));

    colorspaceLayout->addWidget(colorspaceLabel);
    colorspaceLayout->addWidget(_select);

    //
    QWidget *colorprofileWidget = new QWidget(this);
    colorprofileWidget->setContentsMargins(0,0,0,0);
    QHBoxLayout *colorprofileLayout = new QHBoxLayout(colorprofileWidget);
    colorprofileLayout->setContentsMargins(0,0,0,0);

    QLabel *colorprofileLabel = new QLabel(this);
    colorprofileLabel->setText(tr("Color profile"));

    colorprofileLayout->addWidget(colorprofileLabel);
    colorprofileLayout->addWidget(_profile);

    //
    QWidget *colordepthWidget = new QWidget(this);
    colordepthWidget->setContentsMargins(0,0,0,0);
    QHBoxLayout *colordepthLayout = new QHBoxLayout(colordepthWidget);
    colordepthLayout->setContentsMargins(0,0,0,0);

    QLabel *colordepthLabel = new QLabel(this);
    colordepthLabel->setText(tr("Color depth"));

    colordepthLayout->addWidget(colordepthLabel);
    colordepthLayout->addWidget(_bitBox);

    //
    mainLayout->addWidget(_label);
    mainLayout->addWidget(sizeWidget);
    mainLayout->addWidget(colorspaceWidget);
    mainLayout->addWidget(colorprofileWidget);
    mainLayout->addWidget(colordepthWidget);
    mainLayout->addWidget(backgroundWidget);
    mainLayout->addWidget(colorWidget);
    mainLayout->addStretch();
    mainLayout->addWidget(_progbar);
    mainLayout->addWidget(buttonWidget);

    if (_type == CyanCommon::newLayerDialogType) {
        colorspaceWidget->setHidden(true);
        colorprofileWidget->setHidden(true);
    }
    handleColorspaceChanged();

    connect(_ok,
            SIGNAL(released()),
            this,
            SLOT(handleOk()));
    connect(_cancel,
            SIGNAL(released()),
            this,
            SLOT(handleCancel()));
    connect(_select,
            SIGNAL(currentIndexChanged(int)),
            this,
            SLOT(handleColorspaceChanged(int)));
    connect(this,
            SIGNAL(createImageFinished()),
            this,
            SLOT(handleCreatedImage()));
}

NewMediaDialog::~NewMediaDialog()
{

}

Magick::Image NewMediaDialog::getImage()
{
    return _image;
}

void NewMediaDialog::handleOk()
{
    Magick::ColorspaceType type = _colorspace;
    bool warnColorspace = false;
    QString warncolorspaceText;
    if (_type == CyanCommon::newImageDialogType) {
        switch (_select->currentData().toInt()) {
        case 0:
            type = Magick::sRGBColorspace;
            break;
        case 1:
            type = Magick::CMYKColorspace;
            break;
        case 2:
            type = Magick::GRAYColorspace;
            break;
        }
    }
    switch(_colorspace) {
        case Magick::CMYKColorspace:
        warnColorspace = true;
        warncolorspaceText = "CMYK";
        break;
    case Magick::GRAYColorspace:
        warnColorspace = true;
        warncolorspaceText = "GRAY";
        break;
    default:
        warncolorspaceText = tr("unknown");
    }
    if (warnColorspace) {
        QMessageBox question;
        question.setWindowTitle(tr("Add layer to %1 project?").arg(warncolorspaceText));
        question.setIconPixmap(QIcon::fromTheme("colors").pixmap(QSize(32, 32)));
        question.setText(tr("Working in %1 color space with multiple layers are not recommended. Add new layer anyway?").arg(warncolorspaceText));
        question.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        question.setDefaultButton(QMessageBox::No);
        int reply = question.exec();
        if (reply != QMessageBox::Yes) { QDialog::reject(); return; }
    }
    int depth = _bitBox->currentData().toInt();
    if (depth > CyanImageFormat::supportedQuantumDepth()) {
        qWarning() << "requested depth higher than supported by IM!";
        depth = CyanImageFormat::supportedQuantumDepth();
    }
    /*createImage(QSize(_width->value(),
                      _height->value()),
                type,
                depth);

    QDialog::accept();*/
    this->setEnabled(false);
    _progbar->setHidden(false);
    QtConcurrent::run(this,
                      &NewMediaDialog::createImage,
                      QSize(_width->value(), _height->value()),
                      type,
                      depth,
                      _label->text(),
                      _drawOption->currentData().toInt());
}

void NewMediaDialog::handleCancel()
{
    QDialog::reject();
}

void NewMediaDialog::createImage(QSize geo,
                                 Magick::ColorspaceType colorspace,
                                 size_t depth,
                                 QString label,
                                 int option)
{
    Magick::Geometry mGeo = Magick::Geometry(static_cast<size_t>(geo.width()),
                                             static_cast<size_t>(geo.height()));
    qsrand(QDateTime::currentMSecsSinceEpoch() / 1000);
    Magick::SetRandomSeed(qrand());
    try {
        _image = Magick::Image(mGeo, Magick::Color("rgba(0,0,0,0)"));
        //_image.colorSpace(Magick::sRGBColorspace); // we start off as RGB then convert later
        _image.depth(depth);
        _image.alpha(true);
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) {
        qDebug() << warn_.what();
    }
    qDebug() << "ADD TEXTURE";
    try {
        switch (option) {
        case CyanDrawTextureSolid:
            _image = Magick::Image(mGeo, Magick::Color(_colorPickerFrom->currentColor()
                                                       .name().toStdString()));
            break;
        case CyanDrawTextureGradient:
            _image.size(mGeo);
            _image.read("gradient:");
            break;
        case CyanDrawTextureGradientColor:
            _image.size(mGeo);
            _image.read(QString("gradient:%1-%2")
                        .arg(_colorPickerFrom->currentColor().name())
                        .arg(_colorPickerTo->currentColor().name())
                        .toStdString());
            break;
        case CyanDrawTextureRadial:
            _image.size(mGeo);
            _image.read("radial-gradient:");
            break;
        case CyanDrawTextureRadialColor:
            _image.size(mGeo);
            _image.read(QString("radial-gradient:%1-%2")
                        .arg(_colorPickerFrom->currentColor().name())
                        .arg(_colorPickerTo->currentColor().name())
                        .toStdString());
            break;
        case CyanDrawTexturePlasma:
            _image.size(mGeo);
            _image.read("plasma:");
            break;
        case CyanDrawTexturePlasmaFractal:
            _image.size(mGeo);
            _image.read("plasma:fractal");
            break;
        case CyanDrawTexturePlasmaColor:
            _image.size(mGeo);
            _image.read(QString("Plasma:%1-%2")
                        .arg(_colorPickerFrom->currentColor().name())
                        .arg(_colorPickerTo->currentColor().name())
                        .toStdString());
            break;
        case CyanDrawTextureGaussianNoise:
        case CyanDrawTextureLoopsGaussian:
            _image = Magick::Image(mGeo, Magick::Color("rgba(0,0,0,0)"));
            _image.size(mGeo);
            _image.addNoise(Magick::GaussianNoise);
            break;
        case CyanDrawTextureImpulseNoise:
        case CyanDrawTextureLoopsImpulse:
            _image = Magick::Image(mGeo, Magick::Color("rgba(0,0,0,0)"));
            _image.size(mGeo);
            _image.addNoise(Magick::ImpulseNoise);
            break;
        case CyanDrawTextureLaplacianNoise:
        case CyanDrawTextureLoopsLaplacian:
            _image = Magick::Image(mGeo, Magick::Color("rgba(0,0,0,0)"));
            _image.size(mGeo);
            _image.addNoise(Magick::LaplacianNoise);
            break;
        default:
            _image.size(mGeo);
            _image.evaluate(Magick::AlphaChannel,
                            Magick::MultiplyEvaluateOperator,
                            0.0);
            break;
        }

        if (option == CyanDrawTextureLoopsGaussian ||
            option == CyanDrawTextureLoopsImpulse ||
            option == CyanDrawTextureLoopsLaplacian)
        { // fancy and slow "loops" effect
            qDebug() << "FANCY";
            _image.alpha(false);
            _image.blur(0,10);
            _image.normalize();
            _image.fx("sin(u*4*pi)*100");
            _image.edge(1);
            _image.blur(0,10);
            _image.alpha(true);
        }

        if (colorspace != _image.colorSpace()) {
            qDebug() << "NEED TO CONVERT COLORSPACE!" << colorspace << "vs" << _image.colorSpace();
            QString defaultProfile;
            QSettings settings;
            settings.beginGroup(QString("color"));
            switch(_image.colorSpace()) {
            case Magick::CMYKColorspace:
                defaultProfile = settings.value(QString("cmyk_profile")).toString();
                break;
            case Magick::GRAYColorspace:
                defaultProfile = settings.value(QString("gray_profile")).toString();
                break;
            default:
                defaultProfile = settings.value(QString("rgb_profile")).toString();
            }
            settings.endGroup();
            _image = ColorConvert::convertColorspace(_image, defaultProfile, selectedProfile());
            _image.profile("ICC", selectedProfile()); // probably remove
        } else {
            _image.profile("ICC", selectedProfile());
        }
        if (_image.depth() != depth) {
            _image.depth(depth);
        }
        _image.label(label.toStdString());
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) {
        qDebug() << warn_.what();
    }

    emit createImageFinished();
}

void NewMediaDialog::populateProfiles(Magick::ColorspaceType colorspace)
{
    _profile->clear();

    QSettings settings;
    settings.beginGroup(QString("color"));
    QString filename;

    switch(colorspace) {
    case Magick::CMYKColorspace:
        filename = settings.value(QString("cmyk_profile")).toString();
        break;
    case Magick::GRAYColorspace:
        filename = settings.value(QString("gray_profile")).toString();
        break;
    default:
        filename = settings.value(QString("rgb_profile")).toString();
    }
    settings.endGroup();

    QIcon colorWheelIcon = QIcon::fromTheme("color-wheel");
    QMapIterator<QString, QString> i(CyanCommon::getColorProfiles(colorspace));
    while (i.hasNext()) {
        i.next();
        _profile->addItem(colorWheelIcon,
                          i.key(),
                          i.value());
    }
    int index = _profile->findData(filename);
    if (index>=0) { _profile->setCurrentIndex(index); }
}

void NewMediaDialog::handleColorspaceChanged(int index)
{
    Q_UNUSED(index)
    Magick::ColorspaceType type = _colorspace;
    switch (_select->currentData().toInt()) {
    case 0:
        type = Magick::sRGBColorspace;
        break;
    case 1:
        type = Magick::CMYKColorspace;
        break;
    case 2:
        type = Magick::GRAYColorspace;
        break;
    }
    populateProfiles(type);
}

Magick::Blob NewMediaDialog::selectedProfile()
{
    QString filename = _profile->currentData().toString();
    if (_forcedProfile.length()>0) {
        qDebug() << "using same profile as canvas";
        return _forcedProfile;
    }
    qDebug() << "selected profile" << filename;
    if (!filename.isEmpty()) {
        try {
            Magick::Image image;
            image.read(filename.toStdString());
            Magick::Blob profile;
            image.write(&profile);
            return profile;
        }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
        catch(Magick::Warning &warn_ ) {
            qDebug() << warn_.what();
        }
    }
    return Magick::Blob();
}

void NewMediaDialog::handleCreatedImage()
{
    QDialog::accept();
}
