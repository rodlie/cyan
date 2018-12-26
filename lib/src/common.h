/*
# Copyright Ole-André Rodlie.
#
# ole.andre.rodlie@gmail.com
#
# This software is governed by the CeCILL license under French law and
# abiding by the rules of distribution of free software. You can use,
# modify and / or redistribute the software under the terms of the CeCILL
# license as circulated by CEA, CNRS and INRIA at the following URL
# "https://www.cecill.info".
#
# As a counterpart to the access to the source code and rights to
# modify and redistribute granted by the license, users are provided only
# with a limited warranty and the software's author, the holder of the
# economic rights and the subsequent licensors have only limited
# liability.
#
# In this respect, the user's attention is drawn to the associated risks
# with loading, using, modifying and / or developing or reproducing the
# software by the user in light of its specific status of free software,
# that can mean that it is complicated to manipulate, and that also
# so that it is for developers and experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to test and test the software's suitability
# Requirements in the conditions of their systems
# data to be ensured and, more generally, to use and operate
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL license and that you accept its terms.
*/

#ifndef COMMON_H
#define COMMON_H

#include <QObject>
#include <QMap>
#include <QDateTime>
#include <QMenu>

#include <list>
#include <lcms2.h>
#include <Magick++.h>

#ifndef NO_FFMPEG
extern "C" {
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
}
#endif

#include "tileitem.h"

#define CYAN_PROJECT_VERSION 1.0
#define CYAN_LAYER_VERSION 1.0

#define CYAN_PROJECT "cyan-project"
#define CYAN_LAYER "cyan-layer"

#define CYAN_LAYER_VISIBILITY "cyan-visibility"
#define CYAN_LAYER_OPACITY "cyan-opacity"
#define CYAN_LAYER_COMPOSE "cyan-compose"
#define CYAN_LAYER_X "cyan-layer-x"
#define CYAN_LAYER_Y "cyan-layer-y"


class Common: public QObject
{
    Q_OBJECT

public:

    enum MoveLayer {
        MoveLayerUp,
        MoveLayerDown,
        MoveLayerLeft,
        MoveLayerRight
    };

    enum ICCTag {
        ICCDescription,
        ICCManufacturer,
        ICCModel,
        ICCCopyright
    };

    enum Colorspace
    {
        NoColorspace,
        RGBColorspace,
        CMYKColorspace,
        GRAYColorspace
    };

    enum newDialogType
    {
        newImageDialogType,
        newLayerDialogType
    };

    enum RenderingIntent
    {
        UndefinedRenderingIntent,
        SaturationRenderingIntent,
        PerceptualRenderingIntent,
        AbsoluteRenderingIntent,
        RelativeRenderingIntent
    };

    struct Tile
    {
        TileItem *rect;
    };

    struct Layer
    {
        Magick::Image image;
        QMap<int, Common::Layer> layers;
        Magick::CompositeOperator composite = Magick::OverCompositeOp;
        QSize pos = QSize(0, 0);
        bool visible = true;
        double opacity = 1.0;
        QString label = QObject::tr("New Layer");
    };

    struct Canvas
    {
        Magick::Image image;
        QMap<int, Common::Layer> layers;
        QString error;
        QString warning;
        QString label = QObject::tr("New Image");
        QMap<int, Common::Tile> tiles;
        QSize tileSize;
        QColor brushColor;
        bool brushAA = true;
        Magick::LineCap brushLineCap = Magick::RoundCap;
        Magick::LineJoin brushLineJoin = Magick::MiterJoin;
        QString timestamp;
        Magick::Blob profile;
    };

    Common(QObject *parent = nullptr);

    static QString timestamp();

    static QMap<Magick::CompositeOperator, QString> compositeModes();
    static Magick::CompositeOperator compositeModeFromString(const QString &name);

    static Magick::Image compLayers(Magick::Image canvas,
                                    QMap<int, Common::Layer> layers,
                                    Magick::Geometry crop = Magick::Geometry());

    static const QString canvasWindowTitle(Magick::Image image);

