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
#include <QDebug>

ImageLoader::ImageLoader(QObject *parent) : QObject(parent)
  , loading(false)
  , converting(false)
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

bool ImageLoader::isLoading()
{
    return loading;
}

bool ImageLoader::isConverting()
{
    return converting;
}

void ImageLoader::readImage(const QString &file, const FXX::Image &failsafe)
{
    FXX::Image imageData;
    if (!file.isEmpty() /*&& !loading && !converting*/) {
        qDebug() << "Loading image" << file << QString::fromStdString(failsafe.filename);
        loading = true;
        imageData = fx.readImage(file.toStdString(), failsafe);
        loading = false;
    }
    emit loadedImage(imageData);
}

void ImageLoader::convertImage(const FXX::Image &image)
{
    FXX::Image result;
    if (image.imageBuffer.size()>0 /*&& !loading && !converting*/) {
        qDebug() << "Converting image" << QString::fromStdString(image.filename);
        converting = true;
        result= fx.convertImage(image);
        converting = false;
    }
    emit convertedImage(result);
}
