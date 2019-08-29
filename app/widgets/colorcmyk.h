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

#ifndef COLORCMYK_H
#define COLORCMYK_H

#include <QWidget>
#include <QColor>
#include <QSpinBox>
#include <QSlider>

class ColorCMYK : public QWidget
{
    Q_OBJECT

public:

    explicit ColorCMYK(QWidget *parent = nullptr);

signals:

    void colorChanged(const QColor &color);

public slots:

    const QColor getColor();
    void setColor(const QColor &color);

private:

    QSpinBox *spinC;
    QSpinBox *spinM;
    QSpinBox *spinY;
    QSpinBox *spinK;

    QSlider *slideC;
    QSlider *slideM;
    QSlider *slideY;
    QSlider *slideK;

private slots:

    void setC(int c);
    void setM(int m);
    void setY(int y);
    void setK(int k);
    void handleColorCChanged(int value);
    void handleColorMChanged(int value);
    void handleColorYChanged(int value);
    void handleColorKChanged(int value);
};

#endif // COLORRGB_H
