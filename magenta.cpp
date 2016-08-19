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
            image.read(file.toAscii().data());
        } else {
            Magick::Blob imageData(data.data(),data.length());
            image.read(imageData);
        }

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

        if (edit.intent>0) {
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

        if (edit.black) {
            image.blackPointCompensation(edit.black);
        }

        if (edit.brightness!=100 || edit.saturation!=100 || edit.hue!=100) {
            image.modulate(edit.brightness,edit.saturation,edit.hue);
        }

        if (inprofile.length()>0) {
            Magick::Blob sourceProfile(inprofile.data(),inprofile.length());
            image.profile("ICC",sourceProfile); // use ICM in GM and ICC in IM
        }
        if (outprofile.length()>0) {
            Magick::Blob destProfile(outprofile.data(),outprofile.length());
            image.profile("ICC",destProfile); // use ICM in GM and ICC in IM
        }
        if (monitorprofile.length()>0 && isPreview) {
            Magick::Blob proofProfile(monitorprofile.data(),monitorprofile.length());
            image.profile("ICC",proofProfile); // use ICM in GM and ICC in IM
        }

        outputProfile = QByteArray((char*)image.iccColorProfile().data(),image.iccColorProfile().length());

        if (isPreview) {
            result.preview = true;
        }
        else {
            result.preview = false;
        }

        if (isPreview && !doSave) {
            image.depth(8);
            image.magick("TIF");
        }

        if (doSave) {
            image.magick("TIF");
            image.write(file.toAscii().data());
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
        if (outputImage.length()>0) {
            result.data = QByteArray((char*)outputImage.data(),outputImage.length());
        }

        if (outputProfile.length()>0) {
            result.profile = QByteArray((char*)outputProfile.data(),outputProfile.length());
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
