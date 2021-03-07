/*
#
# Cyan <https://cyan.fxarena.net>
# Copyright Ole-André Rodlie, FxArena DA.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
*/

#ifndef CYANIMAGEFORMAT_H
#define CYANIMAGEFORMAT_H

#include "CyanGlobal.h"
#include "CyanTileItem.h"
#include "CyanGuideItem.h"

#include <QString>
#include <QSize>
#include <QMap>
#include <QColor>
#include <QPair>
#include <QUuid>

#include <Magick++.h>

#define CYAN_IMAGE_FORMAT "cyan-image-format"
#define CYAN_IMAGE_V_GUIDES "cyan-image-v_guides"
#define CYAN_IMAGE_H_GUIDES "cyan-image-h_guides"

#define CYAN_LAYER "cyan-layer"
#define CYAN_LAYER_LABEL "cyan-layer-label"
#define CYAN_LAYER_VISIBILITY "cyan-layer-visibility"
#define CYAN_LAYER_OPACITY "cyan-layer-opacity"
#define CYAN_LAYER_COMPOSE "cyan-layer-compose"
#define CYAN_LAYER_LOCK "cyan-layer-lock"
#define CYAN_LAYER_X "cyan-layer-x"
#define CYAN_LAYER_Y "cyan-layer-y"
#define CYAN_LAYER_ORDER "cyan-layer-order"

#define CYAN_LAYER_TEXT "cyan-layer-text"
#define CYAN_LAYER_TEXT_HTML "cyan-layer-text-html"
#define CYAN_LAYER_TEXT_MARKUP "cyan-layer-text-markup"
#define CYAN_LAYER_TEXT_ALIGN "cyan-layer-text-align"
#define CYAN_LAYER_TEXT_ROTATE "cyan-layer-text-rotate"

class CYANSHARED_EXPORT CyanImageFormat
{
public:
    enum CyanTextAlign
    {
        CyanTextAlignLeft,
        CyanTextAlignRight,
        CyanTextAlignCenter
    };
    enum CyanICCTag {
        CyanICCDescription,
        CyanICCManufacturer,
        CyanICCModel,
        CyanICCCopyright
    };
    enum CyanColorspace
    {
        CyanNoColorspace,
        CyanRGBColorspace,
        CyanCMYKColorspace,
        CyanGRAYColorspace
    };
    enum CyanRenderingIntent
    {
        CyanUndefinedRenderingIntent,
        CyanSaturationRenderingIntent,
        CyanPerceptualRenderingIntent,
        CyanAbsoluteRenderingIntent,
        CyanRelativeRenderingIntent
    };
    struct CyanBrush
    {
        QColor color;
        bool antialiasing = true;
        Magick::LineCap lineCap = Magick::RoundCap;
        Magick::LineJoin lineJoin = Magick::MiterJoin;
    };
    struct CyanTile
    {
        CyanTileItem *rect;
    };
    struct CyanGuide
    {
        CyanGuideItem *rect;
    };
    struct CyanEffect
    {
        QString id;
        QMap<QString, QVariant> values;
    };
    struct CyanLayer
    {
        Magick::Image image;
        QMap<int, CyanLayer> layers;
        QMap<int, CyanEffect> effects;
        Magick::CompositeOperator composite = Magick::OverCompositeOp;
        QSize position = QSize(0, 0);
        bool visible = true;
        bool locked = false;
        double opacity = 1.0;
        QString label;
        int order = -1;
        QString html;
        QString markup;
        bool isText = false;
        int textAlign = CyanTextAlignLeft;
        int textRotate = 0;
    };
    struct CyanCanvas
    {
        Magick::Image image;
        QMap<int, CyanLayer> layers;
        QString error;
        QString warning;
        QString label;
        QMap<int, CyanTile> tiles;
        QSize tileSize;
        CyanBrush brush;
        Magick::Blob profile;
        bool dirty = true;
        QMap<int, CyanGuideItem*> guides;
        QString filename;
        QString uid = QUuid::createUuid().toString();
    };
    struct QPairSortFirst
    {
        template<typename T1, typename T2>
        bool operator()(const QPair<T1,T2> &a,
                        const QPair<T1,T2> &b) const
        {
            return a.first < b.first;
        }
    };
    CyanImageFormat();
    static const QString getVersion();
    static QMap<Magick::CompositeOperator, QPair<QString, int> > compositeModes();
    static Magick::CompositeOperator findCompositeMode(const QString &title,
                                                       int id = -1);
    static bool isValidCanvas(const QString &filename);
    static bool isValidImage(const QString &filename);
    static int hasLayers(const QString &filename);
    static bool writeCanvas(CyanCanvas canvas,
                            const QString &filename,
                            Magick::CompressionType compress = Magick::NoCompression);
    static bool writePSD(CyanCanvas canvas,
                         const QString &filename,
                         Magick::CompressionType compress = Magick::NoCompression);
    static CyanCanvas readCanvas(const QString &filename);
    static CyanCanvas readUnknownCanvas(const QString &filename);
    static CyanCanvas readImage(const QString &filename);
    static Magick::Image renderText(CyanLayer layer);
    static Magick::Image compLayers(Magick::Image canvas,
                                    QMap<int, CyanLayer> layers,
                                    Magick::Geometry crop = Magick::Geometry());
    static Magick::Image renderCanvasToImage(CyanCanvas canvas);
    static bool renderCanvasToFile(CyanCanvas canvas,
                                   const QString &filename,
                                   Magick::CompressionType compress = Magick::NoCompression,
                                   QMap<QString, QString> attr = QMap<QString, QString>(),
                                   QMap<QString, QString> arti = QMap<QString, QString>());
    static const QString supportedWriteFormats();
    static const QString supportedReadFormats();
    static const QString supportedImageFormats(bool readFormats);
    static bool isBlacklistedReadFormat(const QString &readFormat);
    static bool isBlacklistedWriteFormat(const QString &writeFormat);
    static int supportedQuantumDepth();
    static bool hasDelegate(const QString &delegate);
    static bool hasFeature(const QString &feature);
    static const QString html2Pango(const QString &html);
    //static const QString canvasTitleWithSpecs(Magick::Image image);
};

#endif // CYANIMAGEFORMAT_H
