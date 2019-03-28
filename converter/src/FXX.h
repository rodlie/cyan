/*
# Copyright Ole-André Rodlie, INRIA.
#
# ole.andre.rodlie@gmail.com
#
# This software is governed by the CeCILL license under French law and
# abiding by the rules of distribution of free software. You can use,
# modify and / or redistribute the software under the terms of the CeCILL
# license as circulated by CEA, CNRS and INRIA at the following URL
# "https://www.cecill.info".
#
# As a counterpart to the access to the source code and rights to
# modify and redistribute granted by the license, users are provided only
# with a limited warranty and the software's author, the holder of the
# economic rights and the subsequent licensors have only limited
# liability.
#
# In this respect, the user's attention is drawn to the associated risks
# with loading, using, modifying and / or developing or reproducing the
# software by the user in light of its specific status of free software,
# that can mean that it is complicated to manipulate, and that also
# so that it is for developers and experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to test and test the software's suitability
# Requirements in the conditions of their systems
# data to be ensured and, more generally, to use and operate
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL license and that you accept its terms.
*/

#ifndef FXX_H
#define FXX_H

#include <iostream>
#include <vector>
#include <Magick++.h>
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
        std::vector<Magick::Image> layers;
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
                                bool getInfo = true,
                                bool readLayers = false);
    static FXX::Image readImage(Magick::Image image,
                                FXX::Image failsafe,
                                bool getInfo = true);

    static std::vector<unsigned char> generateThumb(Magick::Image image,
                                                    int width = 75,
                                                    int height = 75);

    static FXX::Image convertImage(FXX::Image input,
                                   bool getInfo = true);

    static FXX::ColorSpace readImageColorspaceType(Magick::Image image);
    static int readImageChannelCount(Magick::Image image);
    static std::vector<unsigned char> readImageColorProfile(Magick::Image image,
                                              FXX::Image failsafe);

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
    static std::string identify(Magick::Image image);
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
