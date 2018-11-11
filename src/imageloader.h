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

#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QObject>
#include <QThread>
#include <QString>
#include <QByteArray>
#include "FXX.h"

class ImageLoader : public QObject
{
    Q_OBJECT
public:
    explicit ImageLoader(QObject *parent = Q_NULLPTR);
    ~ImageLoader();

private:
    FXX fx;
    QThread t;

signals:
    void loadedImage(FXX::Image result);
    void convertedImage(FXX::Image result);

public slots:
    void requestImage(const QString &file, const FXX::Image &failsafe);
    void requestConvert(const FXX::Image &image);

private slots:
    void readImage(const QString &file, const FXX::Image &failsafe);
    void convertImage(const FXX::Image &image);
};

#endif // IMAGELOADER_H
