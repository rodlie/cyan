/*
# Cyan <http://prepress.sf.net> <https://cyan.fxarena.net>,
# Copyright (C) 2016, 2017, 2018 Ole-André Rodlie<ole.andre.rodlie@gmail.com>
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

FXX::FXX()
{
    Magick::InitializeMagick(nullptr);
}

FXX::Image FXX::readImage(const std::string &file,
                          FXX::Image failsafe,
                          bool getInfo)
{
    FXX::Image result;
    if (!file.empty()) {
        Magick::Image image;
        Magick::Blob output;
        Magick::Blob preview;
        try {
            image.read(file.c_str());
            image.magick("MIFF");
        }
        catch(Magick::Error &error_ ) {
            result.error.append(error_.what());
            return result;
        }
        catch(Magick::Warning &warn_ ) {
            result.warning.append(warn_.what());
        }
        try {
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
                unsigned char *iccBuffer = reinterpret_cast<unsigned char*>(const_cast<void*>(image.iccColorProfile().data()));
                std::vector<unsigned char> iccData(iccBuffer, iccBuffer + image.iccColorProfile().length());
                result.iccInputBuffer = iccData;
            } else { // apply failsafe profile if missing input profile
                if (failsafe.iccRGB.size()==0 ||
                    failsafe.iccCMYK.size()==0 ||
                    failsafe.iccGRAY.size()==0)
                {
                    result.error = "No default input profiles!";
                    return  result;
                }
                Magick::Blob profile;
                switch(result.colorspace) {
                case FXX::RGBColorSpace:
                    profile = Magick::Blob(failsafe.iccRGB.data(),
                                           failsafe.iccRGB.size());
                    result.iccInputBuffer = failsafe.iccRGB;
                    break;
                case FXX::CMYKColorSpace:
                    profile = Magick::Blob(failsafe.iccCMYK.data(),
                                           failsafe.iccCMYK.size());
                    result.iccInputBuffer = failsafe.iccCMYK;
                    break;
                case FXX::GRAYColorSpace:
                    profile = Magick::Blob (failsafe.iccGRAY.data(),
                                            failsafe.iccGRAY.size());
                    result.iccInputBuffer = failsafe.iccGRAY;
                    break;
                default:;
                }
                if (profile.length()>0) {
                    image.profile("ICC", profile);
                } else {
                    result.error = "No input profile!";
                    return result;
                }
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
            unsigned char *imgBuffer = reinterpret_cast<unsigned char*>(const_cast<void*>(output.data()));
            std::vector<unsigned char> imgData(imgBuffer, imgBuffer + output.length());
            result.imageBuffer = imgData;

            // make a preview
            if (image.depth()>8) { image.depth(8); }
            image.magick("BMP");
            image.write(&preview);
            unsigned char *preBuffer = reinterpret_cast<unsigned char*>(const_cast<void*>(preview.data()));
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

FXX::Image FXX::convertImage(FXX::Image input, bool getInfo)
{
    FXX::Image result;
    if (input.imageBuffer.size()>0 &&
        input.iccInputBuffer.size()>0)
    {
        Magick::Image image;
        try {
            Magick::Blob tmp(input.imageBuffer.data(),
                             input.imageBuffer.size());
            image.read(tmp); // read image
        }
        catch(Magick::Error &error_ ) {
            result.error.append(error_.what());
            return result;
        }
        catch(Magick::Warning &warn_ ) {
            result.warning.append(warn_.what());
        }
        try {
            // change bit depth
            if (input.depth>0) {
               //image.channelDepth(Magick::ChannelType::AllChannels, input.depth);
                image.depth(input.depth);
            }

            // strip existing profiles from image
            image.profile("ICC", Magick::Blob());
            image.profile("ICM", Magick::Blob());

            // set rendering intent and blackpoint
            if (input.intent != FXX::UndefinedRenderingIntent) {
                switch (input.intent) {
                case FXX::SaturationRenderingIntent:
                    image.renderingIntent(Magick::SaturationIntent);
                    break;
                case FXX::PerceptualRenderingIntent:
                    image.renderingIntent(Magick::PerceptualIntent);
                    break;
                case FXX::AbsoluteRenderingIntent:
                    image.renderingIntent(Magick::AbsoluteIntent);
                    break;
                case FXX::RelativeRenderingIntent:
                    image.renderingIntent(Magick::RelativeIntent);
                    break;
                default:;
                }
            }
            image.blackPointCompensation(input.blackpoint);

            // apply source color profile
            Magick::Blob sourceProfile(input.iccInputBuffer.data(),
                                       input.iccInputBuffer.size());
            image.profile("ICC", sourceProfile);

            // apply destination color profile (if any)
            if (input.iccOutputBuffer.size()>0) {
                Magick::Blob destinationProfile(input.iccOutputBuffer.data(),
                                                input.iccOutputBuffer.size());
                image.profile("ICC", destinationProfile);
                result.iccInputBuffer = input.iccOutputBuffer;
            } else {
                result.iccInputBuffer = input.iccInputBuffer;
            }

            // write image
            result.filename = input.filename;
            Magick::Blob output;
            image.write(&output);
            unsigned char *imgBuffer = reinterpret_cast<unsigned char*>(const_cast<void*>(output.data()));
            std::vector<unsigned char> imgData(imgBuffer, imgBuffer + output.length());
            result.imageBuffer = imgData;

            // make preview
            Magick::Blob preview;
            if (input.iccMonitorBuffer.size()>0) {
                // apply monitor color profile (if any)
                Magick::Blob monitorProfile(input.iccMonitorBuffer.data(),
                                            input.iccMonitorBuffer.size());
                image.profile("ICC", monitorProfile);
            }
            if (image.depth()>8) { image.depth(8); }
            image.magick("BMP");
            image.write(&preview);
            unsigned char *preBuffer = reinterpret_cast<unsigned char*>(const_cast<void*>(preview.data()));
            std::vector<unsigned char> preData(preBuffer, preBuffer + preview.length());
            result.previewBuffer = preData;

            // get image stats
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
    } else {
        result.error.append("Missing image or ICC profiles, unable to convert.");
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
        case FXX::ICCManufacturer:
            cmsSelectedType = cmsInfoManufacturer;
            break;
        case FXX::ICCModel:
            cmsSelectedType = cmsInfoModel;
            break;
        case FXX::ICCCopyright:
            cmsSelectedType = cmsInfoCopyright;
            break;
        default:
            cmsSelectedType = cmsInfoDescription;
        }
        size = cmsGetProfileInfoASCII(profile, cmsSelectedType,
                                      "en", "US", nullptr, 0);
        if (size > 0) {
            std::vector<char> buffer(size);
            cmsUInt32Number newsize = cmsGetProfileInfoASCII(profile, cmsSelectedType,
                                          "en", "US", &buffer[0], size);
            if (size == newsize) {
                result = buffer.data();
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
                                                   static_cast<cmsUInt32Number>(buffer.size())),tag);
    }
    return "";
}

FXX::ColorSpace FXX::getProfileColorspace(std::vector<unsigned char> buffer)
{
    if (buffer.size()>0) {
        return getProfileColorspace(cmsOpenProfileFromMem(buffer.data(),
                                                          static_cast<cmsUInt32Number>(buffer.size())));
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

std::string FXX::supportedQuantumDepth()
{
    return MagickQuantumDepth;
}

std::string FXX::backendInfo()
{
    std::string result;
    result.append("<p>Powered by ");
    result.append("<a href=\"http://www.imagemagick.org\">");
    result.append(MagickPackageName);
    result.append("</a> ");
    result.append(MagickQuantumDepth);
    result.append(" ");
    result.append(MagickLibVersionText);
    result.append("<br>");
    result.append(MagickCopyright);
    result.append("<br>");
    result.append("ImageMagick is distributed under the <a href=\"https://www.imagemagick.org/script/license.php\">Apache 2.0 license</a>.<br>");
    result.append("<small><i>");
    result.append(MagickCore::GetMagickFeatures());
    result.append(" ");
    result.append(MagickCore::GetMagickDelegates());
    result.append("</i></small>");
    result.append("</p>");
    result.append("<p>Powered by <a href=\"http://www.littlecms.com\">Little CMS</a> ");
    result.append("&copy; Marti Maria Saguer. All rights reserved.<br>");
    result.append("LCMS is distributed under the <a href=\"http://www.opensource.org/licenses/mit-license.php\">MIT license</a>.");
    result.append("</p>");
    return  result;
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
    data.iccMonitorBuffer.clear();
    data.imageBuffer.clear();
    data.previewBuffer.clear();
    data.workBuffer.clear();
    data.iccCMYK.clear();
    data.iccGRAY.clear();
    data.iccRGB.clear();
    data.channels = 0;
    data.colorspace = FXX::UnknownColorSpace;
    data.depth = 0;
    data.height = 0;
    data.modified.clear();
    data.warning.clear();
    data.format.clear();
}

bool FXX::saveImage(FXX::Image data)
{
    if (data.imageBuffer.size()==0 || data.filename.empty()) {
        return false;
    }
    Magick::Image image;
    try {
        Magick::Blob buffer(data.imageBuffer.data(),
                            data.imageBuffer.size());
        if (buffer.length()==0) { return false; }
        image.read(buffer);
    }
    catch(Magick::Error &error_ ) {
        std::cout << "save image error!" << error_.what() << std::endl;
        return false;
    }
    catch(Magick::Warning &warn_ ) {
        std::cout << "save image warning! " << warn_.what() << std::endl;
    }
    if (image.columns() == 0 && image.rows() == 0) { return false; }
    try {
        image.write(data.filename);
    }
    catch(Magick::Error &error_ ) {
        std::cout << "save image error!" << error_.what() << std::endl;
        return false;
    }
    catch(Magick::Warning &warn_ ) {
        std::cout << "save image warning! " << warn_.what() << std::endl;
    }
    return true;
}

bool FXX::hasJPEG()
{
    std::string result = MagickCore::GetMagickDelegates();
    if (result.find("jpeg") != std::string::npos) { return true; }
    return false;
}

bool FXX::hasPNG()
{
    std::string result = MagickCore::GetMagickDelegates();
    if (result.find("png") != std::string::npos) { return true; }
    return false;
}

bool FXX::hasTIFF()
{
    std::string result = MagickCore::GetMagickDelegates();
    if (result.find("tiff") != std::string::npos) { return true; }
    return false;
}

bool FXX::hasLCMS()
{
    std::string result = MagickCore::GetMagickDelegates();
    if (result.find("lcms") != std::string::npos) { return true; }
    return false;
}

bool FXX::hasHDRI()
{
    std::string result = MagickCore::GetMagickFeatures();
    if (result.find("HDRI") != std::string::npos) { return true; }
    return false;
}

bool FXX::hasMP()
{
    std::string result = MagickCore::GetMagickFeatures();
    if (result.find("OpenMP") != std::string::npos) { return true; }
    return false;
}
