#include "FXX.h"
#include <Magick++.h>
#include <wand/magick_wand.h>

FXX::FXX()
{
    Magick::InitializeMagick(nullptr);
}

FXX::Image FXX::readImage(std::string file,
                          bool getInfo)
{
    FXX::Image result;
    if (!file.empty()) {
        try {
            Magick::Image image;
            Magick::Blob output;
            Magick::Blob preview;
            image.read(file.c_str());

            // get colorspace
            switch(image.colorSpace()) {
            case Magick::CMYKColorspace:
                result.colorspace = FXX::CMYKColorSpace;
                break;
            case Magick::GRAYColorspace:
                result.colorspace = FXX::GRAYColorSpace;
                break;
            case Magick::RGBColorspace:
            case Magick::sRGBColorspace:
            case Magick::TransparentColorspace:
                result.colorspace = FXX::RGBColorSpace;
                break;
            default:
                result.colorspace = FXX::UnknownColorSpace;
            }

            // get image channels
            result.channels = 0;
            MagickCore::ImageInfo * imageInfo = image.imageInfo();
            if ((imageInfo->channel & Magick::RedChannel) != 0) result.channels++;
            if ((imageInfo->channel & Magick::GreenChannel) != 0) result.channels++;
            if ((imageInfo->channel & Magick::BlueChannel) != 0) result.channels++;
            if (((imageInfo->channel & Magick::OpacityChannel) != 0)
               && (image.matte() != Magick::MagickFalse)) result.channels++;
            if (((imageInfo->channel & Magick::IndexChannel) != 0)
               && (image.colorSpace() == Magick::CMYKColorspace)) result.channels++;

            // has embedded color profile?
            if (image.iccColorProfile().length()>0) {
                unsigned char *iccBuffer = (unsigned char*)image.iccColorProfile().data();
                std::vector<unsigned char> iccData(iccBuffer, iccBuffer + image.iccColorProfile().length());
                result.iccInputBuffer = iccData;
                result.iccDescription = getProfileTag(iccData,
                                                      FXX::ICCDescription);
                result.iccManufacturer = getProfileTag(iccData,
                                                       FXX::ICCManufacturer);
                result.iccModel = getProfileTag(iccData,
                                                FXX::ICCModel);
                result.iccCopyright = getProfileTag(iccData,
                                                    FXX::ICCCopyright);
                //image.profile("ICC", Magick::Blob());
                //image.profile("ICM", Magick::Blob());
            }

            // get meta info
            if (image.profile("exif").length()>0) {
                result.hasEXIF = true;
            } else {
                result.hasEXIF = false;
            }
            if (image.profile("IPTC").length()>0) {
                result.hasIPTC = true;
            } else {
                result.hasIPTC = false;
            }
            result.comment = image.comment();
            result.width = image.columns();
            result.height = image.rows();
            result.depth = image.depth();
            result.created = image.attribute("date:create");
            result.modified = image.attribute("date:modified");
            result.filename = image.fileName();
            result.format = image.format();

            // write original
            image.write(&output);
            unsigned char *imgBuffer = (unsigned char*)output.data();
            std::vector<unsigned char> imgData(imgBuffer, imgBuffer + output.length());
            result.imageBuffer = imgData;

            // make a preview
            if (image.depth()>8) { image.depth(8); }
            image.magick("BMP");
            image.write(&preview);
            unsigned char *preBuffer = (unsigned char*)preview.data();
            std::vector<unsigned char> preData(preBuffer, preBuffer + preview.length());
            result.previewBuffer = preData;

            // get image specs?
            if (getInfo) {
                result.info = identify(result.imageBuffer);
            }
        }
        catch(Magick::Error &error_ ) {
            result.error.append(error_.what());
        }
        catch(Magick::Warning &warn_ ) {
            result.warning.append(warn_.what());
        }
    }
    return result;
}

bool FXX::editProfile(std::string file,
                      std::string description,
                      std::string copyright)
{
    bool result = false;
    if (!file.empty()) {
        cmsHPROFILE lcmsProfile = nullptr;
        lcmsProfile = cmsOpenProfileFromFile(file.c_str(), "w");
        if (lcmsProfile) {
            cmsContext ContextID = cmsGetProfileContextID(lcmsProfile);
            bool modified = false;
            if (!description.empty()) {
                cmsMLU *DescriptionMLU;
                DescriptionMLU  = cmsMLUalloc(ContextID, 1);
                std::wstring descriptionW = std::wstring(description.begin(), description.end());
                if (!cmsMLUsetWide(DescriptionMLU,  "en", "US", descriptionW.c_str())) {
                    result = false;
                }
                if (cmsWriteTag(lcmsProfile, cmsSigProfileDescriptionTag,  DescriptionMLU)) {
                    modified = true;
                }
                if (DescriptionMLU) {
                    cmsMLUfree(DescriptionMLU);
                }
            }
            if (!copyright.empty()) {
                cmsMLU *CopyrightMLU;
                CopyrightMLU  = cmsMLUalloc(ContextID, 1);
                std::wstring copyrightW = std::wstring(copyright.begin(), copyright.end());
                if (!cmsMLUsetWide(CopyrightMLU,  "en", "US", copyrightW.c_str())) {
                    result = false;
                }
                if (cmsWriteTag(lcmsProfile, cmsSigCopyrightTag,  CopyrightMLU)) {
                    modified = true;
                }
                if (CopyrightMLU) {
                    cmsMLUfree(CopyrightMLU);
                }
            }
            if (modified) {
                if (cmsSaveProfileToFile(lcmsProfile, file.c_str())) {
                    result = true;
                }
            }
        }
        cmsCloseProfile(lcmsProfile);
    }
    return result;
}

