/*
# Copyright Ole-André Rodlie.
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

#include "cyan_layeritem.h"
#include <QPen>

#include "cyan_common.h"
#include "cyan_tileitem.h"

LayerItem::LayerItem(QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
    , mouseIsDown(false)
    , _movable(false)
    , _drag(false)
    , _draw(false)
{
    setAcceptHoverEvents(true);
    QPen newPen(Qt::transparent);
    newPen.setWidth(0);
    setPen(newPen);
}

void LayerItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
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
}

void LayerItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (_drag) { QGraphicsItem::mouseReleaseEvent(event); }
    mouseIsDown = false;
    if (_movable) { mouseMoveEvent(event); }

    bool outOfBounds = true;
    for (int i=0;i<collidingItems().size();++i) {
        LayerItem *layer = dynamic_cast<LayerItem*>(collidingItems().at(i));
        TileItem *tile = dynamic_cast<TileItem*>(collidingItems().at(i));
        if (layer || tile) { continue; }
        outOfBounds = false;
    }
    if (outOfBounds) {
        QPen newPen(Qt::green);
        newPen.setWidth(0);
        setPen(newPen);
    }
}

void LayerItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (_drag) {
        QGraphicsItem::mouseMoveEvent(event);
        return;
    }
    QPointF epos = mapToScene(event->pos());

    if ((_draw && mouseIsDown) || !_movable || (epos==lpos)) {
        return;
    }

    epos.setX(epos.x()-fpos.x());
    epos.setY(epos.y()-fpos.y());

    this->setPos(epos);
    lpos = epos;

    emit movingItem(this->pos(), data(1).toInt());
    if (!mouseIsDown) { emit movedItem(this->pos(), data(1).toInt()); }
}

void LayerItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QPen newPen(Qt::green);
    newPen.setWidth(0);
    setPen(newPen);
    QGraphicsItem::hoverEnterEvent(event);
}

void LayerItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    bool outOfBounds = true;
    for (int i=0;i<collidingItems().size();++i) {
        LayerItem *layer = dynamic_cast<LayerItem*>(collidingItems().at(i));
        TileItem *tile = dynamic_cast<TileItem*>(collidingItems().at(i));
        if (layer || tile) { continue; }
        outOfBounds = false;
    }
    if (outOfBounds) {
        QPen newPen(Qt::cyan);
        newPen.setWidth(0);
        setPen(newPen);
    } else {
        QPen newPen(Qt::transparent);
        newPen.setWidth(0);
        setPen(newPen);
    }
    QGraphicsItem::hoverLeaveEvent(event);
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
