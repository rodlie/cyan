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

#include "common.h"

#include <QDebug>
#include <QFile>
#include <magick/MagickCore.h>

#define RESOURCE_BYTE 1050000000

Common::Common(QObject *parent): QObject (parent)
{
}

QString Common::timestamp()
{
    QDateTime date;
    return date.currentDateTime().toString("yyyyMMddHHmmsszzz");
}

QMap<Magick::CompositeOperator, QString> Common::compositeModes()
{
    QMap<Magick::CompositeOperator, QString> result;

    result[MagickCore::OverCompositeOp] = QString("Normal");
    result[MagickCore::OverlayCompositeOp] = QString("Overlay");
    result[MagickCore::ScreenCompositeOp] = QString("Screen");
    result[MagickCore::DissolveCompositeOp] = QString("Dissolve");
    result[MagickCore::MultiplyCompositeOp] = QString("Multiply");
    result[MagickCore::PlusCompositeOp] = QString("Addition");
    result[MagickCore::MinusCompositeOp] = QString("Minus");
    result[MagickCore::DivideCompositeOp] = QString("Divide");
    result[MagickCore::SubtractCompositeOp] = QString("Subtract");
    result[MagickCore::DifferenceCompositeOp] = QString("Difference");
    result[MagickCore::XorCompositeOp] = QString("XOR");
    result[MagickCore::ColorBurnCompositeOp] = QString("Burn");
    result[MagickCore::ColorDodgeCompositeOp] = QString("Dodge");
    result[MagickCore::HardLightCompositeOp] = QString("Hard light");
    result[MagickCore::LightenCompositeOp] = QString("Lighten only");
    result[MagickCore::LightenIntensityCompositeOp] = QString("Lighten intensity");
    result[MagickCore::DarkenCompositeOp] = QString("Darken only");
    result[MagickCore::DarkenIntensityCompositeOp] = QString("Darken intensity");
    result[MagickCore::VividLightCompositeOp] = QString("Vivid light");
    result[MagickCore::PinLightCompositeOp] = QString("Pin light");
    result[MagickCore::SoftLightCompositeOp] = QString("Soft light");
    result[MagickCore::PegtopLightCompositeOp] = QString("Pegtop light");
    result[MagickCore::CopyBlackCompositeOp] = QString("Copy Black");
    result[MagickCore::CopyBlueCompositeOp] = QString("Copy Blue");
    result[MagickCore::CopyCyanCompositeOp] = QString("Copy Cyan");
    result[MagickCore::CopyGreenCompositeOp] = QString("Copy Green");
    result[MagickCore::CopyMagentaCompositeOp] = QString("Copy Magenta");
    result[MagickCore::CopyRedCompositeOp] = QString("Copy Red");
    result[MagickCore::CopyYellowCompositeOp] = QString("Copy Yellow");
    result[MagickCore::HueCompositeOp] = QString("Hue");
    result[MagickCore::SaturateCompositeOp] = QString("Saturation");
    result[MagickCore::MinusSrcCompositeOp] = QString("Substract");
    result[MagickCore::DivideSrcCompositeOp] = QString("Divide");
    result[MagickCore::LinearBurnCompositeOp] = QString("Burn (linear)");
    result[MagickCore::LinearDodgeCompositeOp] = QString("Dodge (linear)");
    result[MagickCore::ThresholdCompositeOp] =QString("Threshold");
    result[MagickCore::ReplaceCompositeOp] = QString("Replace");
    result[MagickCore::ModulateCompositeOp] = QString("Modulate");
    result[MagickCore::LuminizeCompositeOp] = QString("Luminize");
    result[MagickCore::LinearLightCompositeOp] = QString("Linear light");
    result[MagickCore::HardMixCompositeOp] = QString("Hard mix");
    result[MagickCore::ExclusionCompositeOp] = QString("Exclusion");

    return result;
}

