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

struct magentaImage {
    QByteArray data;
    QByteArray profile;
    QString error;
    QString warning;
    QString filename;
    bool preview;
    bool saved;
    int colorspace;
    int width;
    int height;
};Q_DECLARE_METATYPE(magentaImage)

struct magentaAdjust {
    double brightness;
    double saturation;
    double hue;
    int intent;
    bool black;
};Q_DECLARE_METATYPE(magentaAdjust)

class Magenta : public QObject
{
    Q_OBJECT
public:
    explicit Magenta(QObject *parent = 0);
    ~Magenta();
    signals:
    void returnImage(magentaImage result);

    public slots:
    void requestImage(bool isPreview, bool doSave, QString file, QByteArray data, QByteArray inprofile, QByteArray outprofile, QByteArray monitorprofile, magentaAdjust edit);
    magentaImage readImage(bool isPreview, bool doSave, QString file, QByteArray data, QByteArray inprofile, QByteArray outprofile, QByteArray monitorprofile, magentaAdjust edit);

private:
    Yellow yellow;
    QThread t;
};

#endif // MAGENTA_H
