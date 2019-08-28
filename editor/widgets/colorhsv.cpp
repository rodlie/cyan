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

#include "colorhsv.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

ColorHSV::ColorHSV(QWidget *parent) : QWidget(parent)
  , spinH(nullptr)
  , spinS(nullptr)
  , spinV(nullptr)
  , slideH(nullptr)
  , slideS(nullptr)
  , slideV(nullptr)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(0);

    QWidget *hWidget = new QWidget(this);
    QHBoxLayout *hLayout = new QHBoxLayout(hWidget);
    QWidget *sWidget = new QWidget(this);
    QHBoxLayout *sLayout = new QHBoxLayout(sWidget);
    QWidget *vWidget = new QWidget(this);
    QHBoxLayout *vLayout = new QHBoxLayout(vWidget);

    hLayout->setContentsMargins(0, 0, 0, 0);
    sLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setContentsMargins(0, 0, 0, 0);

    slideH = new QSlider(this);
    slideS = new QSlider(this);
    slideV = new QSlider(this);

    slideH->setRange(0, 1000);
    slideS->setRange(0, 1000);
    slideV->setRange(0, 1000);

    slideH->setOrientation(Qt::Horizontal);
    slideS->setOrientation(Qt::Horizontal);
    slideV->setOrientation(Qt::Horizontal);

    slideH->setSizePolicy(QSizePolicy::Expanding,
                          QSizePolicy::Fixed);
    slideS->setSizePolicy(QSizePolicy::Expanding,
                          QSizePolicy::Fixed);
    slideV->setSizePolicy(QSizePolicy::Expanding,
                          QSizePolicy::Fixed);

    spinH = new QDoubleSpinBox(this);
    spinS = new QDoubleSpinBox(this);
    spinV = new QDoubleSpinBox(this);

    spinH->setDecimals(3);
    spinS->setDecimals(3);
    spinV->setDecimals(3);

    spinH->setSingleStep(0.01);
    spinS->setSingleStep(0.01);
    spinV->setSingleStep(0.01);

    spinH->setRange(0,1);
    spinS->setRange(0,1);
    spinV->setRange(0,1);

    spinH->setSizePolicy(QSizePolicy::Minimum,
                         QSizePolicy::Fixed);
    spinS->setSizePolicy(QSizePolicy::Minimum,
                         QSizePolicy::Fixed);
    spinV->setSizePolicy(QSizePolicy::Minimum,
                         QSizePolicy::Fixed);

    spinH->setMinimumWidth(70);
    spinH->setMaximumWidth(70);
    spinS->setMinimumWidth(70);
    spinS->setMaximumWidth(70);
    spinV->setMinimumWidth(70);
    spinV->setMaximumWidth(70);

    QLabel *labelH = new QLabel(this);
    QLabel *labelS = new QLabel(this);
    QLabel *labelV = new QLabel(this);

    labelH->setText("H");
    labelS->setText("S");
    labelV->setText("V");

    labelH->setMinimumWidth(10);
    labelH->setMaximumWidth(10);
    labelS->setMinimumWidth(10);
    labelS->setMaximumWidth(10);
    labelV->setMinimumWidth(10);
    labelV->setMaximumWidth(10);

    hLayout->addWidget(labelH);
    hLayout->addWidget(slideH);
    hLayout->addWidget(spinH);
    sLayout->addWidget(labelS);
    sLayout->addWidget(slideS);
    sLayout->addWidget(spinS);
    vLayout->addWidget(labelV);
    vLayout->addWidget(slideV);
    vLayout->addWidget(spinV);

    mainLayout->addWidget(hWidget);
    mainLayout->addWidget(sWidget);
    mainLayout->addWidget(vWidget);

    //mainLayout->addStretch();

    connect(spinH, SIGNAL(valueChanged(double)),
            this, SLOT(handleColorHChanged(double)));
    connect(spinS, SIGNAL(valueChanged(double)),
            this, SLOT(handleColorSChanged(double)));
    connect(spinV, SIGNAL(valueChanged(double)),
            this, SLOT(handleColorVChanged(double)));

    connect(slideH, SIGNAL(sliderMoved(int)),
            this, SLOT(handleSliderHMoved(int)));
    connect(slideS, SIGNAL(sliderMoved(int)),
            this, SLOT(handleSliderSMoved(int)));
    connect(slideV, SIGNAL(sliderMoved(int)),
            this, SLOT(handleSliderVMoved(int)));
}

const QColor ColorHSV::getColor()
{
    QColor color;
    color.setHsvF(spinH->value(),
                  spinS->value(),
                  spinV->value());
    return color;
}

void ColorHSV::setColor(const QColor &color)
{
    setH(color.hsvHueF());
    setS(color.hsvSaturationF());
    setV(color.valueF());
}

void ColorHSV::setH(qreal h)
{
    spinH->blockSignals(true);
    slideH->blockSignals(true);
    spinH->setValue(h);
    int slideValue = qRound(h*1000);
    slideH->setValue(slideValue);
    spinH->blockSignals(false);
    slideH->blockSignals(false);
}

void ColorHSV::setS(qreal s)
{
    spinS->blockSignals(true);
    slideS->blockSignals(true);
    spinS->setValue(s);
    int slideValue = qRound(s*1000);
    slideS->setValue(slideValue);
    spinS->blockSignals(false);
    slideS->blockSignals(false);
}

void ColorHSV::setV(qreal v)
{
    spinV->blockSignals(true);
    slideV->blockSignals(true);
    spinV->setValue(v);
    int slideValue = qRound(v*1000);
    slideV->setValue(slideValue);
    spinV->blockSignals(false);
    slideV->blockSignals(false);
}

void ColorHSV::handleColorHChanged(double value)
{
    QColor color;
    color.setHsvF(value,
                  spinS->value(),
                  spinV->value());
    emit colorChanged(color);
}

void ColorHSV::handleColorSChanged(double value)
{
    QColor color;
    color.setHsvF(spinH->value(),
                  value,
                  spinV->value());
    emit colorChanged(color);
}

void ColorHSV::handleColorVChanged(double value)
{
    QColor color;
    color.setHsvF(spinH->value(),
                  spinS->value(),
                  value);
    emit colorChanged(color);
}

void ColorHSV::handleSliderHMoved(int value)
{
    double newValue = static_cast<qreal>(value)/1000;
    spinH->setValue(newValue);
}

void ColorHSV::handleSliderSMoved(int value)
{
    double newValue = static_cast<qreal>(value)/1000;
    spinS->setValue(newValue);
}

void ColorHSV::handleSliderVMoved(int value)
{
    double newValue = static_cast<qreal>(value)/1000;
    spinV->setValue(newValue);
}
