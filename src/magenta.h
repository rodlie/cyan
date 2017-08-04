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

#ifndef MAGENTA_H
#define MAGENTA_H

#include <QObject>
#include "yellow.h"
#include <Magick++.h>
#include <QByteArray>
#include <QFile>
#include <QDebug>
#include <QStringList>
#include <QThread>

#define COLOR_FILTERS_URL "https://raw.githubusercontent.com/olear/clut/master"

struct magentaImage {
    QByteArray data;
    QByteArray profile;
    QByteArray thumb;
    QString error;
    QString warning;
    QString filename;
    bool preview = false;
    bool saved = false;
    bool hasProfile = false;
    int colorspace = 0;
    int width = 0;
    int height = 0;
    double inkDensity = 0.0;
    double cyanMin = -1.0;
    double cyanMax = -1.0;
    double magentaMin = -1.0;
    double magentaMax = -1.0;
    double yellowMin = -1.0;
    double yellowMax = -1.0;
    double blackMin = -1.0;
    double blackMax = -1.0;
    QString category;
};Q_DECLARE_METATYPE(magentaImage)

struct magentaAdjust {
    double brightness = 100.0;
    double saturation = 100.0;
    double hue = 100.0;
    int intent = 0;
    bool black = false;
    int depth = 0;
    double cmyLevel = 0.0;
    double kLevel = 0.0;
    QString clut;
};Q_DECLARE_METATYPE(magentaAdjust)

struct magentaInfo {
    int width = 0;
    int height = 0;
    QString error;
    QString warning;
};Q_DECLARE_METATYPE(magentaInfo)

class Magenta : public QObject
{
    Q_OBJECT
public:
    explicit Magenta(QObject *parent = 0);
    ~Magenta();

signals:
    void returnImage(magentaImage result);
    void returnColorPreview(magentaImage result);
    void returnThumbnail(magentaImage result);

public slots:
    void requestImage(bool isPreview, bool doSave, QString file, QByteArray data, QByteArray inprofile, QByteArray outprofile, QByteArray monitorprofile, magentaAdjust edit);
    magentaImage readImage(bool isPreview, bool doSave, QString file, QByteArray data, QByteArray inprofile, QByteArray outprofile, QByteArray monitorprofile, magentaAdjust edit);
    QString version();
    bool supportBlackPoint();
    int quantumDepth();
    QString colorFiltersPath();
    void requestColorPreview(QString file, QString category, QByteArray data);
    magentaImage makeColorPreview(QString file, QString category, QByteArray data);
    magentaInfo getImageInfo(QString file);

private slots:
    Magick::Image colorFilter(Magick::Image image, QString hald);

private:
    Yellow yellow;
    QThread t;
};

#endif // MAGENTA_H
