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
#include "common.h"

View::View(QWidget* parent, int width, int height, int depth, Magick::ColorspaceType colorspace) : QGraphicsView(parent)
  , fit(false)
  , _scene(0)
  , _pixmap(0)
  , _rect(0)
{
    setAcceptDrops(true);
    setBackgroundBrush(QColor(100,100,100));

    //setDragMode(QGraphicsView::ScrollHandDrag);
    //setInteractive(false);

    _scene = new QGraphicsScene(this);
    setScene(_scene);
    resetImageZoom();
    connect(this, SIGNAL(resetZoom()), this, SLOT(resetImageZoom()));
    connect(this, SIGNAL(updatedLayers()), this, SLOT(procLayers()));

    _rect = new QGraphicsRectItem();
    _rect->setRect(0, 0, width, height);
    //_rect->setPen(QPen(Qt::black));
    _rect->setBrush(QBrush(Qt::darkGray));
    _scene->addItem(_rect);
    _pixmap = _scene->addPixmap(QPixmap());
    clearCanvas(width, height, depth, colorspace);
}

View::~View()
{
    emit viewClosed();
    clearLayers();
    _scene->deleteLater();
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
        setDragMode(QGraphicsView::ScrollHandDrag);
        emit isDrag(true);
        QMouseEvent fake(event->type(), event->pos(), Qt::LeftButton, Qt::LeftButton, event->modifiers());
        QGraphicsView::mousePressEvent(&fake);
    } else if (event->button() == Qt::RightButton) { emit resetZoom(); }
    else { QGraphicsView::mousePressEvent(event); }
}

void View::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        setDragMode(QGraphicsView::NoDrag);
        QMouseEvent fake(event->type(), event->pos(), Qt::LeftButton, Qt::LeftButton, event->modifiers());
        QGraphicsView::mouseReleaseEvent(&fake);
        emit isDrag(false);
        setCursor(QCursor(Qt::ArrowCursor));
    } else { QGraphicsView::mouseReleaseEvent(event); }
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
    qDebug() << "drop event on view";
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        qDebug() << "DROPPED ITEMS" << mimeData->urls();
        for (int i=0;i<mimeData->urls().size();++i) {
            emit openImage(mimeData->urls().at(i).toLocalFile());
        }
    }

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
    fit = false;
    QMatrix matrix;
    matrix.scale(1.0, 1.0);
    setMatrix(matrix);
}

void View::setLayer(Magick::Image image, int id)
{
    qDebug() << "set image for layer" << id;
    _layers[id] = image;

    emit updatedLayers();
}

void View::addLayer(Magick::Image image, bool updateView)
{
    int id = _layers.size();
    _layers[id] = image;
    _layersPOS[id] = QSize(0, 0);
    _layersComp[id] = MagickCore::OverCompositeOp;
    _layersVisibility[id] = true;
    if (id == 0) { setCanvasSpecsFromImage(image); }

    LayerItem *layer = new LayerItem();
    layer->setRect(0, 0, image.columns(), image.rows());
    layer->setData(1, id);
    connect(layer, SIGNAL(movedItem(QPointF,int)), this, SLOT(handleLayerMoved(QPointF,int)));
    connect(layer, SIGNAL(selectedItem(int)), this, SLOT(handleLayerSelected(int)));
    connect(layer, SIGNAL(cachePixmap(int)), this, SLOT(handleLayerCache(int)));
    connect(this, SIGNAL(updatePixmaps()), layer, SLOT(refreshPixmap()));
    connect(this, SIGNAL(clearPixmaps()), layer, SLOT(clearPixmap()));
    connect(this, SIGNAL(viewClosed()), layer, SLOT(deleteLater()));
    connect(this, SIGNAL(lockLayer(LayerItem*,bool)), layer, SLOT(setMovable(LayerItem*,bool)));
    connect(this, SIGNAL(lockLayers(bool)), layer, SLOT(setMovable(bool)));
    connect(this, SIGNAL(isDrag(bool)), layer, SLOT(setDrag(bool)));

    _scene->addItem(layer);
    layer->updatePixmap();
    layer->setMovable(true);

    emit addedLayer(id);
    if (updateView) { emit updatedLayers(); }
}

void View::clearLayers()
{
    _layers.clear();
    _layersComp.clear();
    _layersPOS.clear();
    _layersVisibility.clear();
    emit updatedLayers();
}

Magick::Image View::getCanvas()
{
    return _canvas;
}

