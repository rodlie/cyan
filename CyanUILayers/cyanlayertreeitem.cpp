#include "cyanlayertreeitem.h"

CyanLayerTreeItem::CyanLayerTreeItem(QTreeWidget *parent) :
    QTreeWidgetItem(parent)
  , _composite(MagickCore::OverCompositeOp)
  , _id(0)
  , _name(tr("New Layer"))
  , _visible(true)
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
    if (composite == MagickCore::UndefinedCompositeOp ||
        composite == MagickCore::NoCompositeOp) { return; }
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
