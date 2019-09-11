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

#include "CyanGuideItem.h"
#include <QPen>
#include <QDebug>
#include <QGraphicsScene>

CyanGuideItem::CyanGuideItem(QGraphicsItem *parent,
                             bool isHorizontal)
    : QGraphicsRectItem(parent)
    , _isHoriz(isHorizontal)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setAcceptHoverEvents(true);
    QPen newPen(Qt::magenta);
    newPen.setWidth(0);
    setPen(newPen);
}

void CyanGuideItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
}

void CyanGuideItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}

void CyanGuideItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF epos = mapToScene(event->pos());
    if (_isHoriz) { epos.setX(0); }
    else { epos.setY(0); }
    setPos(epos);
    emit movedItem();
}

void CyanGuideItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if (pen().color() != Qt::cyan) {
        QPen newPen(Qt::cyan);
        newPen.setWidth(0);
        setPen(newPen);
    }
    QGraphicsItem::hoverEnterEvent(event);
}

void CyanGuideItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if (pen().color() != Qt::magenta) {
        QPen newPen(Qt::magenta);
        newPen.setWidth(0);
        setPen(newPen);
    }
    QGraphicsItem::hoverLeaveEvent(event);
}
