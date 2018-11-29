/*
# Cyan <http://prepress.sf.net> <https://cyan.fxarena.net>,
# Copyright (C) 2016, 2017, 2018 Ole-André Rodlie<ole.andre.rodlie@gmail.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License version 2.1 as published by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, see <http://www.gnu.org/licenses/lgpl-2.0.html>
*/

#ifndef FXX_H
#define FXX_H

#include <iostream>
#include <vector>
#include <lcms2.h>

class FXX
{
public:
    enum ColorSpace {
        UnknownColorSpace,
        RGBColorSpace,
        CMYKColorSpace,
        GRAYColorSpace
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

    struct Image
    {
        std::vector<unsigned char> imageBuffer;
        std::vector<unsigned char> previewBuffer;
        std::vector<unsigned char> workBuffer;
        std::vector<unsigned char> iccInputBuffer;
        std::vector<unsigned char> iccOutputBuffer;
        std::vector<unsigned char> iccMonitorBuffer;
        std::vector<unsigned char> iccRGB;
        std::vector<unsigned char> iccCMYK;
        std::vector<unsigned char> iccGRAY;
        size_t width = 0;
        size_t height = 0;
        size_t depth = 0;
        int channels = 0;
        FXX::ColorSpace colorspace = FXX::UnknownColorSpace;
        FXX::RenderingIntent intent = FXX::UndefinedRenderingIntent;
        std::string comment;
        std::string info;
        std::string format;
        std::string filename;
        std::string created;
        std::string modified;
        std::string error;
        std::string warning;
        bool blackpoint = false;
        bool hasEXIF = false;
        bool hasIPTC = false;
    };

    FXX();

    static FXX::Image readImage(const std::string &file,
                                FXX::Image failsafe,
                                bool getInfo = true);
    static FXX::Image convertImage(FXX::Image input,
                                   bool getInfo = true);

    bool editProfile(std::string file,
                     std::string description,
                     std::string copyright);

    std::string getProfileTag(cmsHPROFILE profile,
                              FXX::ICCTag tag = FXX::ICCDescription);
    std::string getProfileTag(std::string file,
                              FXX::ICCTag tag = FXX::ICCDescription);
    std::string getProfileTag(std::vector<unsigned char> buffer,
                              FXX::ICCTag tag = FXX::ICCDescription);

    FXX::ColorSpace getProfileColorspace(std::vector<unsigned char> buffer);
    FXX::ColorSpace getProfileColorspace(std::string file);
    FXX::ColorSpace getProfileColorspace(cmsHPROFILE profile);

    static std::string identify(std::vector<unsigned char> buffer);
    static std::string identify(std::string file);

    std::string supportedQuantumDepth();
    std::string backendInfo();

    void clearImage(FXX::Image data);
    bool saveImage(FXX::Image data);

    bool hasJPEG();
    bool hasPNG();
    bool hasTIFF();
    bool hasLCMS();
    bool hasHDRI();
    bool hasMP();
};

#endif // FXX_H
