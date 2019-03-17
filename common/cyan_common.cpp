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

#include "cyan_common.h"

#include <QDebug>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QAction>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#define RESOURCE_BYTE 1050000000

CyanCommon::CyanCommon(QObject *parent): QObject (parent)
{
}

QString CyanCommon::timestamp()
{
    QDateTime date;
    return date.currentDateTime().toString("yyyyMMddHHmmsszzz");
}

QMap<Magick::CompositeOperator, QString> CyanCommon::compositeModes()
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

Magick::CompositeOperator CyanCommon::compositeModeFromString(const QString &name)
{
    QMapIterator<Magick::CompositeOperator, QString> i(CyanCommon::compositeModes());
    while (i.hasNext()) {
        i.next();
        if (i.value() == name) { return i.key(); }
    }
    return Magick::UndefinedCompositeOp;
}

const QString CyanCommon::canvasWindowTitle(Magick::Image image)
{
    // filename @ colorspace widthxheight dpixdpi bit
    QString label = QObject::tr("New image");
    if (!QString::fromStdString(image.label()).isEmpty()) {
        label = QString::fromStdString(image.label());
    }
    QString profile = CyanCommon::getProfileTag(image.iccColorProfile());
    if (profile.isEmpty()) { profile = "N/A"; }
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
        result = QString("%1 @ %2 (%8) %3x%4 %5x%6 %7-bit")
                        .arg(label)
                        .arg(colorspace)
                        .arg(width)
                        .arg(height)
                        .arg(dpiX)
                         .arg(dpiY)
                        .arg(depth)
                        .arg(profile);
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    return result;
}

int CyanCommon::getDiskResource()
{
    return qRound(static_cast<double>(Magick::ResourceLimits::disk()/RESOURCE_BYTE));
}

