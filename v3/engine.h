/*
#
# Cyan Graphics - https://cyan.graphics - https://github.com/rodlie/cyan
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

#define CMS_NO_REGISTER_KEYWORD
#include <lcms2.h>

#define THEME_ICON_APPLICATIONS_GRAPHICS "applications-graphics"
#define THEME_ICON_DOCUMENT_OPEN "document-open"
#define THEME_ICON_DOCUMENT_SAVE "document-save"
#define THEME_ICON_VIDEO_DISPLAY "video-display"
#define THEME_ICON_PREFERENCES_COLOR "preferences-color"

#define CYAN_ICON "Cyan"
#define CYAN_ICON_IMAGE "image-x-generic"
#define CYAN_ICON_SUBWINDOW CYAN_ICON_IMAGE
#define CYAN_ICON_COLOR_WHEEL "colors"
#define CYAN_ICON_COLORS "color-wheel"
#define CYAN_ICON_COLOR_MAN "color_management"
#define CYAN_ICON_OPEN_IMAGE THEME_ICON_DOCUMENT_OPEN
#define CYAN_ICON_SAVE_IMAGE THEME_ICON_DOCUMENT_SAVE
#define CYAN_ICON_DISPLAY THEME_ICON_VIDEO_DISPLAY
#define CYAN_ICON_ZOOM "zoom"
#define CYAN_ICON_IMAGES "images"
#define CYAN_ICON_PRINTER "printer"
#define CYAN_ICON_PRINTER_COLOR "printer_color"
#define CYAN_ICON_STOP "process-stop"
#define CYAN_ICON_CONVERT_TO_GRAY "convert_color_to_gray"

#define CYAN_PROFILE_FALLBACK_RGB ":/icc/rgb.icc"
#define CYAN_PROFILE_FALLBACK_CMYK ":/icc/cmyk.icc"
#define CYAN_PROFILE_FALLBACK_GRAY ":/icc/gray.icc"

namespace Cyan
{
    class Engine : public QObject
    {
        Q_OBJECT

    public:

        enum ColorSpace {
            ColorSpaceUnknown,
            ColorSpaceRGB,
            ColorSpaceCMYK,
            ColorSpaceGRAY
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
            QByteArray source;
            QByteArray buffer;
            QByteArray profile;
            ColorSpace colorspace;
            QString errors;
            QString warnings;
            QString information;
            QString printInformation;
            QString filename;
        };
        struct ColorProfiles {
            QString rgb;
            QString cmyk;
            QString gray;
            QString display;
            QString print;
            QString source;
            QString destination;
            RenderingIntent intent = PerceptualRenderingIntent;
            bool blackpoint = true;
        };
        struct ColorSettings {
            ColorSpace colorspace = ColorSpaceUnknown;
            RenderingIntent intent = PerceptualRenderingIntent;
            bool blackpoint = true;
            bool applyDisplayProfile = false;
            bool applyPrintProfile = false;
            ColorProfiles profiles;
        };

        Engine(QObject *parent = nullptr);

        static const QByteArray fileToByteArray(const QString &filename);
        static bool isValidImage(const QString &filename);
        static bool isValidImage(const QByteArray &buffer);
        static bool isValidProfile(QByteArray buffer);
        static ColorSpace getFileColorspace(cmsHPROFILE profile);
        static ColorSpace getFileColorspace(const QString &filename);
        static ColorSpace getFileColorspace(QByteArray buffer);
        static const QString getProfileTag(cmsHPROFILE profile,
                                           ICCTag tag = ICCDescription);
        static const QString getProfileTag(const QString &filename,
                                           ICCTag tag = ICCDescription);
        static const QString getProfileTag(QByteArray buffer,
                                           ICCTag tag = ICCDescription);
        static const QMap<QString, QString> getProfiles(ColorSpace colorspace,
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
        static const Image readImageIM(const QString &filename,
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
                                   bool identify = false,
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
        static const QStringList supportedWriteFormats();
        static const QString humanFileSize(float num,
                                           bool mp = false,
                                           bool are = false);
    };
}

#endif // CYAN_ENGINE_H
