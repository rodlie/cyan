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

#ifndef CYANGUIDEITEM_H
#define CYANGUIDEITEM_H

#include <QObject>
#include <QMouseEvent>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneHoverEvent>

#include "CyanGlobal.h"

class CYANSHARED_EXPORT CyanGuideItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    CyanGuideItem(QGraphicsItem *parent = nullptr,
                  bool isHorizontal = false);

signals:
    void movedItem();

private:
    bool _isHoriz;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
};

#endif // CYANGUIDEITEM_H
