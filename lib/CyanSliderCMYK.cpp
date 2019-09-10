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

#include "CyanSliderCMYK.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

CyanSliderCMYK::CyanSliderCMYK(QWidget *parent) :
    QWidget(parent)
  , spinC(nullptr)
  , spinM(nullptr)
  , spinY(nullptr)
  , spinK(nullptr)
  , slideC(nullptr)
  , slideM(nullptr)
  , slideY(nullptr)
  , slideK(nullptr)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 0);
    mainLayout->setSpacing(0);

    QWidget *cWidget = new QWidget(this);
    QHBoxLayout *cLayout = new QHBoxLayout(cWidget);
    QWidget *mWidget = new QWidget(this);
    QHBoxLayout *mLayout = new QHBoxLayout(mWidget);
    QWidget *yWidget = new QWidget(this);
    QHBoxLayout *yLayout = new QHBoxLayout(yWidget);
    QWidget *kWidget = new QWidget(this);
    QHBoxLayout *kLayout = new QHBoxLayout(kWidget);

    cLayout->setContentsMargins(0, 0, 0, 0);
    mLayout->setContentsMargins(0, 0, 0, 0);
    yLayout->setContentsMargins(0, 0, 0, 0);
    kLayout->setContentsMargins(0, 0, 0, 0);

    slideC = new QSlider(this);
    slideM = new QSlider(this);
    slideY = new QSlider(this);
    slideK = new QSlider(this);

    slideC->setRange(0, 255);
    slideM->setRange(0, 255);
    slideY->setRange(0, 255);
    slideK->setRange(0, 255);

    slideC->setOrientation(Qt::Horizontal);
    slideM->setOrientation(Qt::Horizontal);
    slideY->setOrientation(Qt::Horizontal);
    slideK->setOrientation(Qt::Horizontal);

    slideC->setSizePolicy(QSizePolicy::Expanding,
                          QSizePolicy::Fixed);
    slideM->setSizePolicy(QSizePolicy::Expanding,
                          QSizePolicy::Fixed);
    slideY->setSizePolicy(QSizePolicy::Expanding,
                          QSizePolicy::Fixed);
    slideK->setSizePolicy(QSizePolicy::Expanding,
                          QSizePolicy::Fixed);

    spinC = new QSpinBox(this);
    spinM = new QSpinBox(this);
    spinY = new QSpinBox(this);
    spinK = new QSpinBox(this);

    spinC->setRange(0, 255);
    spinM->setRange(0, 255);
    spinY->setRange(0, 255);
    spinK->setRange(0, 255);

    spinC->setSizePolicy(QSizePolicy::Minimum,
                         QSizePolicy::Fixed);
    spinM->setSizePolicy(QSizePolicy::Minimum,
                         QSizePolicy::Fixed);
    spinY->setSizePolicy(QSizePolicy::Minimum,
                         QSizePolicy::Fixed);
    spinK->setSizePolicy(QSizePolicy::Minimum,
                         QSizePolicy::Fixed);

    spinC->setMinimumWidth(70);
    spinC->setMaximumWidth(70);
    spinM->setMinimumWidth(70);
    spinM->setMaximumWidth(70);
    spinY->setMinimumWidth(70);
    spinY->setMaximumWidth(70);
    spinK->setMinimumWidth(70);
    spinK->setMaximumWidth(70);

    QLabel *labelC = new QLabel(this);
    QLabel *labelM = new QLabel(this);
    QLabel *labelY = new QLabel(this);
    QLabel *labelK = new QLabel(this);

    labelC->setText("C");
    labelM->setText("M");
    labelY->setText("Y");
    labelK->setText("K");

    labelC->setMinimumWidth(10);
    labelC->setMaximumWidth(10);
    labelM->setMinimumWidth(10);
    labelM->setMaximumWidth(10);
    labelY->setMinimumWidth(10);
    labelY->setMaximumWidth(10);
    labelK->setMinimumWidth(10);
    labelK->setMaximumWidth(10);

    cLayout->addWidget(labelC);
    cLayout->addWidget(slideC);
    cLayout->addWidget(spinC);
    mLayout->addWidget(labelM);
    mLayout->addWidget(slideM);
    mLayout->addWidget(spinM);
    yLayout->addWidget(labelY);
    yLayout->addWidget(slideY);
    yLayout->addWidget(spinY);
    kLayout->addWidget(labelK);
    kLayout->addWidget(slideK);
    kLayout->addWidget(spinK);

    mainLayout->addWidget(cWidget);
    mainLayout->addWidget(mWidget);
    mainLayout->addWidget(yWidget);
    mainLayout->addWidget(kWidget);

    mainLayout->addStretch();

    connect(spinC, SIGNAL(valueChanged(int)),
            this, SLOT(handleColorCChanged(int)));
    connect(spinM, SIGNAL(valueChanged(int)),
            this, SLOT(handleColorMChanged(int)));
    connect(spinY, SIGNAL(valueChanged(int)),
            this, SLOT(handleColorYChanged(int)));
    connect(spinK, SIGNAL(valueChanged(int)),
            this, SLOT(handleColorKChanged(int)));

    connect(slideC, SIGNAL(sliderMoved(int)),
            spinC, SLOT(setValue(int)));
    connect(slideM, SIGNAL(sliderMoved(int)),
            spinM, SLOT(setValue(int)));
    connect(slideY, SIGNAL(sliderMoved(int)),
            spinY, SLOT(setValue(int)));
    connect(slideK, SIGNAL(sliderMoved(int)),
            spinK, SLOT(setValue(int)));
}

