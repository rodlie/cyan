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

#include "colorrgb.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

ColorRGB::ColorRGB(QWidget *parent) : QWidget(parent)
  , spinR(nullptr)
  , spinG(nullptr)
  , spinB(nullptr)
  , slideR(nullptr)
  , slideG(nullptr)
  , slideB(nullptr)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 0);
    mainLayout->setSpacing(0);

    QWidget *rWidget = new QWidget(this);
    QHBoxLayout *rLayout = new QHBoxLayout(rWidget);
    QWidget *gWidget = new QWidget(this);
    QHBoxLayout *gLayout = new QHBoxLayout(gWidget);
    QWidget *bWidget = new QWidget(this);
    QHBoxLayout *bLayout = new QHBoxLayout(bWidget);

    rLayout->setContentsMargins(0, 0, 0, 0);
    gLayout->setContentsMargins(0, 0, 0, 0);
    bLayout->setContentsMargins(0, 0, 0, 0);

    slideR = new QSlider(this);
    slideG = new QSlider(this);
    slideB = new QSlider(this);

    slideR->setRange(0,255);
    slideG->setRange(0,255);
    slideB->setRange(0,255);

    slideR->setOrientation(Qt::Horizontal);
    slideG->setOrientation(Qt::Horizontal);
    slideB->setOrientation(Qt::Horizontal);

    slideR->setSizePolicy(QSizePolicy::Expanding,
                          QSizePolicy::Fixed);
    slideG->setSizePolicy(QSizePolicy::Expanding,
                          QSizePolicy::Fixed);
    slideB->setSizePolicy(QSizePolicy::Expanding,
                          QSizePolicy::Fixed);

    spinR = new QSpinBox(this);
    spinG = new QSpinBox(this);
    spinB = new QSpinBox(this);

    spinR->setRange(0, 255);
    spinG->setRange(0, 255);
    spinB->setRange(0, 255);

    spinR->setSizePolicy(QSizePolicy::Minimum,
                         QSizePolicy::Fixed);
    spinG->setSizePolicy(QSizePolicy::Minimum,
                         QSizePolicy::Fixed);
    spinB->setSizePolicy(QSizePolicy::Minimum,
                         QSizePolicy::Fixed);

    spinR->setMinimumWidth(70);
    spinR->setMaximumWidth(70);
    spinG->setMinimumWidth(70);
    spinG->setMaximumWidth(70);
    spinB->setMinimumWidth(70);
    spinB->setMaximumWidth(70);

    QLabel *labelR = new QLabel(this);
    QLabel *labelG = new QLabel(this);
    QLabel *labelB = new QLabel(this);

    labelR->setText("R");
    labelG->setText("G");
    labelB->setText("B");

    labelR->setMinimumWidth(10);
    labelR->setMaximumWidth(10);
    labelG->setMinimumWidth(10);
    labelG->setMaximumWidth(10);
    labelB->setMinimumWidth(10);
    labelB->setMaximumWidth(10);

    rLayout->addWidget(labelR);
    rLayout->addWidget(slideR);
    rLayout->addWidget(spinR);
    gLayout->addWidget(labelG);
    gLayout->addWidget(slideG);
    gLayout->addWidget(spinG);
    bLayout->addWidget(labelB);
    bLayout->addWidget(slideB);
    bLayout->addWidget(spinB);

    mainLayout->addWidget(rWidget);
    mainLayout->addWidget(gWidget);
    mainLayout->addWidget(bWidget);

    mainLayout->addStretch();

    connect(spinR, SIGNAL(valueChanged(int)),
            this, SLOT(handleColorRChanged(int)));
    connect(spinG, SIGNAL(valueChanged(int)),
            this, SLOT(handleColorGChanged(int)));
    connect(spinB, SIGNAL(valueChanged(int)),
            this, SLOT(handleColorBChanged(int)));

    connect(slideR, SIGNAL(sliderMoved(int)),
            spinR, SLOT(setValue(int)));
    connect(slideG, SIGNAL(sliderMoved(int)),
            spinG, SLOT(setValue(int)));
    connect(slideB, SIGNAL(sliderMoved(int)),
            spinB, SLOT(setValue(int)));
}

const QColor ColorRGB::getColor()
{
    QColor color;
    color.setRed(spinR->value());
    color.setGreen(spinG->value());
    color.setBlue(spinB->value());
    return color;
}

void ColorRGB::setColor(const QColor &color)
{
    setR(color.red());
    setG(color.green());
    setB(color.blue());
}

void ColorRGB::setR(int r)
{
    spinR->blockSignals(true);
    slideR->blockSignals(true);
    if (spinR->value() != r) { spinR->setValue(r); }
    if (slideR->value() != r) { slideR->setValue(r); }
    spinR->blockSignals(false);
    slideR->blockSignals(false);
}

void ColorRGB::setG(int g)
{
    spinG->blockSignals(true);
    slideG->blockSignals(true);
    if (spinG->value() != g) { spinG->setValue(g); }
    if (slideG->value() != g) { slideG->setValue(g); }
    spinG->blockSignals(false);
    slideG->blockSignals(false);
}

void ColorRGB::setB(int b)
{
    spinB->blockSignals(true);
    slideB->blockSignals(true);
    if (spinB->value() != b) { spinB->setValue(b); }
    if (slideB->value() != b) { slideB->setValue(b); }
    spinB->blockSignals(false);
    slideB->blockSignals(false);
}

void ColorRGB::handleColorRChanged(int value)
{
    setR(value);
    QColor color;
    color.setRgb(value,
                 spinG->value(),
                 spinB->value());
    emit colorChanged(color);
}

void ColorRGB::handleColorGChanged(int value)
{
    setG(value);
    QColor color;
    color.setRgb(spinR->value(),
                 value,
                 spinB->value());
    emit colorChanged(color);
}

void ColorRGB::handleColorBChanged(int value)
{
    setB(value);
    QColor color;
    color.setRgb(spinR->value(),
                 spinG->value(),
                 value);
    emit colorChanged(color);
}
