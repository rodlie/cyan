#include "view.h"
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include <QDebug>
#include <QPixmap>
#include <QImage>

View::View(QWidget* parent) : QGraphicsView(parent)
  , fit(false)
  , _scene(0)
{
    setAcceptDrops(true);
    setBackgroundBrush(Qt::darkGray);
    setDragMode(QGraphicsView::ScrollHandDrag);
    _scene = new QGraphicsScene(this);
    setScene(_scene);
    resetImageZoom();
    connect(this, SIGNAL(resetZoom()), this, SLOT(resetImageZoom()));
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
    if (mimeData->hasUrls()) {
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
    }
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
    _image = image;
    viewImage();
}

void View::viewImage()
{
    if (_image.rows()==0 || _image.columns()==0) { return; }
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
    try {
        Magick::Image preview = _image;
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
