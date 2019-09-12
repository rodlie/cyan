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

#include "CyanImageFormat.h"
#include <QObject>
#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

CyanImageFormat::CyanImageFormat()
{
}

const QString CyanImageFormat::getVersion()
{
    return QString(CYAN_IMAGE_FORMAT_VERSION);
}

QMap<Magick::CompositeOperator, QPair<QString, int> > CyanImageFormat::compositeModes()
{
    QMap<Magick::CompositeOperator, QPair<QString, int> > result;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Normal");
        info.second = 0;
        result[Magick::OverCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Overlay");
        info.second = 1;
        result[Magick::OverlayCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Screen");
        info.second = 2;
        result[Magick::ScreenCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Dissolve");
        info.second = 3;
        result[Magick::DissolveCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Multiply");
        info.second = 4;
        result[Magick::MultiplyCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Addition");
        info.second = 5;
        result[Magick::PlusCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Minus (src)");
        info.second = 6;
        result[Magick::MinusSrcCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Minus (dst)");
        info.second = 7;
        result[Magick::MinusDstCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Divide (src)");
        info.second = 8;
        result[Magick::DivideSrcCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Divide (dst)");
        info.second = 9;
        result[Magick::DivideDstCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Difference");
        info.second = 10;
        result[Magick::DifferenceCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("XOR");
        info.second = 11;
        result[Magick::XorCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Burn");
        info.second = 12;
        result[Magick::ColorBurnCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Dodge");
        info.second = 13;
        result[Magick::ColorDodgeCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Hard light");
        info.second = 14;
        result[Magick::HardLightCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Lighten only");
        info.second = 15;
        result[Magick::LightenCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Lighten intensity");
        info.second = 16;
        result[Magick::LightenIntensityCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Darken only");
        info.second = 17;
        result[Magick::DarkenCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Darken intensity");
        info.second = 18;
        result[Magick::DarkenIntensityCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Vivid light");
        info.second = 19;
        result[Magick::VividLightCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Pin light");
        info.second = 20;
        result[Magick::PinLightCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Soft light");
        info.second = 21;
        result[Magick::SoftLightCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Pegtop light");
        info.second = 22;
        result[Magick::PegtopLightCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Copy Black");
        info.second = 23;
        result[Magick::CopyBlackCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Copy Blue");
        info.second = 24;
        result[Magick::CopyBlueCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Copy Cyan");
        info.second = 25;
        result[Magick::CopyCyanCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Copy Green");
        info.second = 26;
        result[Magick::CopyGreenCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Copy Magenta");
        info.second = 27;
        result[Magick::CopyMagentaCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Copy Red");
        info.second = 28;
        result[Magick::CopyRedCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Copy Yellow");
        info.second = 29;
        result[Magick::CopyYellowCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Hue");
        info.second = 30;
        result[Magick::HueCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Saturation");
        info.second = 31;
        result[Magick::SaturateCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Substract");
        info.second = 32;
        result[Magick::MinusSrcCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Divide");
        info.second = 33;
        result[Magick::DivideSrcCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Burn (linear)");
        info.second = 34;
        result[Magick::LinearBurnCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Dodge (linear)");
        info.second = 35;
        result[Magick::LinearDodgeCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Threshold");
        info.second = 36;
        result[Magick::ThresholdCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Replace");
        info.second = 37;
        result[Magick::ReplaceCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Modulate");
        info.second = 38;
        result[Magick::ModulateCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Luminize");
        info.second = 39;
        result[Magick::LuminizeCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Linear light");
        info.second = 40;
        result[Magick::LinearLightCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Hard mix");
        info.second = 41;
        result[Magick::HardMixCompositeOp] = info;
    }
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Exclusion");
        info.second = 42;
        result[Magick::ExclusionCompositeOp] = info;
    }
    return result;
}

Magick::CompositeOperator CyanImageFormat::findCompositeMode(const QString &title,
                                                             int id)
{
    QMapIterator<Magick::CompositeOperator, QPair<QString, int> > i(CyanImageFormat::compositeModes());
    while (i.hasNext()) {
        i.next();
        if (id<0 && !title.isEmpty()) { // match on title
            if (i.value().first == title) { return i.key(); }
        } else { // match on id
            if (i.value().second == id) { return i.key(); }
        }
    }
    return Magick::UndefinedCompositeOp;
}

bool CyanImageFormat::isValidCanvas(const QString &filename)
{
    if (filename.isEmpty()) { return false; }
    try {
        Magick::Image image;
        image.quiet(true);
        image.ping(filename.toStdString());
        double project = QString::fromStdString(image.attribute(QString(CYAN_IMAGE_FORMAT)
                                                                .toStdString())).toDouble();
        if (project>0) { return true; }
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
    return false;
}

bool CyanImageFormat::isValidImage(const QString &filename)
{
    if (filename.isEmpty()) { return false; }
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

int CyanImageFormat::hasLayers(const QString &filename)
{
    if (filename.isEmpty()) { return 0; }
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

bool CyanImageFormat::writeCanvas(CyanImageFormat::CyanCanvas canvas,
                                  const QString &filename,
                                  Magick::CompressionType compress)
{
    if (filename.isEmpty() || !canvas.image.isValid()) { return false; }
    std::list<Magick::Image> images;
    Magick::Image image(canvas.image);
    image.magick("MIFF");

    // set cyan project label
    try {
        image.attribute(QString(CYAN_LAYER_LABEL).toStdString(),
                        canvas.label.toStdString());
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); return false; }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    // set cyan project version
    try { image.attribute(QString(CYAN_IMAGE_FORMAT).toStdString(),
                          QString("%1").arg(CYAN_IMAGE_FORMAT_VERSION)
                          .toStdString()); }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); return false; }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    // compress(?)
    try { image.compressType(compress); }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); return false; }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    // add color profile
    try { image.profile("ICC", canvas.profile); }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); return false; }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    // add guides
    QStringList vguides,hguides;
    QMapIterator<int, CyanGuideItem*> i(canvas.guides);
    while (i.hasNext()) {
        i.next();
        if (i.value()->isHorizontal()) {
            hguides << QString("%1x%2")
                       .arg(i.value()->pos().x())
                       .arg(i.value()->pos().y());
        } else {
            vguides << QString("%1x%2")
                       .arg(i.value()->pos().x())
                       .arg(i.value()->pos().y());
        }
    }
    if (vguides.size()>0) {
        try { image.attribute(QString(CYAN_IMAGE_V_GUIDES).toStdString(),
                              vguides.join(";").toStdString()); }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); return false; }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
    }
    if (hguides.size()>0) {
        try { image.attribute(QString(CYAN_IMAGE_H_GUIDES).toStdString(),
                              hguides.join(";").toStdString()); }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); return false; }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
    }

    // add canvas as first image
    image.backgroundColor(Magick::ColorRGB(1, 1, 1)); // workaround issue in IM
    image.alpha(false); // workaround issue in IM
    image.alpha(true); // workaround issue in IM
    images.push_back(image);

    // add layers
    QMapIterator<int, CyanLayer> layers(canvas.layers);
    while (layers.hasNext()) {
        layers.next();
        Magick::Image layer(layers.value().image);
        layer.magick("MIFF");

        // add label
        try {
            layer.attribute(QString(CYAN_LAYER_LABEL).toStdString(),
                            layers.value().label.toStdString());
        }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); return false; }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // add text
        if (layers.value().isText) {
            try {
                layer.alpha(true);
                layer.evaluate(Magick::AlphaChannel,
                                Magick::MultiplyEvaluateOperator,
                                0.0);
                layer.attribute(QString(CYAN_LAYER_TEXT).toStdString(),
                                QString("%1").arg(CYAN_IMAGE_FORMAT_VERSION)
                                .toStdString());
                layer.attribute(QString(CYAN_LAYER_TEXT_HTML).toStdString(),
                                QString("%1").arg(layers.value().html)
                                .toStdString());
                layer.attribute(QString(CYAN_LAYER_TEXT_ALIGN).toStdString(),
                                QString("%1").arg(layers.value().textAlign)
                                .toStdString());
                layer.attribute(QString(CYAN_LAYER_TEXT_ROTATE).toStdString(),
                                QString("%1").arg(layers.value().textRotate)
                                .toStdString());
            }
            catch(Magick::Error &error_ ) { qWarning() << error_.what(); return false; }
            catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
        }

        // add order
        try {
            layer.attribute(QString(CYAN_LAYER_ORDER).toStdString(),
                            QString("%1").arg(layers.value().order)
                            .toStdString());
        }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); return false; }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // add position
        try {
            layer.attribute(QString(CYAN_LAYER_X).toStdString(),
                            QString("%1").arg(layers.value().position.width())
                            .toStdString());
            layer.attribute(QString(CYAN_LAYER_Y).toStdString(),
                            QString("%1").arg(layers.value().position.height())
                            .toStdString());
        }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); return false; }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // set cyan layer version
        try {
            layer.attribute(QString(CYAN_LAYER).toStdString(),
                            QString("%1").arg(CYAN_IMAGE_FORMAT_VERSION)
                            .toStdString());
        }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); return false; }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // add compose mode
        try {
            layer.attribute(QString(CYAN_LAYER_COMPOSE).toStdString(),
                            QString("%1").arg(compositeModes()[layers.value().composite].second)
                            .toStdString());
        }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); return false; }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // add visibility
        try {
            layer.attribute(QString(CYAN_LAYER_VISIBILITY).toStdString(),
                            QString("%1").arg(layers.value().visible)
                            .toStdString());
        }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); return false; }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // add lock
        try {
            layer.attribute(QString(CYAN_LAYER_LOCK).toStdString(),
                            QString("%1").arg(layers.value().locked)
                            .toStdString());
        }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); return false; }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // add opacity
        try {
            layer.attribute(QString(CYAN_LAYER_OPACITY).toStdString(),
                            QString("%1").arg(layers.value().opacity)
                            .toStdString());
        }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); return false; }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // compress(?)
        try { layer.compressType(compress); }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); return false; }
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
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); return false; }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    if (QFile::exists(filename)) { return true; }
    // TODO add more checks
    return false;
}

CyanImageFormat::CyanCanvas CyanImageFormat::readCanvas(const QString &filename)
{
    CyanCanvas canvas;

    if (!isValidCanvas(filename)) { return canvas; }

    // read images
    std::list<Magick::Image> images;
    try {
        Magick::readImages(&images,
                           filename.toStdString());
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    for (std::list<Magick::Image>::iterator it = images.begin(); it != images.end(); ++it){
        double project = QString::fromStdString(it->attribute(QString(CYAN_IMAGE_FORMAT)
                                                              .toStdString())).toDouble();
        if (project>0) { // this is our canvas
            canvas.image = *it;

            // decompress
            try { canvas.image.compressType(Magick::NoCompression); }
            catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
            catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

            // set label
            canvas.label = QString::fromStdString(it->attribute(QString(CYAN_LAYER_LABEL).toStdString()));

            // set profile
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
            QString html = QString::fromStdString(it->attribute(QString(CYAN_LAYER_TEXT_HTML).toStdString()));
            int textAlign = QString::fromStdString(it->attribute(QString(CYAN_LAYER_TEXT_ALIGN).toStdString())).toInt();
            int textRotate = QString::fromStdString(it->attribute(QString(CYAN_LAYER_TEXT_ROTATE).toStdString())).toInt();

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
            Magick::CompositeOperator compose = findCompositeMode("",
                                                                  QString::fromStdString(it->attribute(QString(CYAN_LAYER_COMPOSE).toStdString())).toInt());
            CyanLayer layer;

            // is text?
            double isTextLayer = QString::fromStdString(it->attribute(QString(CYAN_LAYER_TEXT).toStdString())).toInt();

            // setup layer
            if (isTextLayer>0) {
                Magick::Image orig(*it);
                layer.image.size(Magick::Geometry(orig.columns(), orig.rows()));
                layer.image.depth(orig.depth());
                layer.image.colorSpace(orig.colorSpace());
            } else {
                layer.image = *it;
            }
            layer.label = QString::fromStdString(it->attribute(QString(CYAN_LAYER_LABEL).toStdString()));
            layer.visible = visibility;
            layer.locked = locked;
            layer.position = pos;
            layer.opacity = opacity;
            layer.composite = compose;
            layer.order = order;
            layer.html = html;
            layer.isText = isTextLayer>0?true:false;
            layer.textAlign = textAlign;
            layer.textRotate = textRotate;

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

CyanImageFormat::CyanCanvas CyanImageFormat::readImage(const QString &filename)
{
    CyanCanvas canvas;
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

Magick::Image CyanImageFormat::renderText(CyanImageFormat::CyanLayer layer)
{
    qDebug() << "render text" << layer.isText << layer.label;
    if (!layer.html.isEmpty() && layer.isText) {
        try {
            layer.image.quiet(true);
            layer.image.alpha(true);
            layer.image.backgroundColor(Magick::Color("transparent"));
            QString markup = html2Pango(layer.html);
            qDebug() << "HTML" << layer.html;
            qDebug() << "PANGO" << markup;
            //if (!layer.textAlign.isEmpty()) {
                layer.image.defineValue("pango",
                                        "align",
                                        QString::number(layer.textAlign).toStdString());
            //}
            if (layer.textRotate!=0) {
                layer.image.defineValue("pango",
                                        "rotate",
                                        QString::number(layer.textRotate).toStdString());
            }
            layer.image.read(QString("PANGO: %1").arg(markup).toStdString());
            layer.image.label(layer.label.toStdString());
        }
        catch(Magick::Error &error_) { qWarning() << error_.what(); }
        catch(Magick::Warning &warn_) { qWarning() << warn_.what(); }
    }
    return layer.image;
}

Magick::Image CyanImageFormat::compLayers(Magick::Image canvas,
                                          QMap<int, CyanImageFormat::CyanLayer> layers,
                                          Magick::Geometry crop)
{
    // copy canvas to comp
    Magick::Image comp(canvas);
    comp.quiet(true); // ignore warnings

    // crop comp
    if (crop.width()>0) {
        try {
            comp.crop(crop);
            comp.repage();
        }
        catch(Magick::Error &error_) { qWarning() << error_.what(); }
        catch(Magick::Warning &warn_) { qWarning() << warn_.what(); }
    }

    // re-order layers for rendering
    QList<QPair<int, int> > order;
    QMapIterator<int, CyanLayer> layer(layers);
    while (layer.hasNext()) {
        layer.next();
        QPair<int, int> pair(layer.value().order,
                             layer.key());
        order.append(pair);
    }
    std::sort(order.begin(),
              order.end(),
              QPairSortFirst());
    //qDebug() << "LAYERS?" << layers.size();
    //qDebug() << "COMP ORDER:" << order;

    for (int i=0;i<order.size();++i) {
        // get layer id
        int id = order.at(i).second;

        // copy layer
        Magick::Image layer(layers[id].image);

        if (!layers[id].visible || !layer.isValid()) { continue; } //skip

        // get layer offset
        int offsetX = layers[id].position.width();
        int offsetY = layers[id].position.height();

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

            if (ignore) { continue; } // something failed, skip

            // repage layer
            layer.repage();

            // calculate layer offset
            if (offsetX>crop.xOff()) { offsetX = static_cast<int>(offsetX-crop.xOff()); }
            else { offsetX =0; }
            if (offsetY>crop.yOff()) { offsetY = static_cast<int>(offsetY-crop.yOff()); }
            else { offsetY = 0;}
        }

        // set layer opacity
        if (layers[id].opacity<1) {
            try {
                layer.alpha(true);
                layer.evaluate(Magick::AlphaChannel,
                               Magick::MultiplyEvaluateOperator,
                               layers[id].opacity);
            }
            catch(Magick::Error &error_) { qWarning() << error_.what(); }
            catch(Magick::Warning &warn_) { qWarning() << warn_.what(); }
        }

        // set layer composite operator
        Magick::CompositeOperator layerComp = layers[id].composite;

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
        catch(Magick::Error &error_) { qWarning() << error_.what(); }
        catch(Magick::Warning &warn_) { qWarning() << warn_.what(); }
    }
    return comp;
}

Magick::Image CyanImageFormat::renderCanvasToImage(CyanImageFormat::CyanCanvas canvas)
{
    return compLayers(canvas.image,
                      canvas.layers);
}

bool CyanImageFormat::renderCanvasToFile(CyanImageFormat::CyanCanvas canvas,
                                         const QString &filename,
                                         Magick::CompressionType compress,
                                         QMap<QString, QString> attr,
                                         QMap<QString, QString> arti)
{
    // render canvas
    Magick::Image image = compLayers(canvas.image,
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

const QString CyanImageFormat::supportedWriteFormats()
{
    QString formats =  supportedImageFormats(false);
    if (hasDelegate("tiff")) { formats.append(" *.tif"); }
    return formats;
}

const QString CyanImageFormat::supportedReadFormats()
{
    return supportedImageFormats(true);
}

const QString CyanImageFormat::supportedImageFormats(bool readFormats)
{
    QStringList found;
    MagickCore::ExceptionInfo *exception;
    const MagickCore::MagickInfo **mFormat = nullptr;
    size_t nFormats;

    MagickCoreGenesis((char *)nullptr, MagickCore::MagickFalse);
    exception = MagickCore::AcquireExceptionInfo();
    mFormat = GetMagickInfoList("*", &nFormats, exception);

    if (!mFormat) { return found.join(""); }
    for (size_t i=0;i<nFormats;i++) {
        QString suffix = "*.";
        if (readFormats && mFormat[i]->decoder) {
            suffix.append(mFormat[i]->name);
            if (isBlacklistedReadFormat(suffix.toLower())) { continue; }
            found << suffix.toLower();
        } else if (!readFormats && mFormat[i]->encoder) {
            suffix.append(mFormat[i]->name);
            if (isBlacklistedWriteFormat(suffix.toLower())) { continue; }
            found << suffix.toLower();
        }
    }

    delete [] mFormat;
    exception = DestroyExceptionInfo(exception);
    MagickCore::MagickCoreTerminus();

    return found.join(" ");
}

bool CyanImageFormat::isBlacklistedReadFormat(const QString &readFormat)
{
    if (readFormat.isEmpty()) { return true; }
    QStringList blacklisted;
    blacklisted << "*.k" << "*.group4" << "*.g" << "*.gradient" << "*.ftp" << "*.fractal";
    blacklisted << "*.fax" << "*.file" << "*.clip" << "*.caption" << "*.canvas" << "*.label";
    blacklisted << "*.a" << "*.b" << "*.c" << "*.o" << "*.txt" << "*.vst" << "*.tile";
    blacklisted << "*.text" << "*.screenshot" << "*.radial-gradient" << "*.plasma" << "*.pattern";
    blacklisted << "*.pango" << "*.mono" << "*.mat" << "*.mask" << "*.map" << "*.http";
#ifndef WITH_FFMPEG
    blacklisted << "*.m2v" << "*.mpg" << "*.mpeg" << "*.mov" << "*.mkv" << "*.m4v" << "*.avi";
#endif
    if (blacklisted.contains(readFormat)) { return true; }
    return false;
}

bool CyanImageFormat::isBlacklistedWriteFormat(const QString &writeFormat)
{
    if (writeFormat.isEmpty()) { return true; }
    QStringList blacklisted;
    blacklisted << "*.a" << "*.aai" << "*.ai" << "*.art" << "*.avs" << "*.b" << "*.bgr" << "*.bgra";
    blacklisted << "*.bgro" << "*.brf" << "*.c" << "*.cal" << "*.cals" << "*.cin" << "*.cip" << "*.clip";
    blacklisted << "*.cmyk" << "*.cmyka" << "*.cur" << "*.data" << "*.epdf" << "*.epi" << "*.eps" << "*.eps2";
    blacklisted << "*.eps3" << "*.epsf" << "*.epsi" << "*.ept" << "*.ept2" << "*.ept3" << "*.fax" << "*.fits";
    blacklisted << "*.fts" << "*.g" << "*.g3" << "*.g4" << "*.gray" << "*.graya" << "*.group4" << "*.hdr";
    blacklisted << "*.histogram" << "*.hrz" << "*.htm" << "*.html" << "*.icb" << "*.ico" << "*.icon";
    blacklisted << "*.info" << "*.inline" <<  "*.ipl" <<  "*.isobrl" <<  "*.isobrl6" << "*.json" <<  "*.k";
    blacklisted <<  "*.m" << "*.m2v" << "*.m4v" << "*.map" << "*.mask" << "*.mat" << "*.matte";
    blacklisted << "*.mkv" << "*.mng" << "*.mono" << "*.mov" << "*.mp4" << "*.mpc" << "*.mpeg" << "*.mpg";
    blacklisted << "*.msl" << "*.msvg" << "*.mtv" << "*.mvg" << "*.null" << "*.o" << "*.otb" << "*.pal";
    blacklisted << "*.palm" << "*.pam" << "*.pdb" << "*.pdf" << "*.pdfa" << "*.ps" << "*.ps2" << "*.ps3";
    blacklisted << "*.psb" << "*.ptif" << "*.r" << "*.ras" << "*.rgb" << "*.rgba" << "*.rgbo" << "*.rgf";
    blacklisted << "*.shtml" << "*.six" <<"*.sixel" << "*.sparse-color" << "*.svg" << "*.svgz" << "*.thumbnail" ;
    blacklisted << "*.txt" << "*.ubrl" << "*.ubrl6" << "*.uil" << "*.uyvy" << "*.vda" << "*.vicar" << "*.vid";
    blacklisted << "*.viff" << "*.vips" << "*.vst" << "*.wbmp" << "*.wmv" << "*.x" << "*.xbm" ;
    blacklisted << "*.xv" << "*.xwd" << "*.y" << "*.ycbcr" << "*.ycbcra" << "*.yuv";
    if (blacklisted.contains(writeFormat)) { return true; }
    return false;
}

int CyanImageFormat::supportedQuantumDepth()
{
    return QString(MagickQuantumDepth)
           .replace(QString("Q"), QString(""))
            .toInt();
}

bool CyanImageFormat::hasDelegate(const QString &delegate)
{
    if (delegate.isEmpty()) { return false; }
    return QString(QString::fromStdString(MagickCore::GetMagickDelegates()))
           .contains(delegate, Qt::CaseSensitive);
}

bool CyanImageFormat::hasFeature(const QString &feature)
{
    if (feature.isEmpty()) { return false; }
    return QString(QString::fromStdString(MagickCore::GetMagickFeatures()))
            .contains(feature, Qt::CaseSensitive);
}

const QString CyanImageFormat::html2Pango(const QString &html)
{
    QString tmp = html;
    QStringList list = tmp
                       .replace("<p", "<!PARA")
                       .replace("<span","<!SPAN")
                       .replace("</p>","</!PARA>")
                       .replace("</span>","</!SPAN>")
                       .replace("<body", "<!BODY")
                       .replace("</body>", "</!BODY>")
                       .split("<");

    QString markup;
    for (int i=0;i<list.size();++i) {
        QString output;
        if (list.at(i).startsWith("!BODY")) {
            QString body = list.at(i);
            QRegularExpression rxFontSize("(?<=font-size:)[^;]*");
            QRegularExpressionMatch matchFontSize = rxFontSize.match(body);
            QString fontSize = matchFontSize.captured().replace("'","").trimmed();

            QRegularExpression rxFontStyle("(?<=font-style:)[^;]*");
            QRegularExpressionMatch matchFontStyle = rxFontStyle.match(body);
            QString fontStyle = matchFontStyle.captured().replace("'","").trimmed();

            QRegularExpression rxFontFamily("(?<=font-family:)[^;]*");
            QRegularExpressionMatch matchFontFamily = rxFontFamily.match(body);
            QString fontFamily = matchFontFamily.captured().replace("'","");

            QRegularExpression rxFontWeight("(?<=font-weight:)[^;]*");
            QRegularExpressionMatch matchFontWeight = rxFontWeight.match(body);
            QString fontWeight = matchFontWeight.captured().replace("'","").trimmed();

            output = "<span";
            if (!fontSize.isEmpty()) { output.append(QString(" font='%1'").arg(fontSize.replace("pt","").toDouble())); }
            if (!fontStyle.isEmpty()) { output.append(QString(" font_style='%1'").arg(fontStyle)); }
            if (!fontFamily.isEmpty()) { output.append(QString(" font_family='%1'").arg(fontFamily)); }
            if (!fontWeight.isEmpty()) { output.append(QString(" font_weight='%1'").arg(fontWeight)); }
            output.append(">");
        } else if(list.at(i).startsWith("/!BODY")) {
            output = "</span>";
        } else if (list.at(i).startsWith("!PARA")) {
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

/*const QString CyanImageFormat::canvasTitleWithSpecs(Magick::Image image)
{
    // filename @ colorspace widthxheight dpixdpi bit
    QString result;
    try {
        QString label = QObject::tr("Canvas");
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
}*/
