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
#include <QDir>
#include <QDirIterator>
#include <QAction>

//#include <magick/Magick.h>

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

    result[Magick::OverCompositeOp] = QString("Normal");
    result[Magick::OverlayCompositeOp] = QString("Overlay");
    result[Magick::ScreenCompositeOp] = QString("Screen");
    result[Magick::DissolveCompositeOp] = QString("Dissolve");
    result[Magick::MultiplyCompositeOp] = QString("Multiply");
    result[Magick::PlusCompositeOp] = QString("Addition");
    result[Magick::MinusSrcCompositeOp] = QString("Minus (src)");
    result[Magick::MinusDstCompositeOp] = QString("Minus (dst)");
    result[Magick::DivideSrcCompositeOp] = QString("Divide (src)");
    result[Magick::DivideDstCompositeOp] = QString("Divide (dst)");
    //result[Magick::SubtractCompositeOp] = QString("Subtract"); NOT IN IM7
    result[Magick::DifferenceCompositeOp] = QString("Difference");
    result[Magick::XorCompositeOp] = QString("XOR");
    result[Magick::ColorBurnCompositeOp] = QString("Burn");
    result[Magick::ColorDodgeCompositeOp] = QString("Dodge");
    result[Magick::HardLightCompositeOp] = QString("Hard light");
    result[Magick::LightenCompositeOp] = QString("Lighten only");
    result[Magick::LightenIntensityCompositeOp] = QString("Lighten intensity");
    result[Magick::DarkenCompositeOp] = QString("Darken only");
    result[Magick::DarkenIntensityCompositeOp] = QString("Darken intensity");
    result[Magick::VividLightCompositeOp] = QString("Vivid light");
    result[Magick::PinLightCompositeOp] = QString("Pin light");
    result[Magick::SoftLightCompositeOp] = QString("Soft light");
    result[Magick::PegtopLightCompositeOp] = QString("Pegtop light");
    result[Magick::CopyBlackCompositeOp] = QString("Copy Black");
    result[Magick::CopyBlueCompositeOp] = QString("Copy Blue");
    result[Magick::CopyCyanCompositeOp] = QString("Copy Cyan");
    result[Magick::CopyGreenCompositeOp] = QString("Copy Green");
    result[Magick::CopyMagentaCompositeOp] = QString("Copy Magenta");
    result[Magick::CopyRedCompositeOp] = QString("Copy Red");
    result[Magick::CopyYellowCompositeOp] = QString("Copy Yellow");
    result[Magick::HueCompositeOp] = QString("Hue");
    result[Magick::SaturateCompositeOp] = QString("Saturation");
    result[Magick::MinusSrcCompositeOp] = QString("Substract");
    result[Magick::DivideSrcCompositeOp] = QString("Divide");
    result[Magick::LinearBurnCompositeOp] = QString("Burn (linear)");
    result[Magick::LinearDodgeCompositeOp] = QString("Dodge (linear)");
    result[Magick::ThresholdCompositeOp] =QString("Threshold");
    result[Magick::ReplaceCompositeOp] = QString("Replace");
    result[Magick::ModulateCompositeOp] = QString("Modulate");
    result[Magick::LuminizeCompositeOp] = QString("Luminize");
    result[Magick::LinearLightCompositeOp] = QString("Linear light");
    result[Magick::HardMixCompositeOp] = QString("Hard mix");
    result[Magick::ExclusionCompositeOp] = QString("Exclusion");

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
                /*layer.matte(true);
                layer.quantumOperator(Magick::AlphaChannel,
                                      Magick::MultiplyEvaluateOperator,
                                      i.value().opacity);*/
                layer.alpha(true);
                layer.evaluate(Magick::AlphaChannel,
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
    case Magick::LinearGRAYColorspace:
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
        int dpiX = static_cast<int>(image.density().x());
        int dpiY = static_cast<int>(image.density().y());
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
    if (thread == 0) { return; }
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

bool Common::isValidImage(const QString &filename)
{
    try {
        Magick::Image image;
        image.quiet(false);
        image.ping(filename.toStdString());
        return image.isValid();
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
    return false;
}

int Common::hasLayers(const QString &filename)
{
    try {
        std::list<Magick::Image> layers;
        Magick::pingImages(&layers,
                           filename.toStdString());
        return static_cast<int>(layers.size());
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
    return 0;
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

Magick::Image Common::convertColorspace(Magick::Image image,
                                        Magick::Blob input,
                                        Magick::Blob output,
                                        Magick::RenderingIntent intent,
                                        bool blackpoint)
{
    if (output.length()>0) {
        try {
            image.quiet(true);
            image.renderingIntent(intent);
            image.blackPointCompensation(blackpoint);
            if (input.length()>0) { image.profile("ICC", input); }
            image.profile("ICC", output);
        }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
    }
    return image;
}

Magick::Image Common::convertColorspace(Magick::Image image,
                                        const QString &input,
                                        const QString &output,
                                        Magick::RenderingIntent intent,
                                        bool blackpoint)
{
    Magick::Blob blob1,blob2;
    Magick::Image readBlob1,readBlob2;
    try {
        readBlob1.read(input.toStdString());
        readBlob1.write(&blob1);
        readBlob2.read(output.toStdString());
        readBlob2.write(&blob2);
        if (blob1.length()>0 && blob2.length()>0) {
            return convertColorspace(image,
                                     blob1,
                                     blob2,
                                     intent,
                                     blackpoint);
        }
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
    return Magick::Image();
}

Magick::Image Common::convertColorspace(Magick::Image image,
                                        Magick::Blob input,
                                        const QString &output,
                                        Magick::RenderingIntent intent,
                                        bool blackpoint)
{
    Magick::Blob blob;
    Magick::Image readBlob;
    try {
        readBlob.read(output.toStdString());
        readBlob.write(&blob);
        if (blob.length()>0) {
            return convertColorspace(image,
                                     input,
                                     blob,
                                     intent,
                                     blackpoint);
        }
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
    return Magick::Image();
}

Magick::Image Common::convertColorspace(Magick::Image image,
                                        const QString &input,
                                        Magick::Blob output,
                                        Magick::RenderingIntent intent,
                                        bool blackpoint)
{
    Magick::Blob blob;
    Magick::Image readBlob;
    try {
        readBlob.read(input.toStdString());
        readBlob.write(&blob);
        if (blob.length()>0) {
            return convertColorspace(image,
                                     blob,
                                     output,
                                     intent,
                                     blackpoint);
        }
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
    return Magick::Image();
}

QStringList Common::getColorProfilesPath()
{
    QStringList folders;
    folders << QDir::rootPath() + "/WINDOWS/System32/spool/drivers/color";
    folders << "/Library/ColorSync/Profiles";
    folders << QDir::homePath() + "/Library/ColorSync/Profiles";
    folders << "/usr/share/color/icc";
    folders << "/usr/local/share/color/icc";
    folders << QDir::homePath() + "/.color/icc";
    folders << QDir::homePath() + "/.config/Cyan/icc";
    folders << QDir::homePath() + "/.config/FxArena/Cyan/icc";
    return folders;
}

QMap<QString, QString> Common::getColorProfiles(Magick::ColorspaceType colorspace)
{
    QMap<QString, QString> output;
    QStringList folders = getColorProfilesPath();
    for (int i = 0; i < folders.size(); ++i) {
        QStringList filter;
        filter << "*.icc" << "*.icm";
        QDirIterator it(folders.at(i), filter, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString iccFile = it.next();
            QString profile = getProfileTag(iccFile);
            if (iccFile.isEmpty() || profile.isEmpty()) { continue; }
            if (getProfileColorspace(iccFile)!= colorspace) { continue; }
            output[profile] = iccFile;
        }
    }
    return output;
}

Magick::ColorspaceType Common::getProfileColorspace(const QString &filename)
{
    if (!filename.isEmpty()) {
        return getProfileColorspace(cmsOpenProfileFromFile(filename.toStdString().c_str(), "r"));
    }
    return Magick::UndefinedColorspace;
}

Magick::ColorspaceType Common::getProfileColorspace(cmsHPROFILE profile)
{
    Magick::ColorspaceType result = Magick::UndefinedColorspace;
    if (profile) {
        if (cmsGetColorSpace(profile) == cmsSigRgbData) {
            result = Magick::sRGBColorspace;
        } else if (cmsGetColorSpace(profile) == cmsSigCmykData) {
            result = Magick::CMYKColorspace;
        } else if (cmsGetColorSpace(profile) == cmsSigGrayData) {
            result = Magick::GRAYColorspace;
        }
    }
    cmsCloseProfile(profile);
    return result;
}

const QString Common::getProfileTag(const QString filename,
                                    Common::ICCTag tag)
{
    if (!filename.isEmpty()) {
        return getProfileTag(cmsOpenProfileFromFile(filename.toStdString().c_str(), "r"), tag);
    }
    return QString();
}

const QString Common::getProfileTag(cmsHPROFILE profile,
                                    Common::ICCTag tag)
{
    std::string result;
    if (profile) {
        cmsUInt32Number size = 0;
        cmsInfoType cmsSelectedType;
        switch(tag) {
        case Common::ICCManufacturer:
            cmsSelectedType = cmsInfoManufacturer;
            break;
        case Common::ICCModel:
            cmsSelectedType = cmsInfoModel;
            break;
        case Common::ICCCopyright:
            cmsSelectedType = cmsInfoCopyright;
            break;
        default:
            cmsSelectedType = cmsInfoDescription;
        }
        size = cmsGetProfileInfoASCII(profile,
                                      cmsSelectedType,
                                      "en",
                                      "US",
                                      nullptr,
                                      0);
        if (size > 0) {
            std::vector<char> buffer(size);
            cmsUInt32Number newsize = cmsGetProfileInfoASCII(profile,
                                                             cmsSelectedType,
                                                             "en",
                                                             "US",
                                                             &buffer[0], size);
            if (size == newsize) {
                result = buffer.data();
            }
        }
    }
    cmsCloseProfile(profile);
    return QString::fromStdString(result);
}

const QString Common::supportedWriteFormats()
{
    // TODO
    QString result;
    if (supportsJpeg()) {
        result.append(QString("*.jpeg "));
        result.append(QString("*.jpg "));
    }
    if (supportsPng()) {
        result.append(QString("*.png "));
    }
    if (supportsTiff()) {
        result.append(QString("*.tiff "));
        result.append(QString("*.tif "));
    }
    result.append(QString("*.bmp "));
    return result;
}

const QString Common::supportedReadFormats()
{
    QString result;
    result.append(QString("*.miff "));
    result.append(QString("*.xcf "));
    result.append(QString("*.psd "));
    result.append(QString("*.bmp "));
    result.append(QString("*.gif "));
    result.append(QString("*.hdr "));
    result.append(QString("*.ico "));

    result.append(QString("*.k25 "));
    result.append(QString("*.kdc "));
    result.append(QString("*.orf "));
    result.append(QString("*.pcd "));
    result.append(QString("*.pcds "));
    result.append(QString("*.pct "));
    result.append(QString("*.pict "));
    result.append(QString("*.pix "));
    result.append(QString("*.pnm "));
    result.append(QString("*.ppm "));
    result.append(QString("*.rmf "));
    result.append(QString("*.rw2 "));
    result.append(QString("*.sgi "));
    result.append(QString("*.sr2 "));
    result.append(QString("*.srf "));
    result.append(QString("*.sun "));
    result.append(QString("*.xpm "));

    result.append(QString("*.cr2 "));
    result.append(QString("*.crw "));
    result.append(QString("*.dcr "));
    result.append(QString("*.dpx "));

    if (supportsOpenExr()) { result.append(QString("*.exr ")); }
    if (supportsJng()) { result.append(QString("*.jng ")); }
    if (supportsJpeg()) {
        result.append(QString("*.jpeg "));
        result.append(QString("*.jpg "));
    }
    if (supportsJP2()) {
        result.append(QString("*.j2c "));
        result.append(QString("*.j2k "));
        result.append(QString("*.jp2 "));
        result.append(QString("*.jpc "));
        result.append(QString("*.jpe "));
        result.append(QString("*.jpm "));
        result.append(QString("*.jps "));
        result.append(QString("*.pgx "));
    }
    if (supportsPng()) {
        result.append(QString("*.png "));
        result.append(QString("*.mng "));
    }
    if (supportsTiff()) {
        result.append(QString("*.tiff "));
        result.append(QString("*.tif "));
    }
    if (supportsLcms()) {
        result.append(QString("*.icc "));
        result.append(QString("*.icm "));
    }

    result.append(QString("*.mp3 "));
    result.append(QString("*.mp4 "));
    result.append(QString("*.mkv "));

    return  result;
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
            .contains("openjp2", Qt::CaseSensitive);
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

bool Common::supportsJng()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("jng", Qt::CaseSensitive);
}

const QString Common::humanFileSize(float num, bool mp, bool are)
{
    float byte = 1024.0;
    QStringList list;
    list << "KB";

    if (are) {
        byte = 1000.0;
        list << "MP" << "GP";
    } else if (mp) {
        byte = 1000.0;
        list << "MP";
    } else { list << "MB" << "GB" << "TB"; }

    QStringListIterator i(list);
    QString unit("bytes");

    while(num >= byte && i.hasNext()) {
        unit = i.next();
        num /= byte;
    }
    return QString().setNum(num,'f',2)+" "+unit;
}

#ifndef NO_FFMPEG
QByteArray Common::getEmbeddedCoverArt(const QString &filename)
{
    QByteArray result;
    if (!filename.isEmpty()) {
        AVCodecContext  *pCodecCtx;
        AVFormatContext *pFormatCtx = avformat_alloc_context();
        AVCodec * pCodec;

        qDebug() << "open media file";
        if (avformat_open_input(&pFormatCtx,filename.toUtf8().data(),
                                Q_NULLPTR,
                                Q_NULLPTR) != 0) { return result; }
        if (avformat_find_stream_info(pFormatCtx,
                                      Q_NULLPTR) < 0) { return result; }

        av_dump_format(pFormatCtx, 0, filename.toUtf8().data(), 0);
        int videoStream = -1;

        qDebug() << "get video stream";
        for (int i=0; i < static_cast<int>(pFormatCtx->nb_streams); i++) {
            if(pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                videoStream = i;
                break;
            }
        }
        if (videoStream == -1) { return result; }

        qDebug() << "find decoder";
        pCodec =avcodec_find_decoder(pFormatCtx->streams[videoStream]->codecpar->codec_id);
        pCodecCtx = avcodec_alloc_context3(Q_NULLPTR);
        if (pCodec == Q_NULLPTR || pCodecCtx == Q_NULLPTR) { return result; }
        if (avcodec_parameters_to_context(pCodecCtx,
                                          pFormatCtx->streams[videoStream]->codecpar)<0)
        { return result; }
        if (avcodec_open2(pCodecCtx,
                         pCodec,
                         Q_NULLPTR) < 0) { return result; }

        qDebug() << "check for embedded";
        if (pFormatCtx->streams[videoStream]->disposition == AV_DISPOSITION_ATTACHED_PIC) {
            AVPacket pkt = pFormatCtx->streams[videoStream]->attached_pic;
            if (pkt.size>0) {
                QByteArray attachedPix = QByteArray(reinterpret_cast<const char*>(pkt.data),
                                                    pkt.size);
                if (attachedPix.length()>0) {
                    qDebug() << "got embedded image!";
                    avcodec_close(pCodecCtx);
                    avformat_close_input(&pFormatCtx);
                    return attachedPix;
                }
            }
        }
        avcodec_close(pCodecCtx);
        avformat_close_input(&pFormatCtx);
    }
    return result;
}

int Common::getVideoMaxFrames(const QString &filename)
{
    int result = -1;
    if (filename.isEmpty()) { return result; }

    AVCodecContext  *pCodecCtx;
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    AVCodec * pCodec;

    qDebug() << "open media file";
    if (avformat_open_input(&pFormatCtx,filename.toUtf8().data(),
                            Q_NULLPTR,
                            Q_NULLPTR) != 0) { return result; }
    if (avformat_find_stream_info(pFormatCtx,
                                  Q_NULLPTR) < 0) { return result; }

    av_dump_format(pFormatCtx, 0, filename.toUtf8().data(), 0);
    int videoStream = -1;

    qDebug() << "get video stream";
    for (int i=0; i < static_cast<int>(pFormatCtx->nb_streams); i++) {
        if(pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }
    if (videoStream == -1) { return result; }

    qDebug() << "find decoder";
    pCodec =avcodec_find_decoder(pFormatCtx->streams[videoStream]->codecpar->codec_id);
    pCodecCtx = avcodec_alloc_context3(Q_NULLPTR);
    if (pCodec == Q_NULLPTR || pCodecCtx == Q_NULLPTR) { return result; }
    if (avcodec_parameters_to_context(pCodecCtx,
                                      pFormatCtx->streams[videoStream]->codecpar)<0)
    { return result; }
    if (avcodec_open2(pCodecCtx,
                     pCodec,
                     Q_NULLPTR) < 0) { return result; }

    double fps = av_q2d(pFormatCtx->streams[videoStream]->r_frame_rate);
    double dur = static_cast<double>(pFormatCtx->duration)/AV_TIME_BASE;
    result = qRound((dur*fps)/2);
    qDebug() << "max frame" << result;

    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
    return  result;
}

Magick::Image Common::getVideoFrame(const QString &filename,
                                    int frame)
{
    Magick::Image result;

    qDebug() << "getVideoFrame" << filename << frame;

    if (filename.isEmpty()) { return result; }

    AVCodecContext  *pCodecCtx;
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    AVCodec * pCodec;
    AVFrame *pFrame, *pFrameRGB;

    qDebug() << "open media file";
    if (avformat_open_input(&pFormatCtx,filename.toUtf8().data(),
                            Q_NULLPTR,
                            Q_NULLPTR) != 0) { return result; }
    if (avformat_find_stream_info(pFormatCtx,
                                  Q_NULLPTR) < 0) { return result; }

    av_dump_format(pFormatCtx, 0, filename.toUtf8().data(), 0);
    int videoStream = -1;

    qDebug() << "get video stream";
    for (int i=0; i < (int)pFormatCtx->nb_streams; i++) {
        //if (abortProc) { return result; }
        if(pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }
    if (videoStream == -1) { return result; }

    qDebug() << "find decoder";
    pCodec =avcodec_find_decoder(pFormatCtx->streams[videoStream]->codecpar->codec_id);
    pCodecCtx = avcodec_alloc_context3(Q_NULLPTR);
    if (pCodec == Q_NULLPTR || pCodecCtx == Q_NULLPTR) { return result; }
    if (avcodec_parameters_to_context(pCodecCtx,
                                      pFormatCtx->streams[videoStream]->codecpar)<0)
    { return result; }
    if (avcodec_open2(pCodecCtx,
                     pCodec,
                     Q_NULLPTR) < 0) { return result; }

    qDebug() << "setup frame";
    pFrame    = av_frame_alloc();
    pFrameRGB = av_frame_alloc();

    uint8_t *buffer;
    int numBytes;

    AVPixelFormat  pFormat = AV_PIX_FMT_BGR24;
    numBytes = av_image_get_buffer_size(pFormat,
                                        pCodecCtx->width,
                                        pCodecCtx->height,
                                        16);
    buffer = (uint8_t *) av_malloc(numBytes*sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGB->data,
                         pFrameRGB->linesize,
                         buffer,
                         pFormat,
                         pCodecCtx->width,
                         pCodecCtx->height,
                         1);

    qDebug() << "calculate frame to get";
    int res;
    int frameFinished;
    AVPacket packet;
    int currentFrame = 0;
    bool fetchFrame = false;
    double fps = av_q2d(pFormatCtx->streams[videoStream]->r_frame_rate);
    double dur = static_cast<double>(pFormatCtx->duration)/AV_TIME_BASE;
    int maxFrame = qRound((dur*fps)/2);
    qDebug() << "MAX" << maxFrame;
    if (frame>maxFrame) { frame = maxFrame; }

    qDebug() << "we need to get frame" << frame;
    int64_t seekT = (int64_t(frame) *
                     pFormatCtx->streams[videoStream]->r_frame_rate.den *
                     pFormatCtx->streams[videoStream]->time_base.den) /
                     (int64_t(pFormatCtx->streams[videoStream]->r_frame_rate.num) *
                     pFormatCtx->streams[videoStream]->time_base.num);
    if (av_seek_frame(pFormatCtx, videoStream, seekT, AVSEEK_FLAG_FRAME) >= 0) {
              av_init_packet(&packet);
              currentFrame = frame;
    }
    while((res = av_read_frame(pFormatCtx,&packet)>=0)) {
        //qDebug() << "at current frame" << currentFrame;
        if (currentFrame>=frame) { fetchFrame = true; }
        if (packet.stream_index == videoStream){
            if (!fetchFrame) {
                currentFrame++;
                continue;
            }

            //qDebug() << "get frame" << currentFrame;
            int ret = avcodec_send_packet(pCodecCtx, &packet);
            if (ret<0) { continue; }
            ret = avcodec_receive_frame(pCodecCtx, pFrame);
            if (ret>=0) { frameFinished = true; }
            else { continue; }

            if (frameFinished) {
                qDebug() << "extract image from frame" << currentFrame;
                struct SwsContext * img_convert_ctx;
                img_convert_ctx = sws_getCachedContext(Q_NULLPTR,
                                                       pCodecCtx->width,
                                                       pCodecCtx->height,
                                                       pCodecCtx->pix_fmt,
                                                       pCodecCtx->width,
                                                       pCodecCtx->height,
                                                       AV_PIX_FMT_BGR24,
                                                       SWS_BICUBIC,
                                                       Q_NULLPTR,
                                                       Q_NULLPTR,
                                                       Q_NULLPTR);
                sws_scale(img_convert_ctx,
                          ((AVFrame*)pFrame)->data,
                          ((AVFrame*)pFrame)->linesize,
                          0,
                          pCodecCtx->height,
                          ((AVFrame*)pFrameRGB)->data,
                          ((AVFrame*)pFrameRGB)->linesize);

                qDebug() << "prepare image frame for" << filename;
                try {
                    Magick::Image image((size_t)pFrame->width,
                                        (size_t)pFrame->height,
                                        "BGR",
                                        Magick::CharPixel,
                                        pFrameRGB->data[0]);
                    result = image;
                }
                catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
                catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

                av_packet_unref(&packet);
                sws_freeContext(img_convert_ctx);
                break;
            }
            currentFrame++;
        }
    }

    av_packet_unref(&packet);
    avcodec_close(pCodecCtx);
    av_free(pFrame);
    av_free(pFrameRGB);
    avformat_close_input(&pFormatCtx);

    return result;
}
#endif

/*void Common::populateColorProfileMenu(QMenu *menu,
                                      Magick::ColorspaceType colorspace)
{
    menu->clear();
    QMapIterator<QString, QString> i(Common::getColorProfiles(colorspace));
    while (i.hasNext()) {
        i.next();
        QAction *action = new QAction(menu);
        action->setIcon(QIcon(":/icons/colors.png"));
        action->setText(i.key());
        action->setData(i.value());
        action->setCheckable(true);
        menu->addAction(action);
        connect(action,
                SIGNAL(triggered()),
                this,
                SLOT(selectDefaultColorProfile()));
    }
}

void Common::setDefaultColorProfileFromFilename(QMenu *menu,
                                                const QString &filename)
{
    for (int i=0;i<menu->actions().size();++i) {
        QAction *action = menu->actions().at(i);
        if (!action) { continue; }
        if (action->data().toString() == filename) {
            action->setChecked(true);
        } else { action->setChecked(false); }
    }
}

void Common::setDefaultColorProfileFromTitle(QMenu *menu,
                                             const QString &title)
{
    for (int i=0;i<menu->actions().size();++i) {
        QAction *action = menu->actions().at(i);
        if (!action) { continue; }
        if (action->text() == title) {
            action->setChecked(true);
        } else { action->setChecked(false); }
    }
}

Magick::Blob Common::selectedDefaultColorProfileData(QMenu *menu)
{
    QString filename;
    for (int i=0;i<menu->actions().size();++i) {
        QAction *action = menu->actions().at(i);
        if (!action) { continue; }
        if (action->isChecked()) { filename =  action->data().toString(); }
    }
    if (!filename.isEmpty()) {
        try {
            Magick::Image image;
            image.read(filename.toStdString());
            Magick::Blob profile;
            image.write(&profile);
            return profile;
        }
        catch(Magick::Error &error_ ) { emit errorMessage(error_.what()); }
        catch(Magick::Warning &warn_ ) { emit warningMessage(warn_.what()); }
    }
    return Magick::Blob();
}

const QString Common::selectedDefaultColorProfile(QMenu *menu)
{
    for (int i=0;i<menu->actions().size();++i) {
        QAction *action = menu->actions().at(i);
        if (!action) { continue; }
        if (action->isChecked()) { return action->data().toString(); }
    }
    return QString();
}
*/
