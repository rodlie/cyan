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

#ifndef COLORRGB_H
#define COLORRGB_H

#include <QWidget>
#include <QColor>
#include <QSpinBox>
#include <QSlider>

#include "CyanGlobal.h"

class CYANSHARED_EXPORT CyanSliderRGB : public QWidget
{
    Q_OBJECT

public:
    explicit CyanSliderRGB(QWidget *parent = nullptr);

signals:
    void colorChanged(const QColor &color);

public slots:
    const QColor getColor();
    void setColor(const QColor &color);

private:
    QSpinBox *spinR;
    QSpinBox *spinG;
    QSpinBox *spinB;
    QSlider *slideR;
    QSlider *slideG;
    QSlider *slideB;

private slots:
    void setR(int r);
    void setG(int g);
    void setB(int b);
    void handleColorRChanged(int value);
    void handleColorGChanged(int value);
    void handleColorBChanged(int value);
};

#endif // COLORRGB_H
