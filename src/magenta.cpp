/*
* Cyan <http://cyan.fxarena.net> <https://github.com/olear/cyan>,
* Copyright (C) 2016, 2017 Ole-André Rodlie
*
* Cyan is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as published
* by the Free Software Foundation.
*
* Cyan is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Cyan.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>
*/

#include "magenta.h"
#include <QCoreApplication>
#include <QDir>
//#include <magick/version.h>
//#include <magick/statistic.h>
#ifdef MAGICK7
#include <MagickCore/MagickCore.h>
#include <MagickWand/MagickWand.h>
#endif
#include <QDebug>

#ifdef GMAGICK
#define PROFILE "ICM"
#define NOBLACKPOINT
#else
#define PROFILE "ICC"
#endif

Magenta::Magenta(QObject *parent) :
    QObject(parent)
{
    Magick::InitializeMagick(NULL);
    moveToThread(&t);
    t.start();
}

Magenta::~Magenta()
{
    t.quit();
    t.wait();
}

void Magenta::requestImage(bool isPreview, bool doSave, QString file, QByteArray data, QByteArray inprofile, QByteArray outprofile, QByteArray monitorprofile, magentaAdjust edit)
{
    QMetaObject::invokeMethod(this,"readImage", Q_ARG(bool, isPreview), Q_ARG(bool, doSave), Q_ARG(QString, file), Q_ARG(QByteArray, data), Q_ARG(QByteArray, inprofile), Q_ARG(QByteArray, outprofile), Q_ARG(QByteArray, monitorprofile), Q_ARG(magentaAdjust, edit));
}

