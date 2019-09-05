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

#include "CyanLayerTreeItem.h"

CyanLayerTreeItem::CyanLayerTreeItem(QTreeWidget *parent) :
    QTreeWidgetItem(parent)
  , _composite(Magick::OverCompositeOp)
  , _id(0)
  , _name(tr("New Layer"))
  , _visible(true)
  , _locked(false)
  , _order(0)
{
}

CyanLayerTreeItem::~CyanLayerTreeItem()
{
}

Magick::CompositeOperator CyanLayerTreeItem::getComposite()
{
    return _composite;
}

void CyanLayerTreeItem::setComposite(Magick::CompositeOperator composite)
{
    if (composite == Magick::UndefinedCompositeOp ||
        composite == Magick::NoCompositeOp) { return; }
    _composite = composite;
}

int CyanLayerTreeItem::getLayerID()
{
    return _id;
}

void CyanLayerTreeItem::setLayerID(int id)
{
    if (id<0) { return; }
    _id = id;
}

int CyanLayerTreeItem::getLayerOrder()
{
    return _order;
}

void CyanLayerTreeItem::setLayerOrder(int order)
{
    _order = order;
}

QString CyanLayerTreeItem::getLayerName()
{
    return _name;
}

void CyanLayerTreeItem::setLayerName(QString name)
{
    if (name.isEmpty()) { return; }
    _name = name;
}

double CyanLayerTreeItem::getOpacity()
{
    return _opacity;
}

void CyanLayerTreeItem::setOpacity(double value)
{
    _opacity = value;
}

bool CyanLayerTreeItem::getVisibility()
{
    return _visible;
}

void CyanLayerTreeItem::setVisibility(bool visible)
{
    _visible = visible;
}

bool CyanLayerTreeItem::getLock()
{
    return _locked;
}

void CyanLayerTreeItem::setLock(bool lock)
{
    _locked = lock;
}
