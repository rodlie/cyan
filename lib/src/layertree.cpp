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

#include "layertree.h"

#include <QDebug>
#include <QLayout>

LayerTreeItem::LayerTreeItem(QTreeWidget *parent) :
    QTreeWidgetItem(parent)
  , _composite(MagickCore::OverCompositeOp)
  , _id(0)
  , _name(tr("New Layer"))
{
}

LayerTreeItem::~LayerTreeItem()
{
}

Magick::CompositeOperator LayerTreeItem::getComposite()
{
    return _composite;
}

void LayerTreeItem::setComposite(Magick::CompositeOperator composite)
{
    if (composite == MagickCore::UndefinedCompositeOp ||
        composite == MagickCore::NoCompositeOp) { return; }
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
    return _name;
}

void LayerTreeItem::setLayerName(QString name)
{
    if (name.isEmpty()) { return; }
    _name = name;
}

double LayerTreeItem::getOpacity()
{
    return _opacity;
}

void LayerTreeItem::setOpacity(double value)
{
    _opacity = value;
}

LayerTree::LayerTree(QWidget *parent) : QTreeWidget(parent)
{
    setHeaderHidden(true);
    setHeaderLabels(QStringList() << tr("ID") << tr("Title"));
    //setSortingEnabled(true);
    //setColumnHidden(0, true);
    setColumnWidth(0, 50);

    connect(this,
            SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this,
            SLOT(handleItemActivated(QTreeWidgetItem*, int)));
    connect(this,
            SIGNAL(itemPressed(QTreeWidgetItem*,int)),
            this,
            SLOT(handleItemActivated(QTreeWidgetItem*, int)));
}

LayerTree::~LayerTree()
{
}

void LayerTree::setCanvasID(const QString &id)
{
    _canvasID = id;
}

const QString LayerTree::getCanvasID()
{
    return _canvasID;
}

void LayerTree::handleTabActivated(QMdiSubWindow *tab)
{
    if (!tab) { return; }
    View *view = qobject_cast<View*>(tab->widget());
    if (!view) { return; }
    bool equalID = getCanvasID().isEmpty() && (getCanvasID() == view->getCanvasID());
    if (!equalID || view->getLayerCount() != topLevelItemCount()) {
        populateTree(view);
    }
}

void LayerTree::populateTree(View *image)
{
    if (!image) { return; }
    clear();
    setCanvasID(image->getCanvasID());
    for (int i=0;i<image->getLayerCount();++i) {
        LayerTreeItem *item = new LayerTreeItem(this);
        item->setText(0, QString::number(i));
        item->setText(1,image->getLayerName(i));
        item->setLayerID(i);
        item->setLayerName(image->getLayerName(i));
        item->setComposite(image->getLayerComposite(i));
        item->setOpacity(image->getLayerOpacity(i));
        addTopLevelItem(item);
        if (i==0) { setCurrentItem(item); }
    }
    sortByColumn(0);
}

void LayerTree::handleItemActivated(QTreeWidgetItem *item, int col)
{
    Q_UNUSED(col)
    LayerTreeItem *layer = dynamic_cast<LayerTreeItem*>(item);
    if (!item) { return; }
    emit selectedLayer(layer->getLayerID());
}

void LayerTree::keyPressEvent(QKeyEvent *e)
{
    emit moveLayerEvent(e);
}