    static int getDiskResource();
    static void setDiskResource(int gib);

    static int getMemoryResource();
    static void setMemoryResource(int gib);

    static void setThreadResources(int thread);

    static bool writeCanvas(Common::Canvas canvas,
                            const QString &filename,
                            Magick::CompressionType compress = Magick::LZMACompression);
    static Common::Canvas readCanvas(const QString &filename);

    static Magick::Image renderCanvasToImage(Common::Canvas canvas);
    static bool renderCanvasToFile(Common::Canvas canvas,
                            const QString &filename,
                            Magick::CompressionType compress = Magick::NoCompression,
                            QMap<QString, QString> attr = QMap<QString, QString>(),
                            QMap<QString, QString> arti = QMap<QString, QString>());

    static bool isValidCanvas(const QString &filename);
    static bool isValidImage(const QString &filename);

    static int hasLayers(const QString &filename);

    static Common::Canvas readImage(const QString &filename);

    static Magick::Image convertColorspace(Magick::Image image,
                                           Magick::Blob input,
                                           Magick::Blob output,
                                           Magick::RenderingIntent intent = Magick::PerceptualIntent,
                                           bool blackpoint = true);
    static Magick::Image convertColorspace(Magick::Image image,
                                           const QString &input,
                                           const QString &output,
                                           Magick::RenderingIntent intent = Magick::PerceptualIntent,
                                           bool blackpoint = true);
    static Magick::Image convertColorspace(Magick::Image image,
                                           Magick::Blob input,
                                           const QString &output,
                                           Magick::RenderingIntent intent = Magick::PerceptualIntent,
                                           bool blackpoint = true);
    static Magick::Image convertColorspace(Magick::Image image,
                                           const QString &input,
                                           Magick::Blob output,
                                           Magick::RenderingIntent intent = Magick::PerceptualIntent,
                                           bool blackpoint = true);

    static QStringList getColorProfilesPath();
    static QMap<QString, QString> getColorProfiles(Magick::ColorspaceType colorspace);

    static Magick::ColorspaceType getProfileColorspace(const QString &filename);
    static Magick::ColorspaceType getProfileColorspace(cmsHPROFILE profile);

    static const QString getProfileTag(const QString filename,
                                       Common::ICCTag tag = Common::ICCDescription);
    static const QString getProfileTag(cmsHPROFILE profile,
                                       Common::ICCTag tag = Common::ICCDescription);

    static const QString supportedWriteFormats();
    static const QString supportedReadFormats();
    static int supportedQuantumDepth();
    static bool supportsJpeg();
    static bool supportsPng();
    static bool supportsTiff();
    static bool supportsLcms();
    static bool supportsHdri();
    static bool supportsOpenMP();
    static bool supportsBzlib();
    static bool supportsCairo();
    static bool supportsFontConfig();
    static bool supportsFreeType();
    static bool supportsJP2();
    static bool supportsLzma();
    static bool supportsOpenExr();
    static bool supportsPangoCairo();
    static bool supportsRaw();
    static bool supportsRsvg();
    static bool supportsWebp();
    static bool supportsXml();
    static bool supportsZlib();
    static bool supportsJng();

    static const QString humanFileSize(float num,
                                       bool mp = false,
                                       bool are = false);


    /*void populateColorProfileMenu(QMenu *menu,
                                  Magick::ColorspaceType colorspace);
    void setDefaultColorProfileFromFilename(QMenu *menu,
                                            const QString &filename);
    void setDefaultColorProfileFromTitle(QMenu *menu,
                                         const QString &title);
    Magick::Blob selectedDefaultColorProfileData(QMenu *menu);

    const QString selectedDefaultColorProfile(QMenu *menu);

signals:
    void errorMessage(const QString &message);
    void warningMessage(const QString &message);*/


#ifndef NO_FFMPEG
    static QByteArray getEmbeddedCoverArt(const QString &filename);
    static int getVideoMaxFrames(const QString &filename);
    static Magick::Image getVideoFrame(const QString &filename,
                                       int frame = 0);
#endif
};

#endif // COMMON_H
