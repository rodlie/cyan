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

#ifndef COLORHSV_H
#define COLORHSV_H

#include <QWidget>
#include <QColor>
#include <QDoubleSpinBox>
#include <QSlider>

class CyanSliderHSV : public QWidget
{
    Q_OBJECT

public:

    explicit CyanSliderHSV(QWidget *parent = nullptr);

signals:

    void colorChanged(const QColor &color);

public slots:

    const QColor getColor();
    void setColor(const QColor &color);

private:

    QDoubleSpinBox *spinH;
    QDoubleSpinBox *spinS;
    QDoubleSpinBox *spinV;

    QSlider *slideH;
    QSlider *slideS;
    QSlider *slideV;

private slots:

    void setH(qreal h);
    void setS(qreal s);
    void setV(qreal v);
    void handleColorHChanged(double value);
    void handleColorSChanged(double value);
    void handleColorVChanged(double value);

    void handleSliderHMoved(int value);
    void handleSliderSMoved(int value);
    void handleSliderVMoved(int value);
};

#endif // COLORHSV_H
