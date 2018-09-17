/*
#
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Cyan is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.
#
# Cyan is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Cyan.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>
#
*/

#ifndef VIEW_H
#define VIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QResizeEvent>
#include <QGraphicsScene>
#include <Magick++.h>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QPainter>
#include "common.h"

class LayerItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

signals:
    void movedItem(QPointF pos, int layerID);
    void cachePixmap(int layerID);
    void selectedItem(int LayerID);

private:
    bool mouseIsDown = false;
    QPixmap _pixmap;
    bool _movable = false;
    QPointF lpos;
    bool _drag = false;
    void mousePressEvent(QGraphicsSceneMouseEvent *event)
    {
        if (_drag) {
            QGraphicsItem::mousePressEvent(event);
            return;
        }

        lpos = mapToScene(event->pos());
        mouseIsDown = true;
        setCursor(QCursor(Qt::ClosedHandCursor));
        emit selectedItem(data(1).toInt());
    }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
    {
        if (_drag) { QGraphicsItem::mouseReleaseEvent(event); }
        mouseIsDown = false;
        setCursor(QCursor(Qt::ArrowCursor));
        if (_movable) { mouseMoveEvent(event); }
    }
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event)
    {
        if (_drag) {
            QGraphicsItem::mouseMoveEvent(event);
            return;
        }
        if (!_movable) { return; }

        QPointF epos = mapToScene(event->pos());
        if (epos==lpos) {
            qDebug() << "same don't move" << epos << lpos;
            return;
        }

        //qDebug() << "==============>layer" << this->boundingRect().width() << this->boundingRect().height() << this->boundingRect().center();
        //qDebug() << "==============>pos" << epos.x() << epos.y();
        // TODO: offset proper!
        epos.setX(epos.x()-this->boundingRect().center().x());
        epos.setY(epos.y()-this->boundingRect().center().y());
        this->setPos(epos);
        lpos = epos;
        if (!mouseIsDown) { emit movedItem(this->pos(), data(1).toInt()); }
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        Q_UNUSED(option)
        Q_UNUSED(widget)
        if (!mouseIsDown) {
            QPen pen(Qt::red);
            painter->setPen(pen);
            painter->drawRect(rect());
        } else {
            if (!_pixmap.isNull()) {
                painter->setOpacity(0.5);
                painter->drawImage(QPoint(0,0), _pixmap.toImage());
            }
        }
    }

public slots:
    void setPixmap(QPixmap pixmap)
    {
        qDebug() << "set pixmap";
        _pixmap = pixmap;
        update();
    }
    void clearPixmap()
    {
        qDebug() << "clear pixmap";
        _pixmap = QPixmap();
        update();
    }
    void refreshPixmap()
    {
        update();
    }
    void updatePixmap()
    {
        qDebug() << "layer must update pixmap";
        emit cachePixmap(data(1).toInt());
    }
    void setMovable(bool movable)
    {
        qDebug() << "set layer movable" << movable;
        _movable = movable;
    }
    void setMovable(LayerItem *layer, bool movable)
    {
        if (layer == this) { setMovable(movable); }
    }
    bool isMovable()
    {
        return _movable;
    }
    void setDrag(bool drag)
    {
        _drag = drag;
    }
};

class View : public QGraphicsView
{
    Q_OBJECT

public:
    explicit View(QWidget* parent = NULL, int width = 640, int height = 480, int depth = 8, Magick::ColorspaceType colorspace = MagickCore::sRGBColorspace);
    ~View();
    bool fit;

private:
    Magick::Image _canvas;
    QGraphicsScene *_scene;
    QGraphicsPixmapItem *_pixmap;
    QGraphicsRectItem *_rect;
    QMap<int, Magick::Image> _layers;
    QMap<int, QSize> _layersPOS;
    QMap<int, Magick::CompositeOperator> _layersComp;
    QMap<int, bool> _layersVisibility;

signals:
    void resetZoom();
    void myZoom(double scaleX, double scaleY);
    void myFit(bool value);
    void openImage(QString filename);
    void openProfile(QString filename);
    void updatedLayers();
    void addedLayer(int layer);
    void selectedLayer(int layer);
    void updatePixmaps();
    void clearPixmaps();
    void errorMessage(QString message);
    void warningMessage(QString message);
    void statusMessage(QString message);
    void viewClosed();
    void requestComposite(Magick::Image canvas,
                          layersMap layers,
                          compMap comps,
                          posMap pos,
                          visibilityMap visibility);
    void lockLayers(bool lock);
    void lockLayer(LayerItem *layer, bool lock);
    void isDrag(bool drag);

public slots:
    void doZoom(double scaleX, double scaleY);
    void setFit(bool value);
    void resetImageZoom();
    void setLayer(Magick::Image image, int id);
    void addLayer(Magick::Image image, bool updateView = true);
    void clearLayers();
    Magick::Image getCanvas();
    void setLayerVisibility(int layer, bool layerIsVisible);
    void setLayerComposite(int layer, Magick::CompositeOperator composite);
    Magick::CompositeOperator getLayerComposite(int layer);
    int getLayerCount();
    Magick::Image getLayer(int layer);
    void setLayer(int layer, Magick::Image image);
    QSize getLayerOffset(int layer);
    void setLayerOffset(int layer, QSize offset);
    QString getLayerName(int layer);
    void setLayerName(int layer, QString name);
    void removeLayer(int layer);
    void handlePixmapRefresh();

private slots:
    void clearCanvas(int width = 640,
                     int height = 480,
                     int depth = 8,
                     Magick::ColorspaceType colorspace = MagickCore::sRGBColorspace);
    void setCanvasSpecsFromImage(Magick::Image image);
    void procLayers();
    void viewImage();
    Magick::Blob makePreview(int LayerID = -1);
    void handleLayerMoved(QPointF pos, int id);
    void handleLayerSelected(int id);
    void handleLayerCache(int id);
    void handleCompImage(Magick::Image comp);

protected:
    void wheelEvent(QWheelEvent* event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void resizeEvent(QResizeEvent *event);
};

#endif // VIEW_H
