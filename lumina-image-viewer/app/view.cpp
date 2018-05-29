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
{
    clearCanvas(width, height, depth, colorspace);

    setAcceptDrops(true);
    setBackgroundBrush(Qt::darkGray);
    setDragMode(QGraphicsView::ScrollHandDrag);
    _scene = new QGraphicsScene(this);
    setScene(_scene);
    resetImageZoom();
    connect(this, SIGNAL(resetZoom()), this, SLOT(resetImageZoom()));
    connect(this, SIGNAL(updatedLayers()), this, SLOT(procLayers()));
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
    const QMimeData *mimeData = event->mimeData();
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
    return _image;
}

void View::setImage(Magick::Image image)
{
    /*_image = image;
    viewImage();*/
    addLayer(image);
}

void View::addLayer(Magick::Image image)
{
    int layer = _layers.size();
    if (layer>0) { layer++; }
    _layers[layer] = image;
    _layersPOS[layer] = QSize(0, 0);
    _layersComp[layer] = MagickCore::OverCompositeOp;
    qDebug() << "added layer" << layer << QString::fromStdString(_layers[layer].fileName()) << _layersPOS[layer];
    if (layer == 0) { setCanvasSpecsFromImage(image); }
    emit updatedLayers();
}

void View::clearLayers()
{
    qDebug() << "clear layers";
    _layers.clear();
    emit updatedLayers();
}

void View::clearCanvas(int width, int height, int depth, Magick::ColorspaceType colorspace)
{
    qDebug() << "clear canvas" << width << height << depth << colorspace;
    Magick::Image canvas;
    canvas.size(Magick::Geometry(width, height));
    canvas.depth(depth);
    canvas.colorSpace(colorspace);
    canvas.matte(true); // TODO magick7
    //canvas.backgroundColor(_canvas.pixelColor(0,0));
    //canvas.transparent(_canvas.pixelColor(0,0));
    _canvas=canvas;
    //_canvas.backgroundColor(_canvas.pixelColor(0,0));
    //_canvas.transparent(_canvas.pixelColor(0,0));
}

void View::setCanvasSpecsFromImage(Magick::Image image)
{
    qDebug() << "set canvas specs from image" << image.columns() << image.rows() << image.depth() << image.colorSpace();
    _canvas.size(Magick::Geometry(image.columns(), image.rows()));
    _canvas.depth(image.depth());
    _canvas.colorSpace(image.colorSpace());
}

void View::procLayers()
{
    qDebug() << "proc layers";
    clearCanvas(_canvas.columns(), _canvas.rows(), _canvas.depth(), _canvas.colorspaceType());
    QMapIterator<int, Magick::Image> i(_layers);
    i.toBack();
    while (i.hasPrevious()) {
        i.previous();
        qDebug() << i.key() << QString::fromStdString(i.value().fileName());
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
    _scene->clear();
    _scene->addPixmap(pixmap);
    _scene->setSceneRect(0, 0, pixmap.width(), pixmap.height());
}

Magick::Blob View::makePreview()
{
    qDebug() << "make preview";
    try {
        Magick::Image preview = _canvas;
        Magick::Blob result;
        if (preview.depth()>8) { preview.depth(8); }
        preview.strip();
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
