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
