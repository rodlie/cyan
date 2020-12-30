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

#ifndef CYANLAYERITEM_H
#define CYANLAYERITEM_H

#include <QObject>
#include <QMouseEvent>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneHoverEvent>

#include "CyanGlobal.h"

class CYANSHARED_EXPORT LayerItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:

    LayerItem(QGraphicsItem *parent = nullptr);

signals:

    void movedItem(QPointF pos,
                   QPointF lpos,
                   int layerID);
    void movingItem(QPointF pos,
                    int layerID);
    void selectedItem(int LayerID);

private:

    bool mouseIsDown;
    bool _movable;
    QPointF lpos;
    QPointF llpos;
    QPointF fpos;
    bool _drag;
    bool _draw;
    bool _locked;

protected:

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

public slots:

    void setLock(bool lock);
    void setLock(LayerItem *layer,
                 bool lock);
    void setMovable(bool movable);
    void setMovable(LayerItem *layer,
                    bool movable);
    bool isMovable();
    bool isLocked();
    //bool isDrag();
    //void setDrag(bool drag);
    int getID();
    bool isDrawing();
    void setDraw(bool draw);
};

#endif // CYANLAYERITEM_H
