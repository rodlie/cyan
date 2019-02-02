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

#include "cyan_layertreeitem.h"

CyanLayerTreeItem::CyanLayerTreeItem(QTreeWidget *parent) :
    QTreeWidgetItem(parent)
  , _composite(Magick::OverCompositeOp)
  , _id(0)
  , _name(tr("New Layer"))
  , _visible(true)
  , _locked(false)
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