void View::setLayerVisibility(int layer, bool layerIsVisible)
{
    emit statusMessage(tr("Set layer %1 visibility to %2").arg(layer).arg(layerIsVisible?"on":"off"));
    if (_layersVisibility[layer] != layerIsVisible) {
        _layersVisibility[layer] = layerIsVisible;
        emit updatedLayers();
    }
}

void View::setLayerComposite(int layer, Magick::CompositeOperator composite)
{
    emit statusMessage(tr("Set layer %1 composite to %2").arg(layer).arg(Common::compositeModes()[composite]));
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
    _layersPOS.remove(layer);
    _layersVisibility.remove(layer);
    emit updatedLayers();
    emit statusMessage(tr("Removed layer %1").arg(layer));
}

void View::clearCanvas(int width, int height, int depth, Magick::ColorspaceType colorspace)
{

    qDebug() << "clear canvas" << width << height << depth << colorspace;
    _rect->setRect(0, 0, width, height);
    _canvas = Magick::Image();
    _canvas.size(Magick::Geometry(width, height));
    _canvas.depth(depth);
    _canvas.colorSpace(colorspace);
    _canvas.matte(true); // TODO magick7
    _canvas.backgroundColor(_canvas.pixelColor(0,0));
    _canvas.transparent(_canvas.pixelColor(0,0));
    _scene->setSceneRect(0, 0, width, height);
    if (_canvas.comment().empty()) { _canvas.comment(Common::timestamp().toStdString()); }
}

void View::setCanvasSpecsFromImage(Magick::Image image)
{
    emit statusMessage(tr("Set canvas specifications from image ..."));
    qDebug() << "set canvas specs from image" << image.columns() << image.rows() << image.depth() << image.colorSpace();
    _canvas.size(Magick::Geometry(image.columns(), image.rows()));
    _canvas.depth(image.depth());
    _canvas.colorSpace(image.colorSpace());
    _scene->setSceneRect(0, 0, image.columns(), image.rows());
    emit statusMessage(tr("Done"));
}

void View::procLayers()
{
    clearCanvas(_canvas.columns(), _canvas.rows(), _canvas.depth(), _canvas.colorspaceType());
    emit requestComposite(_canvas, _layers, _layersComp, _layersPOS, _layersVisibility);
}

void View::viewImage()
{
    emit statusMessage(tr("Generating preview ..."));
    Magick::Blob preview = makePreview();
    if (preview.length()==0) { return; }
    QPixmap pixmap(QPixmap::fromImage(QImage::fromData(QByteArray((char*)preview.data(), preview.length()))));
    if (pixmap.isNull()) { return; }
    _pixmap->setPixmap(pixmap);
    emit statusMessage(tr("Done"));
}

Magick::Blob View::makePreview(int LayerID)
{
    try {
        Magick::Image preview;
        if (LayerID>=0) { preview = getLayer(LayerID); }
        else { preview = _canvas; }
        Magick::Blob result;
        if (preview.depth()>8) { preview.depth(8); }
        preview.magick("BMP");
        preview.write(&result);
        emit updatePixmaps();
        return result;
    }
    catch(Magick::Error &error_ ) {
        emit errorMessage(error_.what());
    }
    catch(Magick::Warning &warn_ ) {
        emit warningMessage(warn_.what());
    }
    return Magick::Blob();
}

void View::handleLayerMoved(QPointF pos, int id)
{
    emit statusMessage(tr("Moved layer %1 to %2x%3").arg(id).arg(pos.x()).arg(pos.y()));
    _layersPOS[id] = QSize((int)pos.x(), (int)pos.y());
    emit updatedLayers();
}

void View::handleLayerSelected(int id)
{
    emit statusMessage(tr("Selected layer %1").arg(id));
    emit selectedLayer(id);
}

void View::handleLayerCache(int id)
{
    emit statusMessage(tr("Generating cache for layer %1...").arg(id));
    LayerItem *item = dynamic_cast<LayerItem*>(sender());
    if (!item) { return; }
    Magick::Blob cache = makePreview(id);
    QPixmap pix = QPixmap::fromImage(QImage::fromData(QByteArray((char*)cache.data(), cache.length())));
    if (!pix.isNull()) { item->setPixmap(pix); }
    emit statusMessage(tr("Done"));
}

void View::handleCompImage(Magick::Image comp)
{
    if (_canvas.comment() != comp.comment()) { return; }
    _canvas = comp;
    viewImage();
}

void View::handlePixmapRefresh()
{
    emit updatePixmaps();
}