std::string FXX::getProfileTag(cmsHPROFILE profile,
                               FXX::ICCTag tag)
{
    std::string result;
    if (profile) {
        cmsUInt32Number size = 0;
        cmsInfoType cmsSelectedType;
        switch(tag) {
        case FXX::ICCDescription:
            cmsSelectedType = cmsInfoDescription;
            break;
        case FXX::ICCManufacturer:
            cmsSelectedType = cmsInfoManufacturer;
            break;
        case FXX::ICCModel:
            cmsSelectedType = cmsInfoModel;
            break;
        case FXX::ICCCopyright:
            cmsSelectedType = cmsInfoCopyright;
            break;
        }
        size = cmsGetProfileInfoASCII(profile, cmsSelectedType,
                                      "en", "US", nullptr, 0);
        if (size > 0) {
            char buffer[size+1];
            size = cmsGetProfileInfoASCII(profile, cmsSelectedType,
                                          "en", "US", buffer, size);
            if (size > 0) {
                result = buffer;
            }
        }
    }
    cmsCloseProfile(profile);
    return result;
}

std::string FXX::getProfileTag(std::string file,
                               FXX::ICCTag tag)
{
    if (!file.empty()) {
        return getProfileTag(cmsOpenProfileFromFile(file.c_str(), "r"), tag);
    }
    return "";
}

std::string FXX::getProfileTag(std::vector<unsigned char> buffer,
                               FXX::ICCTag tag)
{
    if (buffer.size()>0) {
        return getProfileTag(cmsOpenProfileFromMem(buffer.data(),
                                                   (cmsUInt32Number)buffer.size()),tag);
    }
    return "";
}

FXX::ColorSpace FXX::getProfileColorspace(std::vector<unsigned char> buffer)
{
    if (buffer.size()>0) {
        return getProfileColorspace(cmsOpenProfileFromMem(buffer.data(),
                                                          (cmsUInt32Number)buffer.size()));
    }
    return FXX::UnknownColorSpace;
}

FXX::ColorSpace FXX::getProfileColorspace(std::string file)
{
    if (!file.empty()) {
        return getProfileColorspace(cmsOpenProfileFromFile(file.c_str(), "r"));
    }
    return FXX::UnknownColorSpace;
}

FXX::ColorSpace FXX::getProfileColorspace(cmsHPROFILE profile)
{
    FXX::ColorSpace result = FXX::UnknownColorSpace;
    if (profile) {
        if (cmsGetColorSpace(profile) == cmsSigRgbData) {
            result = FXX::RGBColorSpace;
        } else if (cmsGetColorSpace(profile) == cmsSigCmykData) {
            result = FXX::CMYKColorSpace;
        } else if (cmsGetColorSpace(profile) == cmsSigGrayData) {
            result = FXX::GRAYColorSpace;
        }
    }
    cmsCloseProfile(profile);
    return result;
}

std::string FXX::identify(std::vector<unsigned char> buffer)
{
    std::string result;
    if (buffer.size()>0) {
        MagickCore::MagickWand *wand = MagickCore::NewMagickWand();
        if (MagickReadImageBlob(wand, buffer.data(), buffer.size())) {
            result = MagickIdentifyImage(wand);
        }
        wand = DestroyMagickWand(wand);
    }
    return result;
}

std::string FXX::identify(std::string file)
{
    std::string result;
    if (!file.empty()) {
        MagickCore::MagickWand *wand = MagickCore::NewMagickWand();
        if (MagickReadImage(wand, file.c_str())) {
            result = MagickIdentifyImage(wand);
        }
        wand = DestroyMagickWand(wand);
    }
    return result;
}

void FXX::clearImage(FXX::Image data)
{
    data.comment.clear();
    data.created.clear();
    data.error.clear();
    data.filename.clear();
    data.hasEXIF = false;
    data.hasIPTC = false;
    data.iccInputBuffer.clear();
    data.iccOutputBuffer.clear();
    data.iccCopyright.clear();
    data.iccDescription.clear();
    data.iccManufacturer.clear();
    data.iccModel.clear();
    data.imageBuffer.clear();
    data.previewBuffer.clear();
    data.channels = 0;
    data.colorspace = FXX::UnknownColorSpace;
    data.depth = 0;
    data.height = 0;
    data.modified.clear();
    data.warning.clear();
    data.format.clear();
}