magentaImage Magenta::readImage(bool isPreview, bool doSave, QString file, QByteArray data, QByteArray inprofile, QByteArray outprofile, QByteArray monitorprofile, magentaAdjust edit)
{
    magentaImage result;
    //result.inkDensity = 0.0;
    Magick::Blob outputImage;
    QByteArray outputProfile;
    int outputColorSpace = 0;
    Magick::Image image;
    try {
        if (!file.isEmpty() && !doSave ) {
            image.read(file.toUtf8().data());
        } else {
            Magick::Blob imageData(data.data(),data.length());
            image.read(imageData);
        }
    }
    catch(Magick::Error &error_ ) {
        result.error.append(error_.what());
        return result;
    }
    catch(Magick::Warning &warn_ ) {
        result.warning.append(warn_.what());
    }

    try {
        if (image.iccColorProfile().length() > 0) {
            result.hasProfile = true;
        } else {
            result.hasProfile = false;
        }

        if (edit.depth == 8 || edit.depth == 16 || edit.depth == 32) {
            image.depth(edit.depth);
        }

/*
#ifdef MAGICK7
        if (!image.alpha()) {
            image.alpha(true);
        }
#else
        if (!image.matte()) {
            image.matte(true);
        }
#endif
*/
        switch(image.colorSpace()) {
        case Magick::CMYKColorspace:
            outputColorSpace = CMYK_COLORSPACE;
            break;
        case Magick::GRAYColorspace:
            outputColorSpace = GRAY_COLORSPACE;
            break;
        case Magick::RGBColorspace:
            outputColorSpace = RGB_COLORSPACE;
            break;
        case Magick::sRGBColorspace:
            outputColorSpace = RGB_COLORSPACE;
            break;
        case Magick::TransparentColorspace:
            outputColorSpace = RGB_COLORSPACE;
            break;
        default:
            outputColorSpace = NO_COLORSPACE;
        }
        result.colorspace = outputColorSpace;

        result.width = (int)image.columns();
        result.height = (int)image.rows();

        if (edit.brightness!=100 || edit.saturation!=100 || edit.hue!=100) {
            image.modulate(edit.brightness, edit.saturation, edit.hue);
        }

        if (edit.intent > 0) {
            switch(edit.intent) {
            case 1:
                image.renderingIntent(Magick::SaturationIntent);
                break;
            case 2:
                image.renderingIntent(Magick::PerceptualIntent);
                break;
            case 3:
                image.renderingIntent(Magick::AbsoluteIntent);
                break;
            case 4:
                image.renderingIntent(Magick::RelativeIntent);
            }
        }

#ifndef NOBLACKPOINT
        if (edit.black) {
            image.blackPointCompensation(edit.black);
        }
#endif

        // Apply source profile
        if (inprofile.length() > 0) {
            Magick::Blob sourceProfile(inprofile.data(), inprofile.length());
            image.profile(PROFILE,sourceProfile);
        }

        // apply hald if source is RGB
        bool addedHald = false;
        if (outputColorSpace == RGB_COLORSPACE && !edit.clut.isEmpty() && !colorFiltersPath().isEmpty()) {
            image = colorFilter(image, edit.clut);
            addedHald = true;
        }

        // Apply destination profile
        if (outprofile.length() > 0) {
            Magick::Blob destProfile(outprofile.data(), outprofile.length());
            image.profile(PROFILE,destProfile);
        }

        // apply hald if source is CMYK, only works if destination is RGB
        if (!addedHald && outputColorSpace == CMYK_COLORSPACE && image.colorSpace() != Magick::CMYKColorspace && !edit.clut.isEmpty() && !colorFiltersPath().isEmpty()) {
            image = colorFilter(image, edit.clut);
            addedHald = true;
        }

        if (image.colorSpace() == Magick::CMYKColorspace) {
            if (edit.cmyLevel>0.0 && edit.cmyLevel <=100.0) {
                double value = edit.cmyLevel/100;
                QString fxValue = "u+";
                fxValue.append(QString::number(value));
                image.fx(fxValue.toStdString().c_str(), Magick::CyanChannel);
                image.fx(fxValue.toStdString().c_str(), Magick::MagentaChannel);
                image.fx(fxValue.toStdString().c_str(), Magick::YellowChannel);
/*
#ifdef MAGICK7
                image.evaluate(Magick::CyanChannel, Magick::AddEvaluateOperator,  value);
                image.evaluate(Magick::MagentaChannel, Magick::AddEvaluateOperator,  value);
                image.evaluate(Magick::YellowChannel, Magick::AddEvaluateOperator,  value);
#else
                image.quantumOperator(Magick::CyanChannel, Magick::AddEvaluateOperator, value);
                image.quantumOperator(Magick::MagentaChannel, Magick::AddEvaluateOperator, value);
                image.quantumOperator(Magick::YellowChannel, Magick::AddEvaluateOperator, value);
#endif
*/
            }
            if (edit.kLevel >0.0 && edit.kLevel <=100.0) {
                double value = edit.kLevel/100;
                QString fxValue = "u-";
                fxValue.append(QString::number(value));
                image.fx(fxValue.toStdString().c_str(), Magick::BlackChannel);
/*
#ifdef MAGICK7
                image.evaluate(Magick::BlackChannel, Magick::SubtractEvaluateOperator, value);
#else
                image.quantumOperator(Magick::BlackChannel, Magick::SubtractEvaluateOperator, value);
#endif
*/
            }
#ifdef MAGICK7
            /// broken in v7
            Magick::ExceptionInfo exception;
            result.inkDensity = 100.0 * MagickCore::GetImageTotalInkDensity(image.image(), &exception) / (double)QuantumRange;

            Magick::ImageStatistics stats = image.statistics();
            Magick::ChannelStatistics cyan = stats.channel(Magick::CyanPixelChannel);
            Magick::ChannelStatistics magenta = stats.channel(Magick::MagentaPixelChannel);
            Magick::ChannelStatistics yellow = stats.channel(Magick::YellowPixelChannel);
            Magick::ChannelStatistics black = stats.channel(Magick::BlackPixelChannel);

            qDebug() << "cyan channel min max" << cyan.minima()) / (double)QuantumRange << ((double)QuantumRange-cyan.maxima())/ (double)QuantumRange;
            qDebug() << "magenta channel min max" << magenta.minima()) / (double)QuantumRange << magenta.maxima()) / (double)QuantumRange;
            qDebug() << "yellow channel min max" << yellow.minima()) / (double)QuantumRange << yellow.maxima()) / (double)QuantumRange;
            qDebug() << "black channel min max" << black.minima()) / (double)QuantumRange << black.maxima()) / (double)QuantumRange;
#else
            result.inkDensity = 100.0 * MagickCore::GetImageTotalInkDensity(image.image()) / (double)QuantumRange;

            MagickCore::ExceptionInfo exception;
            MagickCore::ChannelStatistics *stats = MagickCore::GetImageChannelStatistics(image.image(), &exception);
            result.cyanMin = stats[MagickCore::CyanChannel].minima/(double)QuantumRange;
            result.cyanMax = stats[MagickCore::CyanChannel].maxima/(double)QuantumRange;
            result.magentaMin = stats[MagickCore::MagentaChannel].minima/(double)QuantumRange;
            result.magentaMax = stats[MagickCore::MagentaChannel].maxima/(double)QuantumRange;
            result.yellowMin = stats[MagickCore::YellowChannel].minima/(double)QuantumRange;
            result.yellowMax = stats[MagickCore::YellowChannel].maxima/(double)QuantumRange;
            result.blackMin = stats[MagickCore::BlackChannel].minima/(double)QuantumRange;
            result.blackMax = stats[MagickCore::BlackChannel].maxima/(double)QuantumRange;
            if (result.cyanMin < 0) {
                result.cyanMin = 0;
            }
            if (result.magentaMin < 0) {
                result.magentaMin = 0;
            }
            if (result.yellowMin < 0) {
                result.yellowMin = 0;
            }
            if (result.blackMin < 0) {
                result.blackMin = 0;
            }
            if (result.cyanMax > 1) {
                result.cyanMax = 1;
            }
            if (result.magentaMax > 1) {
                result.magentaMax = 1;
            }
            if (result.yellowMax > 1) {
                result.yellowMax = 1;
            }
            if (result.blackMax > 1) {
                result.blackMax = 1;
            }
            free(stats);
