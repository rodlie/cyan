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

#include "CyanLayerItem.h"
#include <QPen>

LayerItem::LayerItem(QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
    , mouseIsDown(false)
    , _movable(false)
    , _drag(false)
    , _draw(false)
    , _locked(false)
{
    setAcceptHoverEvents(true);
    setPen(Qt::NoPen);
}

void LayerItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (_locked) { return; }
    if (_drag) {
        QGraphicsItem::mousePressEvent(event);
        return;
    }

    lpos = mapToScene(event->pos());
    llpos = this->pos();
    fpos.setX(lpos.x()-llpos.x());
    fpos.setY(lpos.y()-llpos.y());

    mouseIsDown = true;
    emit selectedItem(data(1).toInt());

    if (pen().color() != Qt::cyan) {
        QPen newPen(Qt::cyan);
        newPen.setWidth(0);
        setPen(newPen);
    }
}

void LayerItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (_locked) { return; }
    if (_drag) { QGraphicsItem::mouseReleaseEvent(event); }
    mouseIsDown = false;
    if (_movable) { mouseMoveEvent(event); }
}

void LayerItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (_drag) {
        QGraphicsItem::mouseMoveEvent(event);
        return;
    }

    QPointF epos = mapToScene(event->pos());
    if ((_draw && mouseIsDown) || !_movable || _locked || (epos==lpos)) {
        return;
    }

    epos.setX(epos.x()-fpos.x());
    epos.setY(epos.y()-fpos.y());

    this->setPos(epos);
    lpos = epos;

    emit movingItem(this->pos(), data(1).toInt());
    if (!mouseIsDown) { emit movedItem(this->pos(), llpos, data(1).toInt()); }
}

void LayerItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if (pen().color() != Qt::cyan) {
        QPen newPen(Qt::cyan);
        newPen.setWidth(0);
        setPen(newPen);
    }
    if (_locked) { return; }
    QGraphicsItem::hoverEnterEvent(event);
}

void LayerItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if (pen().color() == Qt::cyan) { setPen(Qt::NoPen); }
    if (_locked) { return; }
    QGraphicsItem::hoverLeaveEvent(event);
}

void LayerItem::setLock(bool lock)
{
    _locked = lock;
}

void LayerItem::setLock(LayerItem *layer, bool lock)
{
    if (layer == this) { setLock(lock); }
}

void LayerItem::setMovable(bool movable)
{
    _movable = movable;
}

void LayerItem::setMovable(LayerItem *layer, bool movable)
{
    if (layer == this) { setMovable(movable); }
}

bool LayerItem::isMovable()
{
    return _movable;
}

bool LayerItem::isLocked()
{
    return _locked;
}

/*bool LayerItem::isDrag()
{
    return _drag;
}*/

/*void LayerItem::setDrag(bool drag)
{
    _drag = drag;
}*/

int LayerItem::getID()
{
    return data(1).toInt();
}

bool LayerItem::isDrawing()
{
    return _draw;
}

void LayerItem::setDraw(bool draw)
{
    _draw = draw;
    setMovable(!draw);
}