void CyanCommon::setDiskResource(int gib)
{
    try {
        Magick::ResourceLimits::disk(static_cast<qulonglong>(gib)*static_cast<qulonglong>(RESOURCE_BYTE));
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
}

int CyanCommon::getMemoryResource()
{
    return qRound(static_cast<double>(Magick::ResourceLimits::memory()/RESOURCE_BYTE));
}

void CyanCommon::setMemoryResource(int gib)
{
    try {
        Magick::ResourceLimits::memory(static_cast<qulonglong>(gib)*static_cast<qulonglong>(RESOURCE_BYTE));
        Magick::ResourceLimits::map(static_cast<qulonglong>(gib)*static_cast<qulonglong>(RESOURCE_BYTE));
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
}

void CyanCommon::setThreadResources(int thread)
{
    if (thread == 0) { return; }
    Magick::ResourceLimits::thread(static_cast<qulonglong>(thread));
}

bool CyanCommon::writeCanvas(CyanCommon::Canvas canvas,
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
    catch(Magick::Error &error_ ) {
        qWarning() << error_.what();
        return false;
    }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    // mark as cyan project
    try {
        image.attribute(QString(CYAN_PROJECT).toStdString(),
                        QString("%1").arg(CYAN_PROJECT_VERSION)
                        .toStdString());
    }
    catch(Magick::Error &error_ ) {
        qWarning() << error_.what();
        return false;
    }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    // compress(?)
    try { image.compressType(compress); }
    catch(Magick::Error &error_ ) {
        qWarning() << error_.what();
        return false;
    }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    // add color profile
    try { image.profile("ICC", canvas.profile); }
    catch(Magick::Error &error_ ) {
        qWarning() << error_.what();
        return false;
    }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    // add canvas as first image
    image.backgroundColor(Magick::ColorRGB(1, 1, 1)); // workaround issue in IM
    image.alpha(false); // workaround issue in IM
    image.alpha(true); // workaround issue in IM
    images.push_back(image);

    // add layers
    QMapIterator<int, CyanCommon::Layer> layers(canvas.layers);
    while (layers.hasNext()) {
        layers.next();
        Magick::Image layer(layers.value().image);
        layer.magick("MIFF");

        // add label
        try {
            layer.label(layers.value().label.toStdString());
        }
        catch(Magick::Error &error_ ) {
            qWarning() << error_.what();
            return false;
        }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // add text
        if (!layers.value().text.isEmpty()) {
            try {
                layer.alpha(true);
                layer.evaluate(Magick::AlphaChannel,
                                Magick::MultiplyEvaluateOperator,
                                0.0);
                layer.attribute(QString(CYAN_LAYER_TEXT).toStdString(),
                                QString("%1").arg(layers.value().text)
                                .toStdString());
            }
            catch(Magick::Error &error_ ) {
                qWarning() << error_.what();
                return false;
            }
            catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
        }

        // add order
        try {
            layer.attribute(QString(CYAN_LAYER_ORDER).toStdString(),
                            QString("%1").arg(layers.value().order)
                            .toStdString());
        }
        catch(Magick::Error &error_ ) {
            qWarning() << error_.what();
            return false;
        }
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
        catch(Magick::Error &error_ ) {
            qWarning() << error_.what();
            return false;
        }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // mark as cyan layer
        try {
            layer.attribute(QString(CYAN_LAYER).toStdString(),
                            QString("%1").arg(CYAN_LAYER_VERSION)
                            .toStdString());
        }
        catch(Magick::Error &error_ ) {
            qWarning() << error_.what();
            return false;
        }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // add compose mode
        try {
            layer.attribute(QString(CYAN_LAYER_COMPOSE).toStdString(),
                            QString("%1").arg(CyanCommon::compositeModes()[layers.value().composite])
                            .toStdString());
        }
        catch(Magick::Error &error_ ) {
            qWarning() << error_.what();
            return false;
        }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // add visibility
        try {
            layer.attribute(QString(CYAN_LAYER_VISIBILITY).toStdString(),
                            QString("%1").arg(layers.value().visible)
                            .toStdString());
        }
        catch(Magick::Error &error_ ) {
            qWarning() << error_.what();
            return false;
        }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // add lock
        try {
            layer.attribute(QString(CYAN_LAYER_LOCK).toStdString(),
                            QString("%1").arg(layers.value().locked)
                            .toStdString());
        }
        catch(Magick::Error &error_ ) {
            qWarning() << error_.what();
            return false;
        }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // add opacity
        try {
            layer.attribute(QString(CYAN_LAYER_OPACITY).toStdString(),
                            QString("%1").arg(layers.value().opacity)
                            .toStdString());
        }
        catch(Magick::Error &error_ ) {
            qWarning() << error_.what();
            return false;
        }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // compress(?)
        try { layer.compressType(compress); }
        catch(Magick::Error &error_ ) {
            qWarning() << error_.what();
            return false;
        }
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

CyanCommon::Canvas CyanCommon::readCanvas(const QString &filename)
{
    CyanCommon::Canvas canvas;

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

            //set profile
            canvas.profile = canvas.image.iccColorProfile();
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

            // get lock
            int locked = QString::fromStdString(it->attribute(QString(CYAN_LAYER_LOCK)
                                                              .toStdString())).toInt();

            // get text
            QString text = QString::fromStdString(it->attribute(QString(CYAN_LAYER_TEXT).toStdString()));

            // get order
            int order = QString::fromStdString(it->attribute(QString(CYAN_LAYER_ORDER)
                                                                  .toStdString())).toInt();

            // get position
            int offX = QString::fromStdString(it->attribute(QString(CYAN_LAYER_X)
                                                            .toStdString())).toInt();
            int offY = QString::fromStdString(it->attribute(QString(CYAN_LAYER_Y)
                                                            .toStdString())).toInt();
            QSize pos = QSize(offX, offY);

            // get composite
            Magick::CompositeOperator compose = CyanCommon::compositeModeFromString(QString::fromStdString(it->attribute(QString(CYAN_LAYER_COMPOSE).toStdString())));
            CyanCommon::Layer layer;

            // setup layer
            if (!text.isEmpty()) {
                Magick::Image orig(*it);
                layer.image.size(Magick::Geometry(orig.columns(), orig.rows()));
                layer.image.depth(orig.depth());
                layer.image.colorSpace(orig.colorSpace());
            } else {
                layer.image = *it;
            }
            layer.label = QString::fromStdString(layer.image.label());
            layer.visible = visibility;
            layer.locked = locked;
            layer.pos = pos;
            layer.opacity = opacity;
            layer.composite = compose;
            layer.order = order;
            layer.text = text;
            layer.isText = !text.isEmpty();

            /*if (canvas.profile.length()==0 &&
                layer.image.iccColorProfile().length()>0)
            {
                qWarning() << "CANVAS PROFILE IS EMPTY! ADD FALLBACK FROM LAYER!";
                canvas.profile = layer.image.iccColorProfile();
                try {
                    canvas.image.profile("ICC", canvas.profile);
                }
                catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
                catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
            }*/

            // decompress
            try { layer.image.compressType(Magick::NoCompression); }
            catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
            catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

            // strip
            layer.image.strip();

            // add
            canvas.layers.insert(canvas.layers.size(),
                                 layer);
        }
    }
    return canvas;
}

bool CyanCommon::isValidCanvas(const QString &filename)
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

bool CyanCommon::isValidImage(const QString &filename)
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

int CyanCommon::hasLayers(const QString &filename)
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

CyanCommon::Canvas CyanCommon::readImage(const QString &filename)
{
    CyanCommon::Canvas canvas;
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

QStringList CyanCommon::getColorProfilesPath()
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

QMap<QString, QString> CyanCommon::getColorProfiles(Magick::ColorspaceType colorspace)
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

Magick::ColorspaceType CyanCommon::getProfileColorspace(const QString &filename)
{
    if (!filename.isEmpty()) {
        return getProfileColorspace(cmsOpenProfileFromFile(filename.toStdString().c_str(), "r"));
    }
    return Magick::UndefinedColorspace;
}

Magick::ColorspaceType CyanCommon::getProfileColorspace(cmsHPROFILE profile)
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

const QString CyanCommon::getProfileTag(Magick::Blob buffer,
                                        CyanCommon::ICCTag tag)
{
    qDebug() << "get profile tag from blob" << buffer.length();
    if (buffer.length()>0) {
        return getProfileTag(cmsOpenProfileFromMem(buffer.data(),
                                                   static_cast<cmsUInt32Number>(buffer.length())),tag);
    }
    return QString();
}

const QString CyanCommon::getProfileTag(const QString filename,
                                    CyanCommon::ICCTag tag)
{
    if (!filename.isEmpty()) {
        return getProfileTag(cmsOpenProfileFromFile(filename.toStdString().c_str(), "r"), tag);
    }
    return QString();
}

const QString CyanCommon::getProfileTag(cmsHPROFILE profile,
                                    CyanCommon::ICCTag tag)
{
    std::string result;
    if (profile) {
        cmsUInt32Number size = 0;
        cmsInfoType cmsSelectedType;
        switch(tag) {
        case CyanCommon::ICCManufacturer:
            cmsSelectedType = cmsInfoManufacturer;
            break;
        case CyanCommon::ICCModel:
            cmsSelectedType = cmsInfoModel;
            break;
        case CyanCommon::ICCCopyright:
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

const QString CyanCommon::supportedWriteFormats()
{
    QString result;
    if (supportsJpeg()) {
        result.append(QString("*.jpeg "));
        result.append(QString("*.jpg "));
    }
    if (supportsJP2()) {
        result.append(QString("*.jp2 "));
    }
    if (supportsPng()) {
        result.append(QString("*.png "));
    }
    if (supportsTiff()) {
        result.append(QString("*.tiff "));
        result.append(QString("*.tif "));
    }
    if (supportsOpenExr()) {
        result.append(QString("*.exr "));
    }
    result.append(QString("*.bmp "));
    result.append(QString("*.psd "));

    return result;
}

const QString CyanCommon::supportedReadFormats()
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

int CyanCommon::supportedQuantumDepth()
{
    return QString(MagickQuantumDepth)
           .replace(QString("Q"), QString(""))
            .toInt();
}

bool CyanCommon::supportsJpeg()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("jpeg", Qt::CaseSensitive);
}

bool CyanCommon::supportsPng()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("png", Qt::CaseSensitive);
}

bool CyanCommon::supportsTiff()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("tiff", Qt::CaseSensitive);
}

bool CyanCommon::supportsLcms()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("lcms", Qt::CaseSensitive);
}

bool CyanCommon::supportsHdri()
{
    return QString(QString::fromStdString(MagickCore::GetMagickFeatures()))
            .contains("HDRI", Qt::CaseSensitive);
}

bool CyanCommon::supportsOpenMP()
{
    return QString(QString::fromStdString(MagickCore::GetMagickFeatures()))
            .contains("OpenMP", Qt::CaseSensitive);
}

bool CyanCommon::supportsBzlib()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("bzlib", Qt::CaseSensitive);
}

bool CyanCommon::supportsCairo()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("cairo", Qt::CaseSensitive);
}