#endif
        }

        // Apply monitor profile at the end (if preview)
        if (monitorprofile.length() > 0 && isPreview) {
            Magick::Blob proofProfile(monitorprofile.data(), monitorprofile.length());
            image.profile(PROFILE,proofProfile);
        }

        outputProfile = QByteArray((char*)image.iccColorProfile().data(), image.iccColorProfile().length());

        if (isPreview) {
            result.preview = true;
        } else {
            result.preview = false;
        }

        if (isPreview && !doSave) {
            image.depth(8);
            image.magick("TIF");
        }

        if (doSave) {
            image.magick("TIF");
            QString comment = QCoreApplication::applicationName() + " " + QCoreApplication::applicationVersion() + " <http://cyan.fxarena.net>";
            image.comment(comment.toStdString());
            image.write(file.toUtf8().data());
            result.saved = true;
        } else {
            image.strip();
            image.write(&outputImage);
        }
    }
    catch(Magick::Error &error_ ) {
        result.error.append(error_.what());
        return result;
    }
    catch(Magick::Warning &warn_ ) {
        result.warning.append(warn_.what());
    }

    if (!doSave) {
        if (outputImage.length() > 0) {
            result.data = QByteArray((char*)outputImage.data(), outputImage.length());
        }
        if (outputProfile.length() > 0) {
            result.profile = QByteArray((char*)outputProfile.data(), outputProfile.length());
        } else {
            result.profile = yellow.profileDefault(outputColorSpace);
        }
    }

    if (!file.isEmpty()) {
        result.filename = file;
    }

    // thumb
    if (!doSave && !isPreview) {
        try {
            Magick::Blob thumb;
            Magick::Image t_image;
            t_image = image;
            t_image.depth(8);
            t_image.strip();
            t_image.scale(Magick::Geometry(200,200));
            if (t_image.colorSpace() == Magick::CMYKColorspace) {
                if (inprofile.length() > 0) {
                    Magick::Blob sourceProfile(inprofile.data(), inprofile.length());
                    t_image.profile(PROFILE,sourceProfile);
                } else {
                    QByteArray srcPro = yellow.profileDefault(CMYK_COLORSPACE);
                    Magick::Blob sourceProfile(srcPro.data(), srcPro.length());
                    t_image.profile(PROFILE,sourceProfile);
                }
                QByteArray thumbProfileData = yellow.profileDefault(RGB_COLORSPACE);
                Magick::Blob thumbProfile(thumbProfileData.data(), thumbProfileData.length());
                t_image.profile(PROFILE,thumbProfile);
            }
            t_image.write(&thumb);
            result.thumb = QByteArray((char*)thumb.data(), thumb.length());
        }
        catch(Magick::Error &error_ ) {
            result.error.append(error_.what());
        }
        catch(Magick::Warning &warn_ ) {
            result.warning.append(warn_.what());
        }
    }

    emit returnImage(result);
    return result;
}

QString Magenta::version()
{
    QString result;
    result.append("<p>Powered by ");
    result.append("<a href='");
#ifdef GMAGICK
    result.append("http://www.graphicsmagick.org");
#else
    result.append("http://www.imagemagick.org");
#endif
    result.append("'>");
    result.append(MagickPackageName);
    result.append("</a> ");
    result.append(MagickQuantumDepth);
    result.append(" ");
    result.append(MagickLibVersionText);
    result.append("<br>");
    result.append(MagickCopyright);
    result.append("<br>");
#ifndef GMAGICK
    result.append("ImageMagick is distributed under the <a href=\"https://www.imagemagick.org/script/license.php\">Apache 2.0 license</a>.<br>");
    QString magickFeatures = QString::fromStdString(MagickCore::GetMagickFeatures());
    QString magickDelegates = QString::fromStdString(MagickCore::GetMagickDelegates());
    result.append("<small><i>" + magickFeatures + " " + magickDelegates + "</i></small>");
#endif
    result.append("</p>");
    result.append("<p>Powered by <a href=\"http://www.littlecms.com\">Little CMS</a> ");
    result.append(QString::number(LCMS_VERSION).replace(1,1,".").replace(3,3,""));
    result.append("<br>");
    result.append("&copy; 2010-2016 Marti Maria Saguer. All rights reserved.<br>");
    result.append("LCMS is distributed under the <a href=\"http://www.opensource.org/licenses/mit-license.php\">MIT license</a>.");
    result.append("</p>");

    return result;
}

