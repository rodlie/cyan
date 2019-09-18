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

#include "CyanFFmpegOpenDialog.h"
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>

#ifdef WITH_FFMPEG
#include "CyanFFmpeg.h"
#endif

videoDialog::videoDialog(QWidget *parent,
                         int max,
                         const QString &filename,
                         bool showToAndFrom) :
    QDialog (parent)
  , _max(max)
  , _filename(filename)
  , _label(nullptr)
  , _slider(nullptr)
  , _spin(nullptr)
  , _ok(nullptr)
  , _cancel(nullptr)
  , _fromMark(nullptr)
  , _toMark(nullptr)
  , _from(nullptr)
  , _to(nullptr)
{
    setWindowTitle(tr("Import video frame(s)"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    _label = new QLabel(this);
    _label->setMinimumSize(QSize(480,360));
    _label->setMaximumSize(QSize(480,360));
    _label->setStyleSheet(QString("background-color: black;"));

    _slider = new QSlider(this);
    _slider->setRange(0, _max);
    _slider->setOrientation(Qt::Horizontal);

    _spin = new QSpinBox(this);
    _spin->setRange(0, _max);

    _fromMark = new QPushButton(this);
    _fromMark->setText(tr("Mark"));

    _toMark = new QPushButton(this);
    _toMark->setText(tr("Mark"));

    _from = new QSpinBox(this);
    _from->setRange(0, _max);

    _to = new QSpinBox(this);
    _to->setRange(0, _max);

    QWidget *timeline = new QWidget(this);
    QHBoxLayout *timelineLayout = new QHBoxLayout(timeline);

    QWidget *layersWidget = new QWidget(this);
    QHBoxLayout *layerLayout = new QHBoxLayout(layersWidget);


    QWidget *buttonWidget = new QWidget(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);

    _ok = new QPushButton(this);
    _ok->setText(tr("Import"));

    _cancel = new QPushButton(this);
    _cancel->setText(tr("Cancel"));

    timelineLayout->addWidget(_slider);
    timelineLayout->addWidget(_spin);

    layerLayout->addWidget(_fromMark);
    layerLayout->addWidget(_from);
    layerLayout->addWidget(_toMark);
    layerLayout->addWidget(_to);

    buttonLayout->addStretch();
    buttonLayout->addWidget(_ok);
    buttonLayout->addWidget(_cancel);

    mainLayout->addWidget(_label);
    mainLayout->addWidget(timeline);
    mainLayout->addWidget(layersWidget);
    mainLayout->addStretch();
    mainLayout->addWidget(buttonWidget);

    if (!showToAndFrom) {
        layersWidget->hide();
        _fromMark->setDisabled(true);
        _toMark->setDisabled(true);
        _from->setDisabled(true);
        _to->setDisabled(true);
    }

    connect(_slider,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(handleSlider(int)));
    connect(_spin,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(handleSpin(int)));
    connect(_ok,
            SIGNAL(released()),
            this,
            SLOT(handleOk()));
    connect(_cancel,
            SIGNAL(released()),
            this,
            SLOT(handleCancel()));
    connect(_fromMark,
            SIGNAL(released()),
            this,
            SLOT(handleFromMark()));
    connect(_toMark,
            SIGNAL(released()),
            this,
            SLOT(handleToMark()));

    _slider->setValue(_max/2);
}

int videoDialog::getFrame()
{
    return _slider->value();
}

QSize videoDialog::getFrames()
{
    return QSize(_from->value(),
                 _to->value());
}

void videoDialog::handleSlider(int pos)
{
    if (_spin->value()!=pos) {
        _spin->blockSignals(true);
        _spin->setValue(pos);
        _spin->blockSignals(false);
    }
#ifdef WITH_FFMPEG
    try {
        Magick::Image image = CyanFFmpeg::getVideoFrame(_filename,
                                                    pos);
        if (image.isValid()) {
            Magick::Blob buffer;
            image.magick("BMP");
            image.scale(Magick::Geometry(480, 360));
            image.write(&buffer);
            qDebug() << "set pixmap!";
            _label->setPixmap(QPixmap::fromImage(QImage::fromData((uchar*)buffer.data(),
                                                                  buffer.length())));
        }
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) {
        qDebug() << warn_.what();
    }
#endif
}

void videoDialog::handleSpin(int pos)
{
    if (_slider->value()!=pos) { _slider->setValue(pos); }
}

void videoDialog::handleOk()
{
    QDialog::accept();
}

void videoDialog::handleCancel()
{
    QDialog::reject();
}

void videoDialog::handleFromMark()
{
    _from->setValue(_spin->value());
}

void videoDialog::handleToMark()
{
    _to->setValue(_spin->value());
}

