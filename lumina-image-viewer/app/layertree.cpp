#include "layertree.h"
#include <QDebug>
#include <QTreeWidgetItem>

LayerTree::LayerTree(QWidget *parent) : QTreeWidget(parent)
{
}

LayerTree::~LayerTree()
{

}

void LayerTree::handleTabActivated(QMdiSubWindow *tab)
{
    qDebug() << "tab activated";
    if (!tab) { return; }
    View *view = qobject_cast<View*>(tab->widget());
    if (!view) { return; }
    populateTree(view);
}

void LayerTree::populateTree(View *image)
{
    if (!image) { return; }
    this->clear();
    for (int i=0;i<image->getLayerCount();++i) {
        qDebug() << "adding layer" << image->getLayerName(i);
        QTreeWidgetItem *item = new QTreeWidgetItem(this);
        item->setText(0,image->getLayerName(i));
        item->setData(0, Qt::UserRole+1, i);
        item->setData(0, Qt::UserRole+2, 0);
        item->setToolTip(0, tr("Layer %1").arg(i));
        this->addTopLevelItem(item);
        if (i==0) { setCurrentItem(item); }
    }
}
