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

#include "imageloader.h"

ImageLoader::ImageLoader(QObject *parent) : QObject(parent)
{
    moveToThread(&t);
    t.start();
}

ImageLoader::~ImageLoader()
{
    t.quit();
    t.wait();
}

void ImageLoader::requestImage(const QString &file, const FXX::Image &failsafe)
{
    QMetaObject::invokeMethod(this, "readImage",
                              Q_ARG(QString, file),
                              Q_ARG(FXX::Image, failsafe));
}

void ImageLoader::requestConvert(const FXX::Image &image)
{
    QMetaObject::invokeMethod(this, "convertImage", Q_ARG(FXX::Image, image));
}

void ImageLoader::readImage(const QString &file, const FXX::Image &failsafe)
{
    FXX::Image imageData;
    if (!file.isEmpty()) {
        //std::vector<unsigned char> profile(failsafe.begin(), failsafe.end());
        imageData = fx.readImage(file.toStdString(), failsafe);
    }
    emit loadedImage(imageData);
}

void ImageLoader::convertImage(const FXX::Image &image)
{
    emit convertedImage(fx.convertImage(image));
}
