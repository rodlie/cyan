/*
#
# Cyan - https://github.com/rodlie/cyan
#
# Copyright (c) Ole-André Rodlie <ole.andre.rodlie@gmail.com>. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
#
*/

#ifndef CYAN_ENGINE_H
#define CYAN_ENGINE_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QMap>
#include <QSize>

#include <lcms2.h>

namespace Cyan
{
    class Engine : public QObject
    {
        Q_OBJECT

    public:

        enum colorSpace {
            colorSpaceUnknown,
            colorSpaceRGB,
            colorSpaceCMYK,
            colorSpaceGRAY
        };
        enum ICCTag {
            ICCDescription,
            ICCManufacturer,
            ICCModel,
            ICCCopyright
        };
        enum RenderingIntent {
            UndefinedRenderingIntent,
            SaturationRenderingIntent,
            PerceptualRenderingIntent,
            AbsoluteRenderingIntent,
            RelativeRenderingIntent
        };
        struct Image {
            bool success = false;
            int width = 0;
            int height = 0;
            QByteArray buffer;
            QByteArray profile;
            colorSpace colorspace;
            QString errors;
            QString warnings;
            QString information;
            QString filename;
        };
        struct ColorProfiles {
            QString rgb;
            QString cmyk;
            QString gray;
            QString display;
        };
        struct ColorSettings {
            colorSpace colorspace = colorSpaceUnknown;
            RenderingIntent intent = PerceptualRenderingIntent;
            bool blackpoint = true;
            bool applyDisplayProfile = false;
            ColorProfiles profiles;
        };

        Engine(QObject *parent = nullptr);

        static const QByteArray fileToByteArray(const QString &filename);
        static bool isValidImage(const QString &filename);
        static bool isValidImage(const QByteArray &buffer);
        static bool isValidProfile(QByteArray buffer);
        static colorSpace getFileColorspace(cmsHPROFILE profile);
        static colorSpace getFileColorspace(const QString &filename);
        static colorSpace getFileColorspace(QByteArray buffer);
        static const QString getProfileTag(cmsHPROFILE profile,
                                           ICCTag tag = ICCDescription);
        static const QString getProfileTag(const QString &filename,
                                           ICCTag tag = ICCDescription);
        static const QString getProfileTag(QByteArray buffer,
                                           ICCTag tag = ICCDescription);
        static const QMap<QString, QString> getProfiles(colorSpace colorspace,
                                                        bool returnPaths = false,
                                                        const QString &fallback = QString(),
                                                        bool forceFallback = false);
        static const Image readImage(const QString &filename,
                                     const QString &fallbackProfileRGB = QString(),
                                     const QString &fallbackProfileCMYK = QString(),
                                     const QString &fallbackProfileGRAY = QString(),
                                     const RenderingIntent intent = PerceptualRenderingIntent,
                                     bool blackPoint = true,
                                     bool identify = false);
        static bool compareImages( const QByteArray image1,
                                   const QByteArray image2,
                                   const QString &tmpPath = QString() );
        static Image convertImage( const QByteArray &inputFileData,
                                   const QByteArray &inputProfileData,
                                   const QByteArray &outputProfileData,
                                   RenderingIntent intent = PerceptualRenderingIntent,
                                   bool blackPoint = true,
                                   bool forceInputProfile = false,
                                   bool assignInputProfile = false,
                                   bool applyInputProfile = true,
                                   bool applyOutputProfile = true,
                                   bool checkifValidResult = true,
                                   bool display = false,
                                   const QSize &scale = QSize() );
        static bool hasDelegate(const QString &delegate);
        static bool hasFeature(const QString &feature);
        static const QString supportedQuantumDepth();
        static bool hasJPEG();
        static bool hasPNG();
        static bool hasTIFF();
        static bool hasLCMS();
        static bool hasHDRI();
        static bool hasMP();
        static const QString identify(const QByteArray &buffer);
        static const QString identify(const QString &filename);
        static int getDiskResource();
        static void setDiskResource(int gib);
        static int getMemoryResource();
        static void setMemoryResource(int gib);
        static int getTotalRam(int percent);
        static const QStringList supportedReadFormats();
    };
}

#endif // CYAN_ENGINE_H