bool Magenta::supportBlackPoint()
{
#ifdef NOBLACKPOINT
    return false;
#else
    return true;
#endif
}

int Magenta::quantumDepth()
{
    QString depth = MagickQuantumDepth;
    if (depth == "Q8") {
        return 8;
    } else if (depth == "Q16") {
        return 16;
    } else if (depth == "Q32") {
        return 32;
    } else {
        return 0;
    }
}

QString Magenta::colorFiltersPath()
{
    QString result;
    result.append(QDir::homePath());
    result.append("/.config");
    QDir configDir(result);
    if (!configDir.exists(result)) {
        configDir.mkdir(result);
    }
    result.append("/");
    result.append(qApp->applicationName());
    QDir cyanDir(result);
    if (!cyanDir.exists(result)) {
        cyanDir.mkdir(result);
    }
    result.append("/looks");
    QDir looksDir(result);
    if (!looksDir.exists(result)) {
        looksDir.mkdir(result);
    }
    result.append("/");

    return result;
}

void Magenta::requestColorPreview(QString file, QString category, QByteArray data)
{
    QMetaObject::invokeMethod(this,"makeColorPreview", Q_ARG(QString, file), Q_ARG(QString, category), Q_ARG(QByteArray, data));
}

magentaImage Magenta::makeColorPreview(QString file, QString category, QByteArray data)
{
    magentaImage result;
    result.category = category;
    Magick::Blob outputImage;
    bool hasAlpha = false;
    if (!file.isEmpty() && data.length() > 0) {
        Magick::Image image;
        try {
            Magick::Blob imageData(data.data(),data.length());
            image.read(imageData);
#ifdef MAGICK7
            if (image.alpha()) {
#else
            if (image.matte()) {
#endif
                hasAlpha = true;
            } else {
#ifdef MAGICK7
                image.alpha(true);
#else
                image.matte(true);
#endif
            }
            if (!file.isEmpty() && !colorFiltersPath().isEmpty()) {
                QString clutPath = colorFiltersPath() + file;
                Magick::Image clut;
                clut.read(clutPath.toUtf8().data());
                if (!clut.matte()) {
                    clut.matte(true);
                }
                if (clut.depth()>0) {
                    image.haldClut(clut);
                }
            }

            if (!hasAlpha) {
#ifdef MAGICK7
                image.alpha(false);
#else
                image.matte(false);
#endif
            }

            if (image.depth()>8) {
                image.depth(8);
            }
            image.magick("TIF");
            image.write(&outputImage);
        }
        catch(Magick::Error &error_ ) {
            result.error.append(error_.what());
        }
        catch(Magick::Warning &warn_ ) {
            result.warning.append(warn_.what());
        }
        result.thumb = QByteArray((char*)outputImage.data(), outputImage.length());
        result.filename = file;
    }
    emit returnColorPreview(result);
    return result;
}

magentaInfo Magenta::getImageInfo(QString file)
{
    magentaInfo info;
    try {
        Magick::Image image;
        image.ping(file.toUtf8().data());
        info.width = (int)image.columns();
        info.height = (int)image.rows();
    }
    catch(Magick::Error &error_ ) {
        info.error.append(error_.what());
    }
    catch(Magick::Warning &warn_ ) {
        info.warning.append(warn_.what());
    }
    return info;
}

Magick::Image Magenta::colorFilter(Magick::Image image, QString hald)
{
    if (image.depth()>0 && !hald.isEmpty() && !colorFiltersPath().isEmpty()) {
        QString clutPath = colorFiltersPath() + hald;
        Magick::Image clut;
        clut.read(clutPath.toUtf8().data());
        if (clut.depth()>0) {
            bool hasAlpha = false;
#ifdef MAGICK7
            if (image.alpha()) {
#else
            if (image.matte()) {
#endif
                hasAlpha = true;
            } else {
#ifdef MAGICK7
                image.alpha(true);
#else
                image.matte(true);
#endif
            }
#ifdef MAGICK7
            if (!clut.alpha()) {
                clut.alpha(true);
            }
#else
            if (!clut.matte()) {
                clut.matte(true);
            }
#endif
            image.haldClut(clut);
            if (!hasAlpha) {
#ifdef MAGICK7
                image.alpha(false);
#else
                image.matte(false);
#endif
            }
        }
    }
    return image;
}
