/*
#
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
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
    void mousePressEvent(QGraphicsSceneMouseEvent *event)
    {
        mouseIsDown = true;
        //setCursor(QCursor(Qt::ClosedHandCursor));
        emit selectedItem(data(1).toInt());
        emit cachePixmap(data(1).toInt());
        Q_UNUSED(event)
    }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
    {
        mouseIsDown = false;
        //setCursor(QCursor(Qt::ArrowCursor));
        clearPixmap();
        mouseMoveEvent(event);
    }
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event)
    {
        QPointF epos = mapToScene(event->pos());
        //qDebug() << "==============>layer" << this->boundingRect().width() << this->boundingRect().height() << this->boundingRect().center();
        //qDebug() << "==============>pos" << epos.x() << epos.y();
        // TODO: offset proper!
        epos.setX(epos.x()-this->boundingRect().center().x());
        epos.setY(epos.y()-this->boundingRect().center().y());
        this->setPos(epos);
        if (!mouseIsDown) { emit movedItem(this->pos(), data(1).toInt()); }
        //else { emit cachePixmap(data(1).toInt()); }
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        Q_UNUSED(option)
        Q_UNUSED(widget)
        if (!mouseIsDown) {
            QPen pen(Qt::green);
            painter->setPen(pen);
            painter->drawRect(rect());
        }
        if (!_pixmap.isNull()) {
            painter->setOpacity(0.5);
            painter->drawImage(QPoint(0,0), _pixmap.toImage());
        }
    }
public:
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
};

class View : public QGraphicsView
{
    Q_OBJECT

public:
    explicit View(QWidget* parent = NULL, int width = 640, int height = 480, int depth = 8, Magick::ColorspaceType colorspace = MagickCore::sRGBColorspace);
    bool fit;

private:
    Magick::Image _image;
    Magick::Image _canvas;
    QGraphicsScene *_scene;
    QGraphicsPixmapItem *_pixmap;
    QMap<int, Magick::Image> _layers;
    QMap<int, QSize> _layersPOS;
    QMap<int, Magick::CompositeOperator> _layersComp;
    QMap<int, QVector<Magick::Image> > _layersHistory;
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

public slots:
    void doZoom(double scaleX, double scaleY);
    void setFit(bool value);
    void resetImageZoom();
    Magick::Image getImage();
    void setImage(Magick::Image image);
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

private slots:
    void clearCanvas(int width = 640, int height = 480, int depth = 8, Magick::ColorspaceType colorspace = MagickCore::sRGBColorspace);
    void setCanvasSpecsFromImage(Magick::Image image);
    void procLayers();
    void viewImage();
    Magick::Blob makePreview(int LayerID = -1);
    void handleLayerMoved(QPointF pos, int id);
    void handleLayerSelected(int id);
    void handleLayerCache(int id);

protected:
    void wheelEvent(QWheelEvent* event);
    void mousePressEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void resizeEvent(QResizeEvent *event);
};

#endif // VIEW_H
