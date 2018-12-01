/*
# Copyright or © or Copr. Ole-André Rodlie, INRIA.
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

#include "imageview.h"
#include <QMimeData>
#include <QMimeDatabase>
#include <QMimeType>

ImageView::ImageView(QWidget* parent) : QGraphicsView(parent)
, fit(false) {
    setAcceptDrops(true);
    setBackgroundBrush(QColor(30,30,30));
    setDragMode(QGraphicsView::ScrollHandDrag);
}

void ImageView::wheelEvent(QWheelEvent* event) {
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

void ImageView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        fit = false;
        emit resetZoom();
    } else {
        if (event->button() == Qt::RightButton) {
            setFit(true);
        } else {
            QGraphicsView::mousePressEvent(event);
        }
    }
}

void ImageView::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void ImageView::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void ImageView::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void ImageView::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (!mimeData->hasUrls()) { return; }
    if (mimeData->urls().at(0).isEmpty()) { return; }
    QUrl url = mimeData->urls().at(0);
    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(url.toLocalFile());
    if (type.name().startsWith("image")) { emit openImage(url.toLocalFile()); }
    else if (type.name().contains("iccprofile")) { emit openProfile(url.toLocalFile()); }
}

void ImageView::resizeEvent(QResizeEvent */*event*/)
{
    if (fit) {
        fitInView(0,
                  0,
                  scene()->width(),
                  scene()->height(),
                  Qt::KeepAspectRatio);
    }
}

void ImageView::doZoom(double scaleX, double scaleY)
{
    scale(scaleX,scaleY);
}

void ImageView::setFit(bool value)
{
    fit = value;
    fitInView(0,
              0,
              scene()->width(),
              scene()->height(),
              Qt::KeepAspectRatio);
}