bool CyanCommon::supportsFontConfig()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("fontconfig", Qt::CaseSensitive);
}

bool CyanCommon::supportsFreeType()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("freetype", Qt::CaseSensitive);
}

bool CyanCommon::supportsJP2()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("openjp2", Qt::CaseSensitive);
}

bool CyanCommon::supportsLzma()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("lzma", Qt::CaseSensitive);
}

bool CyanCommon::supportsOpenExr()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("openexr", Qt::CaseSensitive);
}

bool CyanCommon::supportsPangoCairo()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("pangocairo", Qt::CaseSensitive);
}

bool CyanCommon::supportsRaw()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("raw", Qt::CaseSensitive);
}

bool CyanCommon::supportsRsvg()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("rsvg", Qt::CaseSensitive);
}

bool CyanCommon::supportsWebp()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("webp", Qt::CaseSensitive);
}

bool CyanCommon::supportsXml()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("xml", Qt::CaseSensitive);
}

bool CyanCommon::supportsZlib()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("zlib", Qt::CaseSensitive);
}

bool CyanCommon::supportsJng()
{
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
            .contains("jng", Qt::CaseSensitive);
}

const QString CyanCommon::humanFileSize(float num, bool mp, bool are)
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

const QString CyanCommon::html2Pango(const QString &html)
{
    QString tmp = html;
    QStringList list = tmp
                       .replace("<p", "<!PARA")
                       .replace("<span","<!SPAN")
                       .replace("</p>","</!PARA>")
                       .replace("</span>","</!SPAN>")
                       .split("<");

    QString markup;
    for (int i=0;i<list.size();++i) {
        QString output;
        if (list.at(i).startsWith("!PARA")) {
            qDebug() << "PARA START" << list.at(i);
            QStringList item = list.at(i).split(">");
            QString val = item.last();
            output = "<span>";
            if (!val.isEmpty()) { output.append(val); }
        } else if (list.at(i).startsWith("/!PARA")) {
            output = "</span>\n";
        } else if (list.at(i).startsWith("!SPAN")) {
            QString span = list.at(i);
            //qDebug() << span;
            QStringList item = span.split(">");
            QString opt = item.first();
            QString val = item.last();
            //qDebug() << val << opt;
            if (val.isEmpty()) { // container

            } else { // span
                QRegularExpression rxFontSize("(?<=font-size:)[^;]*");
                QRegularExpressionMatch matchFontSize = rxFontSize.match(opt);
                QString fontSize = matchFontSize.captured().replace("'","").trimmed();

                QRegularExpression rxFontStyle("(?<=font-style:)[^;]*");
                QRegularExpressionMatch matchFontStyle = rxFontStyle.match(opt);
                QString fontStyle = matchFontStyle.captured().replace("'","").trimmed();

                QRegularExpression rxFontFamily("(?<=font-family:)[^;]*");
                QRegularExpressionMatch matchFontFamily = rxFontFamily.match(opt);
                QString fontFamily = matchFontFamily.captured().replace("'","");

                QRegularExpression rxFontWeight("(?<=font-weight:)[^;]*");
                QRegularExpressionMatch matchFontWeight = rxFontWeight.match(opt);
                QString fontWeight = matchFontWeight.captured().replace("'","").trimmed();

                QRegularExpression rxFontDecoration("(?<=text-decoration:)[^;]*");
                QRegularExpressionMatch matchFontDecoration = rxFontDecoration.match(opt);
                QString fontDecoration = matchFontDecoration.captured().replace("'","").trimmed();

                QRegularExpression rxFontColor("(?<=color:)[^;]*");
                QRegularExpressionMatch matchFontColor = rxFontColor.match(opt);
                QString fontColor = matchFontColor.captured().replace("'","").trimmed();

                /*qDebug() << "FONT-SIZE" << fontSize;
                qDebug() << "FONT STYLE" << fontStyle;
                qDebug() << "FONT FAMILY" << fontFamily;
                qDebug() << "FONT WEIGHT" << fontWeight;
                qDebug() << "FONT DECORATION" << fontDecoration;
                qDebug() << "FONT COLOR" << fontColor;*/

                if (!fontSize.isEmpty()) {
                    fontSize = QString("font='%1'").arg(fontSize.replace("pt","").toDouble());
                }
                if (!fontStyle.isEmpty()) {
                    fontStyle = QString("font_style='%1'").arg(fontStyle);
                }
                if (!fontFamily.isEmpty()) {
                    fontFamily = QString("font_family='%1'").arg(fontFamily);
                }
                if (!fontWeight.isEmpty()) {
                    fontWeight = QString("font_weight='%1'").arg(fontWeight);
                }
                if (!fontDecoration.isEmpty()) {
                    if (fontDecoration == "underline") {
                        fontDecoration = QString("underline='single'");
                    }
                }
                if (!fontColor.isEmpty()) {
                    fontColor = QString("foreground='%1'").arg(fontColor);
                }

                QString pango = "<span";
                if (!fontSize.isEmpty()) { pango.append(QString(" %1").arg(fontSize)); }
                if (!fontStyle.isEmpty()) { pango.append(QString(" %1").arg(fontStyle)); }
                if (!fontFamily.isEmpty()) { pango.append(QString(" %1").arg(fontFamily)); }
                if (!fontWeight.isEmpty()) { pango.append(QString(" %1").arg(fontWeight)); }
                if (!fontDecoration.isEmpty()) { pango.append(QString(" %1").arg(fontDecoration)); }
                if (!fontColor.isEmpty()) { pango.append(QString(" %1").arg(fontColor)); }
                pango.append(QString(">%1</span>").arg(val));
                output = pango;
            }
        }
        //qDebug() << "OUT" << output;
        if (!output.isEmpty()) { markup.append(output); }
    }

    qDebug() << markup;
    return markup;
}

