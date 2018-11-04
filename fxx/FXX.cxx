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

#include "FXX.h"
#include <Magick++.h>
#include <wand/magick_wand.h>
#include <lcms2.h>

FXX::imageData FXX::readImage(std::string file,
                              bool getInfo,
                              bool getThumb,
                              int thumbWidth,
                              int thumbHeight)
{
    FXX::imageData result;
    if (!file.empty()) {
        try {
            Magick::Image image;
            Magick::Blob output;
            image.read(file.c_str());

            result.comment = image.comment();
            result.imageWidth = image.columns();
            result.imageHeight = image.rows();
            result.imageDepth = image.depth();

            switch(image.colorSpace()) {
            case Magick::CMYKColorspace:
                result.imageColorspace = FXX::COLOR_CMYK;
                break;
            case Magick::GRAYColorspace:
                result.imageColorspace = FXX::COLOR_GRAY;
                break;
            case Magick::RGBColorspace:
            case Magick::sRGBColorspace:
            case Magick::TransparentColorspace:
                result.imageColorspace = FXX::COLOR_RGB;
                break;
            default:
                result.imageColorspace = FXX::COLOR_NONE;
            }

            result.imageChannels = 0;
            MagickCore::ImageInfo * imageInfo = image.imageInfo();
            if ((imageInfo->channel & Magick::RedChannel) != 0) { result.imageChannels++; }
            if ((imageInfo->channel & Magick::GreenChannel) != 0) { result.imageChannels++; }
            if ((imageInfo->channel & Magick::BlueChannel) != 0) { result.imageChannels++; }
            if (((imageInfo->channel & Magick::OpacityChannel) != 0)
               && (image.matte() != Magick::MagickFalse)) { result.imageChannels++; }
            if (((imageInfo->channel & Magick::IndexChannel) != 0)
               && (image.colorSpace() == Magick::CMYKColorspace)) { result.imageChannels++; }

            if (image.iccColorProfile().length()>0) {
                unsigned char *iccBuffer = (unsigned char*)image.iccColorProfile().data();
                std::vector<unsigned char> iccData(iccBuffer,
                                                   iccBuffer + image.iccColorProfile().length());
                result.iccBuffer = iccData;
                result.iccLength = image.iccColorProfile().length();
                result.iccDescription = readProfile("", result,
                                                    FXX::PROFILE_DESCRIPTION);
                result.iccManufacturer = readProfile("", result,
                                                     FXX::PROFILE_MANUFACTURER);
                result.iccModel = readProfile("", result,
                                              FXX::PROFILE_MODEL);
                result.iccCopyright = readProfile("", result,
                                                  FXX::PROFILE_COPYRIGHT);
                //image.profile("ICC", Magick::Blob());
                //image.profile("ICM", Magick::Blob());
            }

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

            result.created = image.attribute("date:create");
            result.modified = image.attribute("date:modified");
            result.filename = image.fileName();
            result.imageFormat = image.format();

            image.write(&output);

            unsigned char *imgBuffer = (unsigned char*)output.data();
            std::vector<unsigned char> imgData(imgBuffer,
                                               imgBuffer + output.length());
            result.imageBuffer = imgData;
            result.imageLength = output.length();
            if (getInfo) {
                result.imageInfo = identify(result.imageBuffer,
                                            result.imageLength);
            }

            if (getThumb) {
                image.depth(8);
                image.scale(Magick::Geometry((size_t)thumbWidth,
                                             (size_t)thumbHeight));
                Magick::Blob thumb;
                image.magick("TIFF");
                image.write(&thumb);
                unsigned char *thumbBuffer = (unsigned char*)thumb.data();
                std::vector<unsigned char> thumbData(thumbBuffer,
                                                     thumbBuffer + thumb.length());
                result.thumbBuffer = thumbData;
                result.thumbLength = thumb.length();
            }
        }
        catch(Magick::Error &error_ ) {
            result.errorMessage.append(error_.what());
        }
        catch(Magick::Warning &warn_ ) {
            result.warningMessage.append(warn_.what());
        }
    }
    return result;
}

std::string FXX::readProfile(std::string file,
                             FXX::imageData data,
                             FXX::ProfileInfo type)
{
    std::string result;
    cmsHPROFILE lcmsProfile = NULL;
    if (!file.empty()) {
        lcmsProfile = cmsOpenProfileFromFile(file.c_str(), "r");
    } else if (data.imageLength>0) {
        lcmsProfile = cmsOpenProfileFromMem(data.iccBuffer.data(),
                                            data.iccLength);
    }
    if (lcmsProfile) {
        cmsUInt32Number size = 0;
        cmsInfoType cmsSelectedType;
        switch(type) {
        case FXX::PROFILE_DESCRIPTION:
            cmsSelectedType = cmsInfoDescription;
            break;
        case FXX::PROFILE_MANUFACTURER:
            cmsSelectedType = cmsInfoManufacturer;
            break;
        case FXX::PROFILE_MODEL:
            cmsSelectedType = cmsInfoModel;
            break;
        case FXX::PROFILE_COPYRIGHT:
            cmsSelectedType = cmsInfoCopyright;
            break;
        default:
            cmsSelectedType = cmsInfoDescription;
        }
        size = cmsGetProfileInfoASCII(lcmsProfile,
                                      cmsSelectedType,
                                      "en",
                                      "US",
                                      NULL,
                                      0);
        if (size > 0) {
            char buffer[size+1];
            size = cmsGetProfileInfoASCII(lcmsProfile,
                                          cmsSelectedType,
                                          "en",
                                          "US",
                                          buffer, size);
            if (size > 0) {
                result = buffer;
            }
        }
    }
    cmsCloseProfile(lcmsProfile);
    return result;
}