Magick::CompositeOperator Common::compositeModeFromString(const QString &name)
{
    QMapIterator<Magick::CompositeOperator, QString> i(Common::compositeModes());
    while (i.hasNext()) {
        i.next();
        if (i.value() == name) { return i.key(); }
    }
    return Magick::UndefinedCompositeOp;
}

Magick::Image Common::compLayers(Magick::Image canvas,
                                 QMap<int, Common::Layer> layers,
                                 Magick::Geometry crop)
{
    // copy canvas to comp
    Magick::Image comp(canvas);
    comp.quiet(true);

    // crop comp
    if (crop.width()>0) {
        try {
            comp.crop(crop);
            comp.repage();
        }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
    }

    QMapIterator<int, Common::Layer> i(layers);
    while (i.hasNext()) {
        i.next();

        // layer
        Magick::Image layer(i.value().image);

        // skip if layer not visible or broken
        if (!i.value().visible || !layer.isValid()) { continue; }

        layer.quiet(true);

        // get layer offset
        int offsetX = i.value().pos.width();
        int offsetY = i.value().pos.height();

        // crop
        if (crop.width()>0) {
            // set page to canvas and position layer accordingly
            layer.page(Magick::Geometry(canvas.columns(),
                                        canvas.rows(),
                                        offsetX,
                                        offsetY));

            // crop layer to tile
            bool ignore = false;
            try { layer.crop(crop); }
            catch(Magick::Error &error_ ) {
                qWarning() << error_.what();
                ignore = true;
            }
            catch(Magick::Warning &warn_ ) {
                qWarning() << warn_.what();
                ignore = true;
            }

            if (ignore) { continue; }

            // repage layer
            layer.repage();

            // calculate layer offset
            if (offsetX>crop.xOff()) { offsetX = static_cast<int>(offsetX-crop.xOff()); }
            else { offsetX =0; }
            if (offsetY>crop.yOff()) { offsetY = static_cast<int>(offsetY-crop.yOff()); }
            else { offsetY = 0;}
        }

        // set layer opacity
        if (i.value().opacity<1) {
            try {
                layer.matte(true);
                layer.quantumOperator(Magick::AlphaChannel,
                                      Magick::MultiplyEvaluateOperator,
                                      i.value().opacity);
            }
            catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
            catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
        }

        // set layer composite operator
        Magick::CompositeOperator layerComp = i.value().composite;

        // composite current layer over previous comp
        /*if (i.value().composite != Magick::OverCompositeOp &&
                i.value().composite != Magick::DstOutCompositeOp &&
                i.value().composite != Magick::XorCompositeOp) {
                try {
                    Magick::Image clone(comp);
                    clone.quiet(true);
                    clone.composite(layer,
                                    offsetX,
                                    offsetY,
                                    i.value().composite);
                    clone.matte(false);
                    layer = clone;
                    offsetX=0;
                    offsetY=0;
                    // override layer composite operator
                    layerComp = Magick::InCompositeOp;
                }
                catch(Magick::Error &error_ ) {
                    //emit errorMessage(error_.what());
                    qDebug() << error_.what();
                }
                catch(Magick::Warning &warn_ ) {
                    //emit warningMessage(warn_.what());
                    qDebug() << warn_.what();
                }
            }*/
        // comp layer over canvas
        try {
            comp.quiet(true);
            comp.composite(layer,
                           offsetX,
                           offsetY,
                           layerComp);
            //comp.repage();
        }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
    }
    return comp;
}

