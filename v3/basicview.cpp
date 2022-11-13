/*
#
# Cyan - https://github.com/rodlie/cyan
#
# Copyright (c) 2020-2022 Ole-André Rodlie. All rights reserved.
# Copyright (c) 2018-2019 Ole-André Rodlie, FxArena. All rights reserved.
# Copyright (c) 2016-2017 Ole-André Rodlie, INRIA. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
#
*/

#include "cyan.h"

#include <QMimeData>
#include <QPixmap>
#include <QImage>
#include <QDebug>

using namespace Cyan;

BasicView::BasicView(QWidget* parent,
                     bool fit,
                     bool native)
    : QGraphicsView(parent)
    , _fit(fit)
    , _native(native)
{
    setAcceptDrops(true);
    if (!_native) { setBackgroundBrush( QColor(30, 30, 30) ); }
    setDragMode(QGraphicsView::ScrollHandDrag);
}

bool
BasicView::isFit()
{
    return _fit;
}

void
BasicView::wheelEvent(QWheelEvent* e)
{
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    double scaleFactor = 1.15;
    if(e->angleDelta().y() > 0) { // up
        _fit = false;
        scale(scaleFactor, scaleFactor);
        emit zoomChanged(scaleFactor, scaleFactor);
    } else { // down
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        emit zoomChanged(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}

void
BasicView::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::MiddleButton) {
        _fit = false;
        emit resetZoom();
    } else {
        if (e->button() == Qt::RightButton) { setFit(true); }
        else { QGraphicsView::mousePressEvent(e); }
    }
}

void
BasicView::dragEnterEvent(QDragEnterEvent *e)
{
    e->acceptProposedAction();
}

void
BasicView::dragMoveEvent(QDragMoveEvent *e)
{
    e->acceptProposedAction();
}

void
BasicView::dragLeaveEvent(QDragLeaveEvent *e)
{
    e->accept();
}

void
BasicView::dropEvent(QDropEvent *e)
{
    if ( e->mimeData()->hasUrls() ) { emit dropped( e->mimeData()->urls() ); }
}

void
BasicView::resizeEvent(QResizeEvent *e)
{
    if (_fit) {
        fitInView(0,
                  0,
                  scene()->width(),
                  scene()->height(),
                  Qt::KeepAspectRatio);
    }
    QGraphicsView::resizeEvent(e);
}

void
BasicView::setZoom(double scaleX,
                   double scaleY)
{
    scale(scaleX, scaleY);
}

void
BasicView::setFit(bool value)
{
    _fit = value;
    fitInView(0,
              0,
              scene()->width(),
              scene()->height(),
              Qt::KeepAspectRatio);
}

void
BasicView::setImage(const QByteArray &buffer,
                    int width,
                    int height)
{
    qDebug() << "setImage" << buffer.size() << width << height;
    if (buffer.size() < 1 || width < 1 || height < 1) { return; }
    QPixmap pixmap = QPixmap::fromImage( QImage( (uchar*)( buffer.data() ),
                                                 width,
                                                 height,
                                                 QImage::Format_RGBA8888) );
    if ( pixmap.isNull() ) { return; }
    scene()->clear();
    scene()->addPixmap(pixmap);
    scene()->setSceneRect( 0, 0, pixmap.width(), pixmap.height() );
}
