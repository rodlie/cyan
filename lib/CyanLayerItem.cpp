/*
#
# Cyan Library <https://cyan.fxarena.net>
#
# Copyright Ole-André Rodlie, FxArena DA.
#
# ole.andre.rodlie@gmail.com / support@fxarena.net
#
# This software is governed either by the CeCILL or the CeCILL-C license
# under French law and abiding by the rules of distribution of free software.
# You can  use, modify and or redistribute the software under the terms of
# the CeCILL or CeCILL-C licenses as circulated by CEA, CNRS and INRIA
# at the following URL: "http://www.cecill.info".
#
# As a counterpart to the access to the source code and  rights to copy,
# modify and redistribute granted by the license, users are provided only
# with a limited warranty  and the software's author,  the holder of the
# economic rights,  and the successive licensors  have only  limited
# liability.
#
# In this respect, the user's attention is drawn to the risks associated
# with loading,  using,  modifying and/or developing or reproducing the
# software by the user in light of its specific status of free software,
# that may mean  that it is complicated to manipulate,  and  that  also
# therefore means  that it is reserved for developers  and  experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to load and test the software's suitability as regards their
# requirements in conditions enabling the security of their systems and/or
# data to be ensured and,  more generally, to use and operate it in the
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL and CeCILL-C licenses and that you accept its terms.
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

    if (pen().color() != Qt::green) {
        QPen newPen(Qt::green);
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
    if (!mouseIsDown) { emit movedItem(this->pos(), data(1).toInt()); }
}

void LayerItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if (pen().color() != Qt::green) {
        QPen newPen(Qt::green);
        newPen.setWidth(0);
        setPen(newPen);
    }
    if (_locked) { return; }
    QGraphicsItem::hoverEnterEvent(event);
}

void LayerItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if (pen().color() == Qt::green) { setPen(Qt::NoPen); }
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
