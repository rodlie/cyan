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

#ifndef COLORCONVERT_H
#define COLORCONVERT_H

#include <QObject>
#include <QString>

#include <Magick++.h>

#include "CyanGlobal.h"

class CYANSHARED_EXPORT ColorConvert: public QObject
{

    Q_OBJECT

public:

    ColorConvert(QObject *parent = nullptr);

    static Magick::Image convertColorspace(Magick::Image image,
                                           Magick::Blob input,
                                           Magick::Blob output,
                                           Magick::RenderingIntent intent = Magick::PerceptualIntent,
                                           bool blackpoint = true);
    static Magick::Image convertColorspace(Magick::Image image,
                                           const QString &input,
                                           const QString &output,
                                           Magick::RenderingIntent intent = Magick::PerceptualIntent,
                                           bool blackpoint = true);
    static Magick::Image convertColorspace(Magick::Image image,
                                           Magick::Blob input,
                                           const QString &output,
                                           Magick::RenderingIntent intent = Magick::PerceptualIntent,
                                           bool blackpoint = true);
    static Magick::Image convertColorspace(Magick::Image image,
                                           const QString &input,
                                           Magick::Blob output,
                                           Magick::RenderingIntent intent = Magick::PerceptualIntent,
                                           bool blackpoint = true);
};
#endif // COLORCONVERT_H
