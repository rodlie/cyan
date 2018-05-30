/*
#
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#
*/

#include "view.h"
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include <QDebug>
#include <QPixmap>
#include <QImage>

View::View(QWidget* parent, int width, int height, int depth, Magick::ColorspaceType colorspace) : QGraphicsView(parent)
  , fit(false)
  , _scene(0)
  , _pixmap(0)
{
    setAcceptDrops(true);
    setBackgroundBrush(Qt::darkGray);
    setDragMode(QGraphicsView::ScrollHandDrag);

    _scene = new QGraphicsScene(this);
    setScene(_scene);
    resetImageZoom();
    connect(this, SIGNAL(resetZoom()), this, SLOT(resetImageZoom()));
    connect(this, SIGNAL(updatedLayers()), this, SLOT(procLayers()));
    clearCanvas(width, height, depth, colorspace);
    _pixmap = _scene->addPixmap(QPixmap());
}

void View::wheelEvent(QWheelEvent* event) {
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    double scaleFactor = 1.15;
    if (event->delta() > 0) { // up
        fit = false;
        scale(scaleFactor, scaleFactor);
        emit myZoom(scaleFactor, scaleFactor);
    } else { // down
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        emit myZoom(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}

void View::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        fit = false;
        emit resetZoom();
    } else {
        if (event->button() == Qt::RightButton) { setFit(true); }
        else { QGraphicsView::mousePressEvent(event); }
    }
}

void View::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void View::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void View::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void View::dropEvent(QDropEvent *event)
{
    Q_UNUSED(event)
    //const QMimeData *mimeData = event->mimeData();
    /*if (mimeData->hasUrls()) {
        if (!mimeData->urls().at(0).isEmpty()) {
            QUrl url = mimeData->urls().at(0);
            QString suffix = QFileInfo(url.toLocalFile()).suffix().toUpper();
            if (suffix == "PNG"
                || suffix == "JPG"
                || suffix == "JPEG"
                || suffix == "TIF"
                || suffix == "TIFF"
                || suffix == "PSD")
            {
                emit openImage(url.toLocalFile());
            } else if (suffix == "ICC" || suffix == "ICM") {
                emit openProfile(url.toLocalFile());
            }
        }
    }*/
}

void View::resizeEvent(QResizeEvent */*event*/)
{
    if (fit) {
        fitInView(0, 0, scene()->width(), scene()->height(), Qt::KeepAspectRatio);
    }
}

void View::doZoom(double scaleX, double scaleY)
{
    scale(scaleX,scaleY);
}

void View::setFit(bool value)
{
    if (!scene()) { return; }
    fit = value;
    fitInView(0, 0, scene()->width(), scene()->height(), Qt::KeepAspectRatio);
}

void View::resetImageZoom()
{
    QMatrix matrix;
    matrix.scale(1.0, 1.0);
    setMatrix(matrix);
}

Magick::Image View::getImage()
{
    return _canvas;
}

void View::setImage(Magick::Image image)
{
    /*_image = image;
    viewImage();*/
    addLayer(image);
}

void View::addLayer(Magick::Image image, bool updateView)
{
    int id = _layers.size();
    _layers[id] = image;
    _layersPOS[id] = QSize(0, 0);
    _layersComp[id] = MagickCore::OverCompositeOp;
    _layersVisibility[id] = true;
    qDebug() << "added layer" << id << QString::fromStdString(_layers[id].fileName()) << _layersPOS[id];
    if (id == 0) { setCanvasSpecsFromImage(image); }

    LayerItem *layer = new LayerItem(/*this*/);
    layer->setRect(0, 0, image.columns(), image.rows());
    layer->setFlag(QGraphicsItem::ItemIsMovable);
    layer->setData(1, id);
    connect(layer, SIGNAL(movedItem(QPointF,int)), this, SLOT(handleLayerMoved(QPointF,int)));
    connect(layer, SIGNAL(selectedItem(int)), this, SLOT(handleLayerSelected(int)));
    connect(layer, SIGNAL(cachePixmap(int)), this, SLOT(handleLayerCache(int)));

    _scene->addItem(layer);



    emit addedLayer(id);
    if (updateView) { emit updatedLayers(); }
}

void View::clearLayers()
{
    qDebug() << "clear layers";
    _layers.clear();
    emit updatedLayers();
}

Magick::Image View::getCanvas()
{
    return _canvas;
}

void View::setLayerVisibility(int layer, bool layerIsVisible)
{
    if (_layersVisibility[layer] != layerIsVisible) {
        _layersVisibility[layer] = layerIsVisible;
        emit updatedLayers();
    }
}

void View::setLayerComposite(int layer, Magick::CompositeOperator composite)
{
    qDebug() << "set layer composite" << layer << composite;
    if (_layersComp[layer] != composite) {
        _layersComp[layer] = composite;
        emit updatedLayers();
    }
}

Magick::CompositeOperator View::getLayerComposite(int layer)
{
    return _layersComp[layer];
}