const QColor CyanSliderCMYK::getColor()
{
    QColor color;
    color.setCmyk(spinC->value(),
                  spinM->value(),
                  spinY->value(),
                  spinK->value());
    return color;
}

void CyanSliderCMYK::setColor(const QColor &color)
{
    setC(color.cyan());
    setM(color.magenta());
    setY(color.yellow());
    setK(color.black());
}

void CyanSliderCMYK::setC(int c)
{
    spinC->blockSignals(true);
    slideC->blockSignals(true);
    if (spinC->value() != c) { spinC->setValue(c); }
    if (slideC->value() != c) { slideC->setValue(c); }
    spinC->blockSignals(false);
    slideC->blockSignals(false);
}

void CyanSliderCMYK::setM(int m)
{
    spinM->blockSignals(true);
    slideM->blockSignals(true);
    if (spinM->value() != m) { spinM->setValue(m); }
    if (slideM->value() != m) { slideM->setValue(m); }
    spinM->blockSignals(false);
    slideM->blockSignals(false);
}

void CyanSliderCMYK::setY(int y)
{
    spinY->blockSignals(true);
    slideY->blockSignals(true);
    if (spinY->value() != y) { spinY->setValue(y); }
    if (slideY->value() != y) { slideY->setValue(y); }
    spinY->blockSignals(false);
    slideY->blockSignals(false);
}

void CyanSliderCMYK::setK(int k)
{
    spinK->blockSignals(true);
    slideK->blockSignals(true);
    if (spinK->value() != k) { spinK->setValue(k); }
    if (slideK->value() != k) { slideK->setValue(k); }
    spinK->blockSignals(false);
    slideK->blockSignals(false);
}

void CyanSliderCMYK::handleColorCChanged(int value)
{
    setC(value);
    QColor color;
    color.setCmyk(value,
                  spinM->value(),
                  spinY->value(),
                  spinK->value());
    emit colorChanged(color);
}

void CyanSliderCMYK::handleColorMChanged(int value)
{
    setM(value);
    QColor color;
    color.setCmyk(spinC->value(),
                  value,
                  spinY->value(),
                  spinK->value());
    emit colorChanged(color);
}

void CyanSliderCMYK::handleColorYChanged(int value)
{
    setY(value);
    QColor color;
    color.setCmyk(spinC->value(),
                  spinM->value(),
                  value,
                  spinK->value());
    emit colorChanged(color);
}

void CyanSliderCMYK::handleColorKChanged(int value)
{
    setK(value);
    QColor color;
    color.setCmyk(spinC->value(),
                  spinM->value(),
                  spinY->value(),
                  value);
    emit colorChanged(color);
}

