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

    FXX::Image readImage(const std::string &file,
                         FXX::Image failsafe,
                         bool getInfo = true);
    FXX::Image convertImage(FXX::Image input,
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


    std::string identify(std::vector<unsigned char> buffer);
    std::string identify(std::string file);

    std::string supportedQuantumDepth();
    std::string backendInfo();

    void clearImage(FXX::Image data);

    /*bool hasJPEG;
    bool hasPNG;
    bool hasTIFF;
    bool hasLCMS;
    bool hasPSD;
    int hasDepth;*/
};

#endif // FXX_H
