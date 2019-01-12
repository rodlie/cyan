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

#include "render.h"

#include <QDebug>
#include <QFile>

Render::Render(QObject *parent) : QObject (parent)
{

}

Magick::Image Render::compLayers(Magick::Image canvas,
                                 QMap<int, Common::Layer> layers,
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
    QMapIterator<int, Common::Layer> layer(layers);
    while (layer.hasNext()) {
        layer.next();
        QPair<int, int> pair(layer.value().order,
                             layer.key());
        order.append(pair);
    }
    std::sort(order.begin(),
              order.end(),
              QPairSortFirst());
    qDebug() << "LAYERS?" << layers.size();
    qDebug() << "COMP ORDER:" << order;

    for (int i=0;i<order.size();++i) {
        // get layer id
        int id = order.at(i).second;

        // copy layer
        Magick::Image layer(layers[id].image);

        // get layer offset
        int offsetX = layers[id].pos.width();
        int offsetY = layers[id].pos.height();

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

Magick::Image Render::renderCanvasToImage(Common::Canvas canvas)
{
    return compLayers(canvas.image,
                      canvas.layers);
}

bool Render::renderCanvasToFile(Common::Canvas canvas,
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
