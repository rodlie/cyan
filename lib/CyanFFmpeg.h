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

#ifndef CYANFFMPEG_H
#define CYANFFMPEG_H

#ifdef WITH_FFMPEG
extern "C" {
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
}
#include <Magick++.h>
#include <QByteArray>
#include <QString>

class CyanFFmpeg
{
public:
    static QByteArray getEmbeddedCoverArt(const QString &filename);
    static int getVideoMaxFrames(const QString &filename);
    static Magick::Image getVideoFrame(const QString &filename,
                                       int frame = 0);
};
#endif

#endif // CYANFFMPEG_H
