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

    struct Image
    {
        std::vector<unsigned char> imageBuffer;
        std::vector<unsigned char> previewBuffer;
        std::vector<unsigned char> iccInputBuffer;
        std::vector<unsigned char> iccOutputBuffer;
        size_t width = 0;
        size_t height = 0;
        size_t depth = 0;
        int channels = 0;
        FXX::ColorSpace colorspace = FXX::UnknownColorSpace;
        std::string iccDescription;
        std::string iccManufacturer;
        std::string iccModel;
        std::string iccCopyright;
        std::string comment;
        std::string info;
        std::string format;
        std::string filename;
        std::string created;
        std::string modified;
        std::string error;
        std::string warning;
        bool hasEXIF = false;
        bool hasIPTC = false;
    };

    FXX();

    FXX::Image readImage(std::string file,
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

    void clearImage(FXX::Image data);

    /*bool hasJPEG;
    bool hasPNG;
    bool hasTIFF;
    bool hasLCMS;
    bool hasPSD;
    int hasDepth;*/
};

#endif // FXX_H