const QString Common::canvasWindowTitle(Magick::Image image)
{
    // filename @ colorspace widthxheight dpixdpi bit
    QString label = QObject::tr("New image");
    if (!QString::fromStdString(image.label()).isEmpty()) {
        label = QString::fromStdString(image.label());
    }
    QString colorspace;
    switch(image.colorSpace()) {
    case Magick::RGBColorspace:
    case Magick::sRGBColorspace:
    case Magick::scRGBColorspace:
    case Magick::TransparentColorspace:
        colorspace = "RGB";
        break;
    case Magick::CMYKColorspace:
        colorspace = "CMYK";
        break;
    case Magick::CMYColorspace:
        colorspace = "CMY";
        break;
    case Magick::GRAYColorspace:
        colorspace = "GRAY";
        break;
    case MagickCore::LinearGRAYColorspace:
        colorspace = "LinearGRAY";
        break;
    case Magick::LabColorspace:
        colorspace = "LAB";
        break;
    case Magick::HSBColorspace:
        colorspace = "HSB";
        break;
    case Magick::HSLColorspace:
        colorspace = "HSL";
        break;
    case Magick::Rec601LumaColorspace:
        colorspace = "Rec601Luma";
        break;
    case Magick::Rec709LumaColorspace:
        colorspace = "Rec709Luma";
        break;
    case Magick::Rec601YCbCrColorspace:
        colorspace = "Rec601YCbCr";
        break;
    case Magick::Rec709YCbCrColorspace:
        colorspace = "Rec709YCbCr";
        break;
    default:
        colorspace = QObject::tr("Other");
    }
    QString result;
    try {
        int width = static_cast<int>(image.columns());
        int height = static_cast<int>(image.rows());
        int depth = static_cast<int>(image.depth());
        int dpiX = static_cast<int>(image.density().width());
        int dpiY = static_cast<int>(image.density().height());
        result = QString("%1 @ %2 %3x%4 %5x%6 %7-bit")
                        .arg(label)
                        .arg(colorspace)
                        .arg(width)
                        .arg(height)
                        .arg(dpiX)
                         .arg(dpiY)
                        .arg(depth);
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    return result;
}

int Common::getDiskResource()
{
    return qRound(static_cast<double>(Magick::ResourceLimits::disk()/RESOURCE_BYTE));
}

void Common::setDiskResource(int gib)
{
    try {
        Magick::ResourceLimits::disk(static_cast<qulonglong>(gib)*static_cast<qulonglong>(RESOURCE_BYTE));
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
}

int Common::getMemoryResource()
{
    return qRound(static_cast<double>(Magick::ResourceLimits::memory()/RESOURCE_BYTE));
}

void Common::setMemoryResource(int gib)
{
    try {
        Magick::ResourceLimits::memory(static_cast<qulonglong>(gib)*static_cast<qulonglong>(RESOURCE_BYTE));
        Magick::ResourceLimits::map(static_cast<qulonglong>(gib)*static_cast<qulonglong>(RESOURCE_BYTE));
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
}

void Common::setThreadResources(int thread)
{
    Magick::ResourceLimits::thread(static_cast<qulonglong>(thread));
}

bool Common::writeCanvas(Common::Canvas canvas,
                         const QString &filename,
                         Magick::CompressionType compress)
{
    if (filename.isEmpty() || !canvas.image.isValid()) { return false; }

    // setup project
    std::list<Magick::Image> images;
    Magick::Image image(canvas.image);
    image.magick("MIFF");

    // set label on image
    try {
        image.label(canvas.label.toStdString());
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    // mark as cyan project
    try {
        image.attribute(QString(CYAN_PROJECT).toStdString(),
                        QString("%1").arg(CYAN_PROJECT_VERSION)
                        .toStdString());
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    // compress(?)
    try { image.compressType(compress); }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    // add canvas as first image
    images.push_back(image);

    // add layers
    QMapIterator<int, Common::Layer> layers(canvas.layers);
    while (layers.hasNext()) {
        layers.next();
        Magick::Image layer(layers.value().image);
        layer.magick("MIFF");

        // add label
        try {
            layer.label(layers.value().label.toStdString());
        }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // add position
        try {
            layer.attribute(QString(CYAN_LAYER_X).toStdString(),
                            QString("%1").arg(layers.value().pos.width())
                            .toStdString());
            layer.attribute(QString(CYAN_LAYER_Y).toStdString(),
                            QString("%1").arg(layers.value().pos.height())
                            .toStdString());
        }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // mark as cyan layer
        try {
            layer.attribute(QString(CYAN_LAYER).toStdString(),
                            QString("%1").arg(CYAN_LAYER_VERSION)
                            .toStdString());
        }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // add compose mode
        try {
            layer.attribute(QString(CYAN_LAYER_COMPOSE).toStdString(),
                            QString("%1").arg(Common::compositeModes()[layers.value().composite])
                            .toStdString());
        }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // add visibility
        try {
            layer.attribute(QString(CYAN_LAYER_VISIBILITY).toStdString(),
                            QString("%1").arg(layers.value().visible)
                            .toStdString());
        }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // add opacity
        try {
            layer.attribute(QString(CYAN_LAYER_OPACITY).toStdString(),
                            QString("%1").arg(layers.value().opacity)
                            .toStdString());
        }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // compress(?)
        try { layer.compressType(compress); }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // add to project
        images.push_back(layer);
    }

    // write project file
    try {
        Magick::writeImages(images.begin(),
                            images.end(),
                            filename.toStdString());
    }
    catch(Magick::Error &error_ ) {
        qWarning() << error_.what();
        return false;
    }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    if (QFile::exists(filename)) { return true; }
    // TODO add more checks
    return false;
}

Common::Canvas Common::readCanvas(const QString &filename)
{
    Common::Canvas canvas;

    if (filename.isEmpty()) { return canvas; }

    // read images
    std::list<Magick::Image> images;
    try {
        Magick::readImages(&images,
                           filename.toStdString());
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    for (std::list<Magick::Image>::iterator it = images.begin(); it != images.end(); ++it){
        double project = QString::fromStdString(it->attribute(QString(CYAN_PROJECT)
                                                              .toStdString())).toDouble();
        if (project>0) { // this is our canvas
            canvas.image = *it;

            // decompress
            try { canvas.image.compressType(Magick::NoCompression); }
            catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
            catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

            // set label
            canvas.label = QString::fromStdString(it->label());
            continue;
        }
        double layer = QString::fromStdString(it->attribute(QString(CYAN_LAYER)
                                                            .toStdString())).toDouble();
        if (layer>0) { // add layer

            // get opacity
            double opacity = QString::fromStdString(it->attribute(QString(CYAN_LAYER_OPACITY)
                                                                  .toStdString())).toDouble();

            // get visibility
            int visibility = QString::fromStdString(it->attribute(QString(CYAN_LAYER_VISIBILITY)
                                                                  .toStdString())).toInt();

            // get position
            int offX = QString::fromStdString(it->attribute(QString(CYAN_LAYER_X)
                                                            .toStdString())).toInt();
            int offY = QString::fromStdString(it->attribute(QString(CYAN_LAYER_Y)
                                                            .toStdString())).toInt();
            QSize pos = QSize(offX, offY);

            // get composite
            Magick::CompositeOperator compose = Common::compositeModeFromString(QString::fromStdString(it->attribute(QString(CYAN_LAYER_COMPOSE).toStdString())));
            Common::Layer layer;

            // setup layer
            layer.image = *it;
            layer.label = QString::fromStdString(layer.image.label());
            layer.visible = visibility;
            layer.pos = pos;
            layer.opacity = opacity;
            layer.composite = compose;

            // decompress
            try { layer.image.compressType(Magick::NoCompression); }
            catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
            catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

            canvas.layers.insert(canvas.layers.size(), layer);
        }
    }
    return canvas;
}

Magick::Image Common::renderCanvasToImage(Common::Canvas canvas)
{
    return Common::compLayers(canvas.image,
                              canvas.layers);
}

bool Common::renderCanvasToFile(Common::Canvas canvas,
                                const QString &filename,
                                Magick::CompressionType compress,
                                QMap<QString, QString> attr,
                                QMap<QString, QString> arti)
{
    // render canvas
    Magick::Image image = Common::compLayers(canvas.image,
                                             canvas.layers);

    // TODO add profiles if missing

    // set compression type
    try { image.compressType(compress); }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    // add attributes
    try {
        QMapIterator<QString, QString> i(attr);
        while (i.hasNext()) {
            i.next();
            image.attribute(i.key().toStdString(),
                            i.value().toStdString());
        }
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    // add artifacts
    try {
        QMapIterator<QString, QString> y(arti);
        while (y.hasNext()) {
            y.next();
            image.artifact(y.key().toStdString(),
                           y.value().toStdString());
        }
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    // write to file
    try { image.write(filename.toStdString()); }
    catch(Magick::Error &error_ ) {
        qWarning() << error_.what();
        return false;
    }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    if (QFile::exists(filename)) { return true; }
    return false;
}

bool Common::isValidCanvas(const QString &filename)
{
    if (filename.isEmpty()) { return false; }

    try {
        Magick::Image image;
        image.quiet(true);
        image.ping(filename.toStdString());
        double project = QString::fromStdString(image.attribute(QString(CYAN_PROJECT)
                                                                .toStdString())).toDouble();
        if (project>0) { return true; }
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
    return false;
}

Common::Canvas Common::readImage(const QString &filename)
{
    Common::Canvas canvas;
    try {
        Magick::Image image;
        image.quiet(false);
        image.read(filename.toStdString());
        image.magick("MIFF");
        image.fileName(filename.toStdString());
        if (image.columns()>0 && image.rows()>0) {
            canvas.image = image;
        }
    }
    catch(Magick::Error &error_ ) {
        canvas.error = QString::fromStdString(error_.what());
    }
    catch(Magick::Warning &warn_ ) {
        canvas.warning = QString::fromStdString(warn_.what());
    }
    return canvas;
}

int Common::supportedQuantumDepth()
{
    return QString(MagickQuantumDepth)
           .replace(QString("Q"), QString(""))
            .toInt();
}

bool Common::supportsJpeg()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("jpeg", Qt::CaseSensitive);
}

bool Common::supportsPng()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("png", Qt::CaseSensitive);
}

bool Common::supportsTiff()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("tiff", Qt::CaseSensitive);
}

bool Common::supportsLcms()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("lcms", Qt::CaseSensitive);
}

bool Common::supportsHdri()
{
    return QString(QString::fromStdString(MagickCore::GetMagickFeatures()))
            .contains("HDRI", Qt::CaseSensitive);
}

bool Common::supportsOpenMP()
{
    return QString(QString::fromStdString(MagickCore::GetMagickFeatures()))
            .contains("OpenMP", Qt::CaseSensitive);
}

bool Common::supportsBzlib()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("bzlib", Qt::CaseSensitive);
}

bool Common::supportsCairo()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("cairo", Qt::CaseSensitive);
}

bool Common::supportsFontConfig()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("fontconfig", Qt::CaseSensitive);
}

bool Common::supportsFreeType()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("freetype", Qt::CaseSensitive);
}

bool Common::supportsJP2()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("jp2", Qt::CaseSensitive);
}

bool Common::supportsLzma()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("lzma", Qt::CaseSensitive);
}

bool Common::supportsOpenExr()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("openexr", Qt::CaseSensitive);
}

bool Common::supportsPangoCairo()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("pangocairo", Qt::CaseSensitive);
}

bool Common::supportsRaw()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("raw", Qt::CaseSensitive);
}

bool Common::supportsRsvg()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("rsvg", Qt::CaseSensitive);
}

bool Common::supportsWebp()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("webp", Qt::CaseSensitive);
}

bool Common::supportsXml()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("xml", Qt::CaseSensitive);
}

bool Common::supportsZlib()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("zlib", Qt::CaseSensitive);
}
