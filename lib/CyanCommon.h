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

#ifndef CYAN_COMMON_H
#define CYAN_COMMON_H

#include <QObject>
#include <QMap>
#include <QDateTime>
#include <QMenu>
#include <QPair>

#include <list>
#include <lcms2.h>
#include <Magick++.h>

#include "CyanGlobal.h"

class CYANSHARED_EXPORT CyanCommon: public QObject
{
    Q_OBJECT

public:
    enum ICCTag {
        ICCDescription,
        ICCManufacturer,
        ICCModel,
        ICCCopyright
    };

    enum Colorspace
    {
        NoColorspace,
        RGBColorspace,
        CMYKColorspace,
        GRAYColorspace
    };

    enum newDialogType
    {
        newImageDialogType,
        newLayerDialogType
    };

    enum RenderingIntent
    {
        UndefinedRenderingIntent,
        SaturationRenderingIntent,
        PerceptualRenderingIntent,
        AbsoluteRenderingIntent,
        RelativeRenderingIntent
    };

    enum OutputFormatConverter
    {
        OutputFormatConverterTiff,
        OutputFormatConverterJpeg,
        OutputFormatConverterPsd
    };

    CyanCommon(QObject *parent = nullptr);

    //static QString timestamp();


    static const QString canvasWindowTitle(Magick::Image image);

    static int getDiskResource();
    static void setDiskResource(int gib);

    static int getMemoryResource();
    static void setMemoryResource(int gib);
    static int getTotalRam(int percent = 75);
    static void setThreadResources(int thread);


    static QStringList getColorProfilesPath();
    static QMap<QString, QString> getColorProfiles(Magick::ColorspaceType colorspace);

    static Magick::ColorspaceType getProfileColorspace(const QString &filename);
    static Magick::ColorspaceType getProfileColorspace(cmsHPROFILE profile);

    static const QString getProfileTag(Magick::Blob buffer,
                                       CyanCommon::ICCTag tag = CyanCommon::ICCDescription);
    static const QString getProfileTag(const QString filename,
                                       CyanCommon::ICCTag tag = CyanCommon::ICCDescription);
    static const QString getProfileTag(cmsHPROFILE profile,
                                       CyanCommon::ICCTag tag = CyanCommon::ICCDescription);


    static const QString humanFileSize(float num,
                                       bool mp = false,
                                       bool are = false);


    static const QString colorspaceToString(Magick::ColorspaceType colorspace);

    static const QString supportedImageMagickVersion();
};

#endif // CYANCOMMON_H
