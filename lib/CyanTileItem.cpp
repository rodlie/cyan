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

#include "CyanTileItem.h"

#include <QPointF>
#include <QPen>

CyanTileItem::CyanTileItem(QGraphicsItem *parent, QGraphicsPixmapItem *pixmapItem)
    : QGraphicsRectItem(parent)
    , _pixmap(pixmapItem)
{
    QPen newPen(Qt::transparent);
    newPen.setWidth(0);
    setPen(newPen);
}

void CyanTileItem::setPixmapItem(QGraphicsPixmapItem *pixmapItem)
{
    if (!pixmapItem) { return; }
    _pixmap = pixmapItem;
}

QGraphicsPixmapItem *CyanTileItem::getPixmapItem()
{
    if (!_pixmap) { return new QGraphicsPixmapItem(); }
    else { return _pixmap; }
}

void CyanTileItem::setPixmap(const QPixmap &pixmap)
{
    if (!_pixmap) { return; }
    _pixmap->setPixmap(pixmap);
}

void CyanTileItem::setPixmap(int id, const QPixmap &pixmap)
{
    if (!_pixmap || this->data(0).toInt() != id) { return; }
    setPixmap(pixmap);
}
