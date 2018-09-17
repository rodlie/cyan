/*
#
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Cyan is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.
#
# Cyan is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Cyan.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>
#
*/

#include "layertree.h"
#include <QDebug>

LayerTreeItem::LayerTreeItem(QTreeWidget *parent) : QTreeWidgetItem(parent)
  , _composite(MagickCore::OverCompositeOp)
  , _id(0)
  , _name(tr("Untitled Layer"))
{
}

LayerTreeItem::~LayerTreeItem()
{
    qDebug() << "kill layer tree item";
}

Magick::CompositeOperator LayerTreeItem::getComposite()
{
    return _composite;
}

void LayerTreeItem::setComposite(Magick::CompositeOperator composite)
{
    if (composite == MagickCore::UndefinedCompositeOp || composite == MagickCore::NoCompositeOp) { return; }
    _composite = composite;
}

int LayerTreeItem::getLayerID()
{
    return _id;
}

void LayerTreeItem::setLayerID(int id)
{
    if (id<0) { return; }
    _id = id;
}

QString LayerTreeItem::getLayerName()
{
    return tr("Untitled Layer");
    //return _name;
}

void LayerTreeItem::setLayerName(QString name)
{
    if (name.isEmpty()) { return; }
    _name = name;
}

LayerTree::LayerTree(QWidget *parent) : QTreeWidget(parent)
{
    setHeaderHidden(true);
    setHeaderLabels(QStringList() << tr("ID") << tr("Title"));
    setSortingEnabled(true);
    //setColumnHidden(0, true);
    setColumnWidth(0, 50);
}

LayerTree::~LayerTree()
{
}

void LayerTree::handleTabActivated(QMdiSubWindow *tab)
{
    qDebug() << "handle tab activated";
    if (!tab) { return; }
    View *view = qobject_cast<View*>(tab->widget());
    if (!view) { return; }
    populateTree(view);
}

void LayerTree::populateTree(View *image)
{
    qDebug() << "populate tree";
    if (!image) { return; }
    clear();
    for (int i=0;i<image->getLayerCount();++i) {
        qDebug() << "adding layer to tree" << i << image->getLayerName(i);
        LayerTreeItem *item = new LayerTreeItem(this);
        item->setText(0, QString::number(i));
        item->setText(1,image->getLayerName(i).split("/").takeLast());
        item->setLayerID(i);
        item->setLayerName(image->getLayerName(i));
        item->setComposite(image->getLayerComposite(i));
        addTopLevelItem(item);
        if (i==0) { setCurrentItem(item); }
    }
    sortByColumn(0);
}
