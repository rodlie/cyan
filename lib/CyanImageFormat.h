/*
#
# Cyan Library <https://cyan.fxarena.net>
#
# Copyright Ole-André Rodlie, FxArena DA.
#
# ole.andre.rodlie@gmail.com / support@fxarena.net
#
# This software is governed either by the CeCILL or the CeCILL-C license
# under French law and abiding by the rules of distribution of free software.
# You can  use, modify and or redistribute the software under the terms of
# the CeCILL or CeCILL-C licenses as circulated by CEA, CNRS and INRIA
# at the following URL: "http://www.cecill.info".
#
# As a counterpart to the access to the source code and  rights to copy,
# modify and redistribute granted by the license, users are provided only
# with a limited warranty  and the software's author,  the holder of the
# economic rights,  and the successive licensors  have only  limited
# liability.
#
# In this respect, the user's attention is drawn to the risks associated
# with loading,  using,  modifying and/or developing or reproducing the
# software by the user in light of its specific status of free software,
# that may mean  that it is complicated to manipulate,  and  that  also
# therefore means  that it is reserved for developers  and  experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to load and test the software's suitability as regards their
# requirements in conditions enabling the security of their systems and/or
# data to be ensured and,  more generally, to use and operate it in the
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL and CeCILL-C licenses and that you accept its terms.
#
*/

#ifndef CYANIMAGEFORMAT_H
#define CYANIMAGEFORMAT_H

#include "CyanGlobal.h"
#include "CyanTileItem.h"

#include <QString>
#include <QSize>
#include <QMap>
#include <QColor>
#include <QPair>

#include <Magick++.h>

#define CYAN_IMAGE_FORMAT "cyan-image-format"

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
    struct CyanLayer
    {
        Magick::Image image;
        QMap<int, CyanLayer> layers;
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
    static CyanCanvas readCanvas(const QString &filename);
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