#ifdef WITH_FFMPEG
QByteArray CyanCommon::getEmbeddedCoverArt(const QString &filename)
{
    QByteArray result;
    if (!filename.isEmpty()) {
        AVCodecContext  *pCodecCtx;
        AVFormatContext *pFormatCtx = avformat_alloc_context();
        AVCodec * pCodec;

        if (avformat_open_input(&pFormatCtx,filename.toUtf8().data(),
                                Q_NULLPTR,
                                Q_NULLPTR) != 0) { return result; }
        if (avformat_find_stream_info(pFormatCtx,
                                      Q_NULLPTR) < 0) { return result; }

        av_dump_format(pFormatCtx, 0, filename.toUtf8().data(), 0);
        int videoStream = -1;

        for (int i=0; i < static_cast<int>(pFormatCtx->nb_streams); i++) {
            if(pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                videoStream = i;
                break;
            }
        }
        if (videoStream == -1) { return result; }

        pCodec =avcodec_find_decoder(pFormatCtx->streams[videoStream]->codecpar->codec_id);
        pCodecCtx = avcodec_alloc_context3(Q_NULLPTR);
        if (pCodec == Q_NULLPTR || pCodecCtx == Q_NULLPTR) { return result; }
        if (avcodec_parameters_to_context(pCodecCtx,
                                          pFormatCtx->streams[videoStream]->codecpar)<0)
        { return result; }
        if (avcodec_open2(pCodecCtx,
                         pCodec,
                         Q_NULLPTR) < 0) { return result; }

        if (pFormatCtx->streams[videoStream]->disposition == AV_DISPOSITION_ATTACHED_PIC) {
            AVPacket pkt = pFormatCtx->streams[videoStream]->attached_pic;
            if (pkt.size>0) {
                QByteArray attachedPix = QByteArray(reinterpret_cast<const char*>(pkt.data),
                                                    pkt.size);
                if (attachedPix.length()>0) {
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

int CyanCommon::getVideoMaxFrames(const QString &filename)
{
    int result = -1;
    if (filename.isEmpty()) { return result; }

    AVCodecContext  *pCodecCtx;
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    AVCodec * pCodec;

    if (avformat_open_input(&pFormatCtx,filename.toUtf8().data(),
                            Q_NULLPTR,
                            Q_NULLPTR) != 0) { return result; }
    if (avformat_find_stream_info(pFormatCtx,
                                  Q_NULLPTR) < 0) { return result; }

    av_dump_format(pFormatCtx, 0, filename.toUtf8().data(), 0);
    int videoStream = -1;

    for (int i=0; i < static_cast<int>(pFormatCtx->nb_streams); i++) {
        if(pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }
    if (videoStream == -1) { return result; }

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

    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
    return  result;
}

Magick::Image CyanCommon::getVideoFrame(const QString &filename,
                                    int frame)
{
    Magick::Image result;

    if (filename.isEmpty()) { return result; }

    AVCodecContext  *pCodecCtx;
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    AVCodec * pCodec;
    AVFrame *pFrame, *pFrameRGB;

    if (avformat_open_input(&pFormatCtx,filename.toUtf8().data(),
                            Q_NULLPTR,
                            Q_NULLPTR) != 0) { return result; }
    if (avformat_find_stream_info(pFormatCtx,
                                  Q_NULLPTR) < 0) { return result; }

    av_dump_format(pFormatCtx, 0, filename.toUtf8().data(), 0);
    int videoStream = -1;

    for (int i=0; i < static_cast<int>(pFormatCtx->nb_streams); i++) {
        if(pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }
    if (videoStream == -1) { return result; }

    pCodec =avcodec_find_decoder(pFormatCtx->streams[videoStream]->codecpar->codec_id);
    pCodecCtx = avcodec_alloc_context3(Q_NULLPTR);
    if (pCodec == Q_NULLPTR || pCodecCtx == Q_NULLPTR) { return result; }
    if (avcodec_parameters_to_context(pCodecCtx,
                                      pFormatCtx->streams[videoStream]->codecpar)<0)
    { return result; }
    if (avcodec_open2(pCodecCtx,
                     pCodec,
                     Q_NULLPTR) < 0) { return result; }

    pFrame    = av_frame_alloc();
    pFrameRGB = av_frame_alloc();

    uint8_t *buffer;
    int numBytes;

    AVPixelFormat  pFormat = AV_PIX_FMT_BGR24;
    numBytes = av_image_get_buffer_size(pFormat,
                                        pCodecCtx->width,
                                        pCodecCtx->height,
                                        16);
    buffer = static_cast<uint8_t*>(av_malloc(static_cast<unsigned long>(numBytes)*sizeof(uint8_t)));
    av_image_fill_arrays(pFrameRGB->data,
                         pFrameRGB->linesize,
                         buffer,
                         pFormat,
                         pCodecCtx->width,
                         pCodecCtx->height,
                         1);

    int res;
    int frameFinished;
    AVPacket packet;
    int currentFrame = 0;
    bool fetchFrame = false;
    double fps = av_q2d(pFormatCtx->streams[videoStream]->r_frame_rate);
    double dur = static_cast<double>(pFormatCtx->duration)/AV_TIME_BASE;
    int maxFrame = qRound((dur*fps)/2);
    if (frame>maxFrame) { frame = maxFrame; }

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
        if (currentFrame>=frame) { fetchFrame = true; }
        if (packet.stream_index == videoStream){
            if (!fetchFrame) {
                currentFrame++;
                continue;
            }

            int ret = avcodec_send_packet(pCodecCtx, &packet);
            if (ret<0) { continue; }
            ret = avcodec_receive_frame(pCodecCtx, pFrame);
            if (ret>=0) { frameFinished = true; }
            else { continue; }

            if (frameFinished) {
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
                          (dynamic_cast<AVFrame*>(pFrame)->data),
                          (dynamic_cast<AVFrame*>(pFrame)->linesize),
                          0,
                          pCodecCtx->height,
                          (dynamic_cast<AVFrame*>(pFrameRGB)->data),
                          (dynamic_cast<AVFrame*>(pFrameRGB)->linesize));
                try {
                    Magick::Image image(static_cast<size_t>(pFrame->width),
                                        static_cast<size_t>(pFrame->height),
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

