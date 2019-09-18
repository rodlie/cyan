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

#include "CyanImageFormat.h"

NewMediaDialog::NewMediaDialog(QWidget *parent,
                               QString title,
                               CyanCommon::newDialogType dialogType,
                               Magick::ColorspaceType colorspace,
                               Magick::Blob profile,
                               QSize size) :
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
  , _depth8(nullptr)
  , _depth16(nullptr)
  , _forcedProfile(profile)
{
    setWindowTitle(title);
    setWindowIcon(_type==CyanCommon::newImageDialogType?QIcon::fromTheme("document-new"):QIcon::fromTheme("layer"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    _label = new QLineEdit(this);
    _width  = new QSpinBox(this);
    _height = new QSpinBox(this);
    _ok = new QPushButton(this);
    _cancel = new QPushButton(this);
    _select = new QComboBox(this);
    _profile = new QComboBox(this);
    _depth8 = new QRadioButton(this);
    _depth16 = new QRadioButton(this);
    _depth32 = new QRadioButton(this);
    _depth64 = new QRadioButton(this);

    _ok->setIcon(QIcon::fromTheme("document-new"));
    _cancel->setIcon(QIcon::fromTheme("process-stop"));

    QWidget *buttonWidget = new QWidget(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);

    QWidget *sizeWidget = new QWidget(this);
    QHBoxLayout *sizeLayout = new QHBoxLayout(sizeWidget);

    QWidget *depthWidget = new QWidget(this);
    QVBoxLayout *depthLayout = new QVBoxLayout(depthWidget);

    _width->setRange(1, 10000);
    _height->setRange(1, 10000);

    if (size.width()>0 && size.height()>0) {
        _width->setValue(size.width());
        _height->setValue(size.height());
    } else {
        _width->setValue(1024);
        _height->setValue(1024);
    }

    _depth8->setText(tr("8-bit"));
    _depth16->setText(tr("16-bit"));
    _depth32->setText(tr("32-bit"));
    _depth64->setText(tr("64-bit"));

    _label->setPlaceholderText(title);

    _ok->setText(tr("New"));
    _cancel->setText(tr("Cancel"));

    QIcon colorsIcon = QIcon::fromTheme("colors");

    _select->addItem(colorsIcon, tr("RGB"), 0);
    _select->addItem(colorsIcon, tr("CMYK"), 1);
    _select->addItem(colorsIcon, tr("GRAY"), 2);

    sizeLayout->addWidget(_width);
    sizeLayout->addWidget(_height);

    depthLayout->addWidget(_depth8);
    depthLayout->addWidget(_depth16);
    depthLayout->addWidget(_depth32);
    depthLayout->addWidget(_depth64);

    _depth8->setDisabled(true);
    _depth16->setDisabled(true);
    _depth32->setDisabled(true);
    _depth64->setDisabled(true);

    int depth = CyanImageFormat::supportedQuantumDepth();
    qDebug() << "DEPTH?" << depth;
    if (depth >= 8) {
        _depth8->setDisabled(false);
        _depth8->setChecked(true);
    }
    if (depth >= 16) {
        _depth16->setDisabled(false);
    }
    if (depth >= 32) {
        _depth32->setDisabled(false);
    }
    if (depth == 64) {
        _depth64->setDisabled(false);
    }

    buttonLayout->addWidget(_ok);
    buttonLayout->addWidget(_cancel);

    mainLayout->addWidget(_label);
    mainLayout->addWidget(sizeWidget);
    mainLayout->addWidget(_select);
    mainLayout->addWidget(_profile);
    mainLayout->addWidget(depthWidget);
    mainLayout->addWidget(buttonWidget);
    mainLayout->addStretch();

    if (_type == CyanCommon::newLayerDialogType) {
        _select->hide();
        _profile->hide();
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
    int depth = 0;
    if (_depth8->isChecked()) {
        depth = 8;
    } else if (_depth16->isChecked()) {
        depth = 16;
    } else if (_depth32->isChecked()) {
        depth = 32;
    } else if (_depth64->isChecked()) {
        depth = 64;
    }
    if (depth > CyanImageFormat::supportedQuantumDepth()) {
        qWarning() << "requested depth higher than supported by IM!";
        depth = CyanImageFormat::supportedQuantumDepth();
    }
    createImage(QSize(_width->value(),
                      _height->value()),
                type,
                depth);

    QDialog::accept();
}

void NewMediaDialog::handleCancel()
{
    QDialog::reject();
}

void NewMediaDialog::createImage(QSize geo,
                                 Magick::ColorspaceType colorspace,
                                 size_t depth)
{
    QString label = _label->text();
    if (label.isEmpty()) { label = windowTitle(); }
    try {
        _image.size(Magick::Geometry(static_cast<size_t>(geo.width()),
                                     static_cast<size_t>(geo.height())));
        _image.colorSpace(colorspace);
        _image.depth(depth);
        _image.label(label.toStdString());
        _image.alpha(true);
        _image.evaluate(Magick::AlphaChannel,
                        Magick::MultiplyEvaluateOperator,
                        0.0);
        _image.profile("ICC",
                       selectedProfile());
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) {
        qDebug() << warn_.what();
    }
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
