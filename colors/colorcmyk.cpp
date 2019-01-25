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

#include "colorcmyk.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

ColorCMYK::ColorCMYK(QWidget *parent) :
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

    //mainLayout->addStretch();

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

const QColor ColorCMYK::getColor()
{
    QColor color;
    color.setCmyk(spinC->value(),
                  spinM->value(),
                  spinY->value(),
                  spinK->value());
    return color;
}

void ColorCMYK::setColor(const QColor &color)
{
    setC(color.cyan());
    setM(color.magenta());
    setY(color.yellow());
    setK(color.black());
}

void ColorCMYK::setC(int c)
{
    spinC->blockSignals(true);
    slideC->blockSignals(true);
    if (spinC->value() != c) { spinC->setValue(c); }
    if (slideC->value() != c) { slideC->setValue(c); }
    spinC->blockSignals(false);
    slideC->blockSignals(false);
}

void ColorCMYK::setM(int m)
{
    spinM->blockSignals(true);
    slideM->blockSignals(true);
    if (spinM->value() != m) { spinM->setValue(m); }
    if (slideM->value() != m) { slideM->setValue(m); }
    spinM->blockSignals(false);
    slideM->blockSignals(false);
}

void ColorCMYK::setY(int y)
{
    spinY->blockSignals(true);
    slideY->blockSignals(true);
    if (spinY->value() != y) { spinY->setValue(y); }
    if (slideY->value() != y) { slideY->setValue(y); }
    spinY->blockSignals(false);
    slideY->blockSignals(false);
}

void ColorCMYK::setK(int k)
{
    spinK->blockSignals(true);
    slideK->blockSignals(true);
    if (spinK->value() != k) { spinK->setValue(k); }
    if (slideK->value() != k) { slideK->setValue(k); }
    spinK->blockSignals(false);
    slideK->blockSignals(false);
}

void ColorCMYK::handleColorCChanged(int value)
{
    setC(value);
    QColor color;
    color.setCmyk(value,
                  spinM->value(),
                  spinY->value(),
                  spinK->value());
    emit colorChanged(color);
}

void ColorCMYK::handleColorMChanged(int value)
{
    setM(value);
    QColor color;
    color.setCmyk(spinC->value(),
                  value,
                  spinY->value(),
                  spinK->value());
    emit colorChanged(color);
}

void ColorCMYK::handleColorYChanged(int value)
{
    setY(value);
    QColor color;
    color.setCmyk(spinC->value(),
                  spinM->value(),
                  value,
                  spinK->value());
    emit colorChanged(color);
}

void ColorCMYK::handleColorKChanged(int value)
{
    setK(value);
    QColor color;
    color.setCmyk(spinC->value(),
                  spinM->value(),
                  spinY->value(),
                  value);
    emit colorChanged(color);
}