int View::getLayerCount()
{
    return _layers.size();
}

Magick::Image View::getLayer(int layer)
{
    return _layers[layer];
}

void View::setLayer(int layer, Magick::Image image)
{
    _layers[layer] = image;
    emit updatedLayers();
}

QSize View::getLayerOffset(int layer)
{
    return _layersPOS[layer];
}

void View::setLayerOffset(int layer, QSize offset)
{
    _layersPOS[layer] = offset;
    emit updatedLayers();
}

QString View::getLayerName(int layer)
{
    return QString::fromStdString(_layers[layer].fileName());
}

void View::setLayerName(int layer, QString name)
{
    _layers[layer].fileName(name.toStdString());
    emit updatedLayers();
}

void View::removeLayer(int layer)
{
    _layers.remove(layer);
    _layersComp.remove(layer);
    _layersHistory.remove(layer);
    _layersPOS.remove(layer);
    _layersVisibility.remove(layer);
    emit updatedLayers();
}

void View::clearCanvas(int width, int height, int depth, Magick::ColorspaceType colorspace)
{
    qDebug() << "clear canvas" << width << height << depth << colorspace;
    /*Magick::Image canvas;
    canvas.size(Magick::Geometry(width, height));
    canvas.depth(depth);
    canvas.colorSpace(colorspace);
    canvas.matte(true); // TODO magick7
    canvas.backgroundColor(canvas.pixelColor(0,0));
    canvas.transparent(canvas.pixelColor(0,0));*/
    //_canvas=canvas;
    _canvas = Magick::Image();
    _canvas.size(Magick::Geometry(width, height));
    _canvas.depth(depth);
    _canvas.colorSpace(colorspace);
    _canvas.matte(true); // TODO magick7
    _canvas.backgroundColor(_canvas.pixelColor(0,0));
    _canvas.transparent(_canvas.pixelColor(0,0));
    _scene->setSceneRect(0, 0, width, height);
}

void View::setCanvasSpecsFromImage(Magick::Image image)
{
    qDebug() << "set canvas specs from image" << image.columns() << image.rows() << image.depth() << image.colorSpace();
    _canvas.size(Magick::Geometry(image.columns(), image.rows()));
    _canvas.depth(image.depth());
    _canvas.colorSpace(image.colorSpace());
    _scene->setSceneRect(0, 0, image.columns(), image.rows());
}

void View::procLayers()
{
    qDebug() << "proc layers";
    clearCanvas(_canvas.columns(), _canvas.rows(), _canvas.depth(), _canvas.colorspaceType());
    QMapIterator<int, Magick::Image> i(_layers);
    while (i.hasNext()) {
        i.next();
        qDebug() << i.key() << QString::fromStdString(i.value().fileName());
        if (!_layersVisibility[i.key()]) { continue; }
        _canvas.composite(i.value(), _layersPOS[i.key()].width(), _layersPOS[i.key()].height(), _layersComp[i.key()]);
    }
    viewImage();
}

void View::viewImage()
{
    qDebug() << "view image";
    //if (_image.rows()==0 || _image.columns()==0) { return; }
    Magick::Blob preview = makePreview();
    if (preview.length()==0) { return; }
    QPixmap pixmap(QPixmap::fromImage(QImage::fromData(QByteArray((char*)preview.data(), preview.length()))));
    if (pixmap.isNull()) { return; }
    _pixmap->setPixmap(pixmap);
}

Magick::Blob View::makePreview(int LayerID)
{
    qDebug() << "make preview";
    try {
        Magick::Image preview;
        if (LayerID>=0) { preview = getLayer(LayerID); }
        else { preview = _canvas; }
        Magick::Blob result;
        if (preview.depth()>8) { preview.depth(8); }
        //preview.strip();
        preview.magick("BMP");
        preview.write(&result);
        return result;
    }
    catch(Magick::Error &error_ ) {
        qDebug() << error_.what();
    }
    catch(Magick::Warning &warn_ ) {
        qDebug() << warn_.what();
    }
    return Magick::Blob();
}

void View::handleLayerMoved(QPointF pos, int id)
{
    qDebug() << "handle layer moved" << id << pos;
    LayerItem *item = dynamic_cast<LayerItem*>(sender());
    if (item) { item->clearPixmap(); }

    _layersPOS[id] = QSize((int)pos.x(), (int)pos.y());
    emit updatedLayers();
}

void View::handleLayerSelected(int id)
{
    qDebug() << "handle layer seleted" << id;
    emit selectedLayer(id);
}

void View::handleLayerCache(int id)
{
    qDebug() << "handle layer cache" << id;
    LayerItem *item = dynamic_cast<LayerItem*>(sender());
    if (!item) { return; }
    Magick::Blob cache = makePreview(id);
    QPixmap pix = QPixmap::fromImage(QImage::fromData(QByteArray((char*)cache.data(), cache.length())));
    if (!pix.isNull()) { item->setPixmap(pix); }
}