bool FXX::editProfile(std::string file,
                      std::string description,
                      std::string copyright)
{
    bool result = false;
    if (!file.empty()) {
        cmsHPROFILE lcmsProfile = NULL;
        lcmsProfile = cmsOpenProfileFromFile(file.c_str(), "w");
        if (lcmsProfile) {
            cmsContext ContextID = cmsGetProfileContextID(lcmsProfile);
            bool modified = false;
            if (!description.empty()) {
                cmsMLU *DescriptionMLU;
                DescriptionMLU  = cmsMLUalloc(ContextID, 1);
                std::wstring descriptionW = std::wstring(description.begin(),
                                                         description.end());
                if (!cmsMLUsetWide(DescriptionMLU,
                                   "en", "US",
                                   descriptionW.c_str())) {
                    result = false;
                }
                if (cmsWriteTag(lcmsProfile,
                                cmsSigProfileDescriptionTag,
                                DescriptionMLU)) {
                    modified = true;
                }
                if (DescriptionMLU) {
                    cmsMLUfree(DescriptionMLU);
                }
            }
            if (!copyright.empty()) {
                cmsMLU *CopyrightMLU;
                CopyrightMLU  = cmsMLUalloc(ContextID, 1);
                std::wstring copyrightW = std::wstring(copyright.begin(),
                                                       copyright.end());
                if (!cmsMLUsetWide(CopyrightMLU,
                                   "en",
                                   "US",
                                   copyrightW.c_str())) {
                    result = false;
                }
                if (cmsWriteTag(lcmsProfile,
                                cmsSigCopyrightTag,
                                CopyrightMLU)) {
                    modified = true;
                }
                if (CopyrightMLU) {
                    cmsMLUfree(CopyrightMLU);
                }
            }
            if (modified) {
                if (cmsSaveProfileToFile(lcmsProfile,
                                         file.c_str())) {
                    result = true;
                }
            }
        }
        cmsCloseProfile(lcmsProfile);
    }
    return result;
}

FXX::ColorspaceType FXX::readProfileColorspace(std::string file,
                                               imageData data)
{
    FXX::ColorspaceType result = FXX::COLOR_NONE;
    cmsHPROFILE lcmsProfile = NULL;
    if (!file.empty()) {
        lcmsProfile = cmsOpenProfileFromFile(file.c_str(), "r");
    } else if (data.imageLength>0) {
        size_t iccLength = data.imageLength;
        unsigned char* iccData = data.imageBuffer.data();
        lcmsProfile = cmsOpenProfileFromMem(iccData,
                                            iccLength);
    }
    if (lcmsProfile) {
        if (cmsGetColorSpace(lcmsProfile) == cmsSigRgbData) {
            result = FXX::COLOR_RGB;
        } else if (cmsGetColorSpace(lcmsProfile) == cmsSigCmykData) {
            result = FXX::COLOR_CMYK;
        } else if (cmsGetColorSpace(lcmsProfile) == cmsSigGrayData) {
            result = FXX::COLOR_GRAY;
        }
    }
    cmsCloseProfile(lcmsProfile);
    return result;
}

std::string FXX::identify(std::vector<unsigned char> buffer,
                          size_t length)
{
    std::string result;
    if (length>0) {
        MagickCore::MagickWand *wand = MagickCore::NewMagickWand();
        if (MagickReadImageBlob(wand,
                                buffer.data(),
                                length)) {
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
        if (MagickReadImage(wand,
                            file.c_str())) {
            result = MagickIdentifyImage(wand);
        }
        wand = DestroyMagickWand(wand);
    }
    return result;
}

/*void FXX::clearData(fxxData data)
{
    data.comment.clear();
    data.created.clear();
    data.errorMessage.clear();
    data.filename.clear();
    data.hasEXIF = false;
    data.hasIPTC = false;
    data.iccBuffer.clear();
    data.iccCopyright.clear();
    data.iccDescription.clear();
    data.iccLength = 0;
    data.iccManufacturer.clear();
    data.iccModel.clear();
    data.imageBuffer.clear();
    data.imageChannels = 0;
    data.imageColorspace = fxxUnknownColorSpace;
    data.imageDepth = 0;
    data.imageHeight = 0;
    data.modified.clear();
    data.warningMessage.clear();
    data.imageFormat.clear();
    data.thumbBuffer.clear();
    data.thumbLength = 0;
}*/
