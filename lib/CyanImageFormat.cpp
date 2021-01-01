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
#include <MagickCore/composite.h>

CyanImageFormat::CyanImageFormat()
{
}

const QString CyanImageFormat::getVersion()
{
    return QString(CYAN_PROJECT_VERSION);
}

QMap<Magick::CompositeOperator, QPair<QString, int> > CyanImageFormat::compositeModes()
{
    // counter is used to give each mode an id for use in projects (load/save)
    // never change this order as this will break older projects! Append new modes at bottom.

    // note that we currently don't depend on the id's for projects load/save, but we will ...

    int counter = 0;
    QMap<Magick::CompositeOperator, QPair<QString, int> > result;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Normal (Over)");
        info.second = counter;
        result[Magick::CompositeOperator::OverCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Overlay");
        info.second = counter;
        result[Magick::CompositeOperator::OverlayCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Screen");
        info.second = counter;
        result[Magick::CompositeOperator::ScreenCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Dissolve");
        info.second = counter;
        result[Magick::CompositeOperator::DissolveCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Multiply");
        info.second = counter;
        result[Magick::CompositeOperator::MultiplyCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Addition");
        info.second = counter;
        result[Magick::CompositeOperator::PlusCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Minus Src");
        info.second = counter;
        result[Magick::CompositeOperator::MinusSrcCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Minus Dst");
        info.second = counter;
        result[Magick::CompositeOperator::MinusDstCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Divide Src");
        info.second = counter;
        result[Magick::CompositeOperator::DivideSrcCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Divide Dst");
        info.second = counter;
        result[Magick::CompositeOperator::DivideDstCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Difference");
        info.second = counter;
        result[Magick::CompositeOperator::DifferenceCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("XOR");
        info.second = counter;
        result[Magick::CompositeOperator::XorCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Burn");
        info.second = counter;
        result[Magick::CompositeOperator::ColorBurnCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Dodge");
        info.second = counter;
        result[Magick::CompositeOperator::ColorDodgeCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Hard light");
        info.second = counter;
        result[Magick::CompositeOperator::HardLightCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Lighten only");
        info.second = counter;
        result[Magick::CompositeOperator::LightenCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Lighten intensity");
        info.second = counter;
        result[Magick::CompositeOperator::LightenIntensityCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Darken only");
        info.second = counter;
        result[Magick::CompositeOperator::DarkenCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Darken intensity");
        info.second = counter;
        result[Magick::CompositeOperator::DarkenIntensityCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Vivid light");
        info.second = counter;
        result[Magick::CompositeOperator::VividLightCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Pin light");
        info.second = counter;
        result[Magick::CompositeOperator::PinLightCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Soft light");
        info.second = counter;
        result[Magick::CompositeOperator::SoftLightCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Pegtop light");
        info.second = counter;
        result[Magick::CompositeOperator::PegtopLightCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Copy Black");
        info.second = counter;
        result[Magick::CompositeOperator::CopyBlackCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Copy Blue");
        info.second = counter;
        result[Magick::CompositeOperator::CopyBlueCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Copy Cyan");
        info.second = counter;
        result[Magick::CompositeOperator::CopyCyanCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Copy Green");
        info.second = counter;
        result[Magick::CompositeOperator::CopyGreenCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Copy Magenta");
        info.second = counter;
        result[Magick::CompositeOperator::CopyMagentaCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Copy Red");
        info.second = counter;
        result[Magick::CompositeOperator::CopyRedCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Copy Yellow");
        info.second = counter;
        result[Magick::CompositeOperator::CopyYellowCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Hue");
        info.second = counter;
        result[Magick::CompositeOperator::HueCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Saturation");
        info.second = counter;
        result[Magick::CompositeOperator::SaturateCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Substract");
        info.second = counter;
        result[Magick::CompositeOperator::MinusSrcCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Divide");
        info.second = counter;
        result[Magick::CompositeOperator::DivideSrcCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Burn Linear");
        info.second = counter;
        result[Magick::CompositeOperator::LinearBurnCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Dodge Linear");
        info.second = counter;
        result[Magick::CompositeOperator::LinearDodgeCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Threshold");
        info.second = counter;
        result[Magick::CompositeOperator::ThresholdCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Replace");
        info.second = counter;
        result[Magick::CompositeOperator::ReplaceCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Modulate");
        info.second = counter;
        result[Magick::CompositeOperator::ModulateCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Luminize");
        info.second = counter;
        result[Magick::CompositeOperator::LuminizeCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Linear Light");
        info.second = counter;
        result[Magick::CompositeOperator::LinearLightCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Hard Mix");
        info.second = counter;
        result[Magick::CompositeOperator::HardMixCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Exclusion");
        info.second = counter;
        result[Magick::CompositeOperator::ExclusionCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Alpha");
        info.second = counter;
        result[Magick::CompositeOperator::AlphaCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Blend");
        info.second = counter;
        result[Magick::CompositeOperator::BlendCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Blur");
        info.second = counter;
        result[Magick::CompositeOperator::BlurCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Bumpmap");
        info.second = counter;
        result[Magick::CompositeOperator::BumpmapCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Change Mask");
        info.second = counter;
        result[Magick::CompositeOperator::ChangeMaskCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Clear");
        info.second = counter;
        result[Magick::CompositeOperator::ClearCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Copy");
        info.second = counter;
        result[Magick::CompositeOperator::CopyCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Copy Alpha");
        info.second = counter;
        result[Magick::CompositeOperator::CopyAlphaCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Displace");
        info.second = counter;
        result[Magick::CompositeOperator::DisplaceCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Distort");
        info.second = counter;
        result[Magick::CompositeOperator::DistortCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Dst Atop");
        info.second = counter;
        result[Magick::CompositeOperator::DstAtopCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Dst");
        info.second = counter;
        result[Magick::CompositeOperator::DstCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Dst In");
        info.second = counter;
        result[Magick::CompositeOperator::DstInCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Dst Out");
        info.second = counter;
        result[Magick::CompositeOperator::DstOutCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Dst Over");
        info.second = counter;
        result[Magick::CompositeOperator::DstOverCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("In");
        info.second = counter;
        result[Magick::CompositeOperator::InCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Mathematics");
        info.second = counter;
        result[Magick::CompositeOperator::MathematicsCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Modulus");
        info.second = counter;
        result[Magick::CompositeOperator::ModulusAddCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Modulus Sustract");
        info.second = counter;
        result[Magick::CompositeOperator::ModulusSubtractCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Src Atop");
        info.second = counter;
        result[Magick::CompositeOperator::SrcAtopCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Src");
        info.second = counter;
        result[Magick::CompositeOperator::SrcCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Src In");
        info.second = counter;
        result[Magick::CompositeOperator::SrcInCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Src Out");
        info.second = counter;
        result[Magick::CompositeOperator::SrcOutCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Src Over");
        info.second = counter;
        result[Magick::CompositeOperator::SrcOverCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Stereo");
        info.second = counter;
        result[Magick::CompositeOperator::StereoCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Freeze");
        info.second = counter;
        result[Magick::CompositeOperator::FreezeCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Interpolate");
        info.second = counter;
        result[Magick::CompositeOperator::InterpolateCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Negate");
        info.second = counter;
        result[Magick::CompositeOperator::NegateCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Reflect");
        info.second = counter;
        result[Magick::CompositeOperator::ReflectCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Soft Burn");
        info.second = counter;
        result[Magick::CompositeOperator::SoftBurnCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Soft Doge");
        info.second = counter;
        result[Magick::CompositeOperator::SoftDodgeCompositeOp] = info;
    }
    counter++;
    {
        QPair<QString, int> info;
        info.first = QObject::tr("Stamp");
        info.second = counter;
        result[Magick::CompositeOperator::StampCompositeOp] = info;
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
    if (!QFile::exists(filename)) { return false; }
    try {
        Magick::Image image;
        image.quiet(true);
        image.ping(filename.toStdString());
        double project = QString::fromStdString(image.attribute(QString(CYAN_IMAGE_FORMAT)
                                                                .toStdString())).toDouble();
        if (project>0) { return true; }
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) {
        qDebug() << warn_.what();
    }
    return false;
}

bool CyanImageFormat::isValidImage(const QString &filename)
{
    if (!QFile::exists(filename)) { return false; }
    try {
        Magick::Image image;
        image.quiet(false);
        image.ping(filename.toStdString());
        return image.isValid();
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) {
        qDebug() << warn_.what();
    }
    return false;
}

int CyanImageFormat::hasLayers(const QString &filename)
{
    if (!QFile::exists(filename)) { return 0; }
    try {
        std::list<Magick::Image> layers;
        Magick::pingImages(&layers,
                           filename.toStdString());
        return static_cast<int>(layers.size());
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) {
        qDebug() << warn_.what();
    }
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
                          QString("%1").arg(CYAN_PROJECT_VERSION)
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
        QString guide = QString("%1x%2")
                .arg(i.value()->pos().x())
                .arg(i.value()->pos().y());
        if (i.value()->isHorizontal()) {
            hguides << guide;
        } else { vguides << guide; }
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
                                QString("%1").arg(CYAN_PROJECT_VERSION)
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
                            QString("%1").arg(CYAN_PROJECT_VERSION)
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

bool CyanImageFormat::writePSD(CyanImageFormat::CyanCanvas canvas,
                               const QString &filename,
                               Magick::CompressionType compress)
{
    if (filename.isEmpty() || !canvas.image.isValid()) { return false; }
    std::list<Magick::Image> images;
    Magick::Image image(renderCanvasToImage(canvas));
    image.magick("PSD");

    // compress(?)
    try { image.compressType(compress); }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); return false; }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    // add color profile
    try { image.profile("ICC", canvas.profile); }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); return false; }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    // add comp as first image
    images.push_back(image);

    // add layers
    QList<QPair<int, CyanLayer> > slayers;
    QMapIterator<int, CyanLayer> layers(canvas.layers);
    while (layers.hasNext()) {
        layers.next();
        QPair<int, CyanLayer> pair(layers.value().order,
                             layers.value());
        slayers.append(pair);
    }
    std::sort(slayers.begin(),
              slayers.end(),
              CyanImageFormat::QPairSortFirst());
    for (int i=0;i<slayers.size();++i) {
        CyanLayer slayer = slayers.at(i).second;
        Magick::Image layer(slayers.at(i).second.image);
        layer.magick("PSD");

        // add position
        try {
            Magick::Geometry layerGeo(image.columns(),
                                      image.rows(),
                                      slayer.position.width(),
                                      slayer.position.height());
            layer.page(layerGeo);
        }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); return false; }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // add compose mode
        layer.compose(slayer.composite); // does not seem to work?

        // add opacity? possible?

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

    if (QFile::exists(filename)) {
        qDebug() << "GOOD JOB!";
        return true;
    }
    // TODO add more checks
    qDebug() << "THIS SHOULD NOT HAPPEN!";
    return false;
}

CyanImageFormat::CyanCanvas CyanImageFormat::readCanvas(const QString &filename)
{
    CyanCanvas canvas;
    canvas.filename = filename;

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
            canvas.image.fileName(canvas.filename.toStdString());

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

CyanImageFormat::CyanCanvas CyanImageFormat::readUnknownCanvas(const QString &filename)
{
    qDebug() << "read unknown canvas, result may vary!" << filename;
    CyanCanvas canvas;
    canvas.filename = filename;

    if (isValidCanvas(filename) || hasLayers(filename)<=1) { return canvas; }

    // read images
    std::list<Magick::Image> images;
    try {
        Magick::readImages(&images,
                           filename.toStdString());
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

    int order = 0;
    for (std::list<Magick::Image>::iterator it = images.begin(); it != images.end(); ++it){

        if (order==0) { // this is our canvas
            canvas.image = *it;

            // decompress
            try { canvas.image.compressType(Magick::NoCompression); }
            catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
            catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

            // set label
            canvas.image.fileName(canvas.filename.toStdString());
            canvas.label = QString::fromStdString(canvas.image.fileName());

            // set profile
            canvas.profile = canvas.image.iccColorProfile();

            // bump layer (we want to include this canvas as our first layer)
            //order++;
        }

        CyanLayer layer;
        layer.image = *it;
        layer.composite = layer.image.compose();

        // TODO tmp workaround:
        if (layer.composite == Magick::DifferenceCompositeOp) {
            layer.composite = Magick::ExclusionCompositeOp;
        }

        layer.label = QString::fromStdString(layer.image.label());
        if (layer.label.isEmpty()) { layer.label = QString("Layer %1").arg(order); }
        layer.order = order;
        layer.position = QSize(layer.image.page().xOff(),
                               layer.image.page().yOff());

        // decompress
        try { layer.image.compressType(Magick::NoCompression); }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        // strip
        layer.image.strip();

        // add
        canvas.layers.insert(canvas.layers.size(),
                             layer);
        order++;
    }

    return canvas;
}

CyanImageFormat::CyanCanvas CyanImageFormat::readImage(const QString &filename)
{
    CyanCanvas canvas;
    canvas.filename = filename;
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
            //layer.image.defineValue("pango", "hinting", "full");
            if (layer.textRotate!=0) {
                layer.image.defineValue("pango",
                                        "rotate",
                                        QString::number(layer.textRotate).toStdString());
            }
            layer.image.read(QString("PANGO: %1").arg(markup).toUtf8().toStdString());
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
    //qDebug() << "comp layers" << crop.width() << crop.height();
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
        layer.quiet(true);

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
        /*if (layers[id].composite != Magick::OverCompositeOp &&
                layers[id].composite != Magick::DstOutCompositeOp &&
                layers[id].composite != Magick::XorCompositeOp)
        {
            try {
                Magick::Image clone(comp);
                clone.quiet(true);
                clone.composite(layer,
                                offsetX,
                                offsetY,
                                layers[id].composite);
                clone.alpha(false);
                layer = clone;
                offsetX=0;
                offsetY=0;
                // override layer composite operator
                layerComp = Magick::InCompositeOp;
            }
            catch(Magick::Error &error_ ) { qDebug() << error_.what(); }
            catch(Magick::Warning &warn_ ) { qDebug() << warn_.what(); }
        }*/

        // TEMP WORKAROUND!
        if (layer.columns()==1 && layer.rows()==1) {
            //continue;
            layer.alpha(true);
            layer.evaluate(Magick::AlphaChannel,
                           Magick::MultiplyEvaluateOperator,
                           0.0);
        }

        // comp layer over canvas
        try {
            //qDebug() << "LAYER COMP IMAGE?" << layer.columns() << layer.rows() << offsetX << offsetY;
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
    if (hasDelegate("tiff")) { found << "*.tif"; }

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
    blacklisted << "*.m2v" << "*.mpg" << "*.mpeg" << "*.mov" << "*.mkv" << "*.m4v" << "*.avi";
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
