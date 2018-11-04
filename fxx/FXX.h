/*
# Cyan <http://prepress.sf.net> <https://cyan.fxarena.net>,
# Copyright (C) 2016, 2017, 2018 Ole-André Rodlie <ole.andre.rodlie@gmail.com>
#
# Cyan is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.
#
# Cyan is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Cyan.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>
*/

#ifndef FXX_H
#define FXX_H

#include <iostream>
#include <vector>

class FXX
{
public:
    enum ProfileInfo {
        PROFILE_NONE,
        PROFILE_DESCRIPTION,
        PROFILE_MANUFACTURER,
        PROFILE_COPYRIGHT,
        PROFILE_MODEL,
        PROFILE_COLORSPACE
    };
    enum ColorspaceType {
        COLOR_NONE,
        COLOR_RGB,
        COLOR_CMYK,
        COLOR_GRAY
    };
    struct imageData {
        std::vector<unsigned char> imageBuffer;
        size_t imageLength;
        size_t imageWidth;
        size_t imageHeight;
        size_t imageDepth;
        std::vector<unsigned char> iccBuffer;
        size_t iccLength;
        std::string comment;
        FXX::ColorspaceType imageColorspace;
        int imageChannels;
        std::string iccDescription;
        std::string iccManufacturer;
        std::string iccModel;
        std::string iccCopyright;
        //std::string iccInfo;
        bool hasEXIF;
        bool hasIPTC;
        std::string imageInfo;
        std::string imageFormat;
        std::string filename;
        std::string created;
        std::string modified;
        std::string errorMessage;
        std::string warningMessage;
        std::vector<unsigned char> thumbBuffer;
        size_t thumbLength;
    };
    struct profileData {
        std::vector<unsigned char> buffer;
        size_t length;
    };
    static FXX::imageData readImage(std::string file,
                                    bool getInfo,
                                    bool getThumb,
                                    int thumbWidth = 0,
                                    int thumbHeight = 0);
    static std::string readProfile(std::string file,
                                   FXX::imageData data,
                                   FXX::ProfileInfo type = FXX::PROFILE_DESCRIPTION);
    static bool editProfile(std::string file,
                            std::string description,
                            std::string copyright);
    static FXX::ColorspaceType readProfileColorspace(std::string file,
                                                     FXX::imageData data);
    static std::string identify(std::vector<unsigned char> buffer,
                                size_t length);
    static std::string identify(std::string file);
    //void clearData(fxxData data);
    /*bool hasJPEG;
    bool hasPNG;
    bool hasTIFF;
    bool hasLCMS;
    bool hasPSD;
    int hasDepth;*/
};

#endif // FXX_H
