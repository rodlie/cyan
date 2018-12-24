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

#include "newmediadialog.h"

#include <QIcon>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMapIterator>
#include <QDebug>
#include <QSettings>

NewMediaDialog::NewMediaDialog(QWidget *parent,
                               QString title,
                               Common::newDialogType dialogType,
                               Magick::ColorspaceType colorspace,
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
{
    setWindowTitle(title);
    setWindowIcon(_type==Common::newImageDialogType?QIcon(":/icons/image.png"):QIcon(":/icons/layer.png"));

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

    QWidget *buttonWidget = new QWidget(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);

    QWidget *sizeWidget = new QWidget(this);
    QHBoxLayout *sizeLayout = new QHBoxLayout(sizeWidget);

    QWidget *depthWidget = new QWidget(this);
    QHBoxLayout *depthLayout = new QHBoxLayout(depthWidget);

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
    _depth8->setChecked(true);
    _depth16->setText(tr("16-bit"));

    _label->setPlaceholderText(title);

    _ok->setText(tr("New"));
    _cancel->setText(tr("Cancel"));

    _select->addItem(tr("RGB"), 0);
    _select->addItem(tr("CMYK"), 1);
    _select->addItem(tr("GRAY"), 2);

    sizeLayout->addWidget(_width);
    sizeLayout->addWidget(_height);

    depthLayout->addWidget(_depth8);
    depthLayout->addWidget(_depth16);

    buttonLayout->addWidget(_ok);
    buttonLayout->addWidget(_cancel);

    mainLayout->addWidget(_label);
    mainLayout->addWidget(sizeWidget);
    mainLayout->addWidget(_select);
    mainLayout->addWidget(_profile);
    mainLayout->addWidget(depthWidget);
    mainLayout->addWidget(buttonWidget);
    mainLayout->addStretch();

    if (_type == Common::newLayerDialogType) {
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
    if (_type == Common::newImageDialogType) {
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
    createImage(QSize(_width->value(), _height->value()),
                type,
                _depth8->isChecked()?8:16);

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
        _image.matte(true);
        _image.quantumOperator(Magick::AlphaChannel,
                               Magick::MultiplyEvaluateOperator,
                               0.0);
        _image.profile("ICC", selectedProfile());
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
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

    QMapIterator<QString, QString> i(Common::getColorProfiles(colorspace));
    while (i.hasNext()) {
        i.next();
        _profile->addItem(i.key(),
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
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
    }
    return Magick::Blob();
}
