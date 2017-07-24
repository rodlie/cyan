/*
* Cyan <https://github.com/olear/cyan>,
* Copyright (C) 2016 Ole-André Rodlie
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
#include <magick/version.h>

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
    Magick::Blob outputImage;
    QByteArray outputProfile;
    int outputColorSpace;
    Magick::Image image;
    try {
        if (!file.isEmpty() && !doSave ) {
            image.read(file.toUtf8().data());
        } else {
            Magick::Blob imageData(data.data(),data.length());
            image.read(imageData);
        }

        if (image.iccColorProfile().length() > 0) {
            result.hasProfile = true;
        } else {
            result.hasProfile = false;
        }

        if (edit.depth == 8 || edit.depth == 16 || edit.depth == 32) {
            image.depth(edit.depth);
        }

#ifdef MAGICK7
        if (!image.alpha()) {
            image.alpha(true);
        }
#else
        if (!image.matte()) {
            image.matte(true);
        }
#endif

        switch(image.colorSpace()) {
        case Magick::CMYKColorspace:
            outputColorSpace=2;
            break;
        case Magick::GRAYColorspace:
            outputColorSpace=3;
            break;
        case Magick::RGBColorspace:
            outputColorSpace=1;
            break;
        case Magick::sRGBColorspace:
            outputColorSpace=1;
            break;
        case Magick::TransparentColorspace:
            outputColorSpace=1;
            break;
        default:
            outputColorSpace=0;
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
            }
        }

#ifndef NOBLACKPOINT
        if (edit.black) {
            image.blackPointCompensation(edit.black);
        }
#endif

        if (inprofile.length() > 0) {
            Magick::Blob sourceProfile(inprofile.data(), inprofile.length());
            image.profile(PROFILE,sourceProfile);
        }
        if (outprofile.length() > 0) {
            Magick::Blob destProfile(outprofile.data(), outprofile.length());
            image.profile(PROFILE,destProfile);
        }
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
            QString comment = QCoreApplication::applicationName() + " " + QCoreApplication::applicationVersion() + " https://github.com/olear/cyan";
            image.comment(comment.toStdString());
            image.write(file.toUtf8().data());
            result.saved = true;
        } else {
            result.saved = false;
            image.strip();
            image.write(&outputImage);
        }
    }
    catch(Magick::Error &error_ ) {
        result.error.append(error_.what());
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
    result.append("ImageMagick is licenced under the <a href=\"https://www.imagemagick.org/script/license.php\">Apache 2.0 license</a>.<br>");
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
