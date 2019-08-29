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

#include "CyanColorConvert.h"

#include <QDebug>

ColorConvert::ColorConvert(QObject *parent) : QObject (parent)
{

}

Magick::Image ColorConvert::convertColorspace(Magick::Image image,
                                              Magick::Blob input,
                                              Magick::Blob output,
                                              Magick::RenderingIntent intent,
                                              bool blackpoint)
{
    if (output.length()>0) {
        try {
            image.quiet(true);
            image.renderingIntent(intent);
            image.blackPointCompensation(blackpoint);
            if (input.length()>0) { image.profile("ICC", input); }
            image.profile("ICC", output);
        }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
    }
    return image;
}

Magick::Image ColorConvert::convertColorspace(Magick::Image image,
                                              const QString &input,
                                              const QString &output,
                                              Magick::RenderingIntent intent,
                                              bool blackpoint)
{
    Magick::Blob blob1,blob2;
    Magick::Image readBlob1,readBlob2;
    try {
        readBlob1.read(input.toStdString());
        readBlob1.write(&blob1);
        readBlob2.read(output.toStdString());
        readBlob2.write(&blob2);
        if (blob1.length()>0 && blob2.length()>0) {
            return convertColorspace(image,
                                     blob1,
                                     blob2,
                                     intent,
                                     blackpoint);
        }
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
    return Magick::Image();
}

Magick::Image ColorConvert::convertColorspace(Magick::Image image,
                                              Magick::Blob input,
                                              const QString &output,
                                              Magick::RenderingIntent intent,
                                              bool blackpoint)
{
    Magick::Blob blob;
    Magick::Image readBlob;
    try {
        readBlob.read(output.toStdString());
        readBlob.write(&blob);
        if (blob.length()>0) {
            return convertColorspace(image,
                                     input,
                                     blob,
                                     intent,
                                     blackpoint);
        }
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
    return Magick::Image();
}

Magick::Image ColorConvert::convertColorspace(Magick::Image image,
                                              const QString &input,
                                              Magick::Blob output,
                                              Magick::RenderingIntent intent,
                                              bool blackpoint)
{
    Magick::Blob blob;
    Magick::Image readBlob;
    try {
        readBlob.read(input.toStdString());
        readBlob.write(&blob);
        if (blob.length()>0) {
            return convertColorspace(image,
                                     blob,
                                     output,
                                     intent,
                                     blackpoint);
        }
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
    return Magick::Image();
}
