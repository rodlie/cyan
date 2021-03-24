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

#include "CyanLayerTree.h"

#include <QDebug>
#include <QLayout>
#include <QHeaderView>
#include <QIcon>
#include <QAction>
#include <QMenu>

#include "CyanImageFormat.h"
#include "CyanLayerItemDelegate.h"

LayerTree::LayerTree(QWidget *parent,
                     bool native) :
    QTreeWidget(parent)
  , lastLayerSelected(-1)
  , _maxLayersOrder(0)
  , _textSupport(false)
  , newImageLayerAct(nullptr)
  , newTextLayerAct(nullptr)
  , removeLayerAct(nullptr)
  , moveUpLayerAct(nullptr)
  , moveDownLayerAct(nullptr)
  , duplicateLayerAct(nullptr)
{
    if (!native) {
        setStyleSheet("QTreeView::item:selected { background-color: rgb(33, 33, 33); } QTreeView::indicator { width: 24px; height: 24px; }");
    }
    setColumnCount(4);
    setHeaderHidden(true);
    setColumnHidden(0, true);
    setColumnWidth(1, 32);
    setColumnWidth(2, 32);
    setIconSize(QSize(32, 32));
    header()->setMaximumSectionSize(32); // QTBUG-68503
    setContextMenuPolicy(Qt::CustomContextMenu);
    setItemDelegateForColumn(1, new CyanLayerItemDelegate(this));
    setItemDelegateForColumn(2, new CyanLayerItemDelegate(this));

    newImageLayerAct = new QAction(tr("New Image Layer"), this);
    newTextLayerAct = new QAction(tr("New Text Layer"), this);
    removeLayerAct = new QAction(tr("Remove layer"), this);
    moveUpLayerAct = new QAction(tr("Move up"), this);
    moveDownLayerAct = new QAction(tr("Move down"), this);
    duplicateLayerAct = new QAction(tr("Duplicate layer"), this);

    if (!_textSupport) {
        newTextLayerAct->setDisabled(true);
    }

    newImageLayerAct->setIcon(QIcon::fromTheme("layer",
                                               QIcon::fromTheme("document-new")));
    removeLayerAct->setIcon(QIcon::fromTheme("edit-delete"));
    moveUpLayerAct->setIcon(QIcon::fromTheme("go-up"));
    moveDownLayerAct->setIcon(QIcon::fromTheme("go-down"));
    duplicateLayerAct->setIcon(QIcon::fromTheme("layers"));

    connect(newImageLayerAct,
            SIGNAL(triggered(bool)),
            this,
            SLOT(handleNewImageAct(bool)));
    connect(newTextLayerAct,
            SIGNAL(triggered(bool)),
            this,
            SLOT(handleNewTextAct(bool)));
    connect(removeLayerAct,
            SIGNAL(triggered(bool)),
            this,
            SLOT(handleRemoveLayerAct(bool)));
    connect(moveUpLayerAct,
            SIGNAL(triggered(bool)),
            this,
            SLOT(handleMoveUpLayerAct(bool)));
    connect(moveDownLayerAct,
            SIGNAL(triggered(bool)),
            this,
            SLOT(handleMoveDownLayerAct(bool)));
    connect(duplicateLayerAct,
            SIGNAL(triggered(bool)),
            this,
            SLOT(handleDuplicateLayerAct(bool)));

    connect(this,
            SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this,
            SLOT(handleItemActivated(QTreeWidgetItem*, int)));
    connect(this,
            SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this,
            SLOT(handleItemChanged(QTreeWidgetItem*, int)));
    connect(this,
            SIGNAL(itemPressed(QTreeWidgetItem*,int)),
            this,
            SLOT(handleItemActivated(QTreeWidgetItem*, int)));
    connect(this,
            SIGNAL(customContextMenuRequested(QPoint)),
            this,
            SLOT(handleContextMenu(QPoint)));
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

void LayerTree::handleTabActivated(QMdiSubWindow *tab, bool force)
{
    if (!tab) { return; }
    View *view = qobject_cast<View*>(tab->widget());
    if (!view) { return; }
    bool equalID = getCanvasID() == view->getCanvasID()?true:false;
    if (force) {
        qDebug() << "force populate layers";
        equalID = false;
    }
    if (!equalID || view->getLayerCount() != topLevelItemCount()) {
        qDebug() << "populate layers";
        populateTree(view);
    }
}

void LayerTree::setTextSupport(bool enabled)
{
    _textSupport = enabled;
    newTextLayerAct->setEnabled(enabled);
}

QList<QAction *> LayerTree::getLeftActions()
{
    QList<QAction *> list;
    list.append(newImageLayerAct);
    list.append(duplicateLayerAct);
    list.append(moveUpLayerAct);
    list.append(moveDownLayerAct);
    return list;
}

QList<QAction *> LayerTree::getRightActions()
{
    QList<QAction *> list;
    list.append(removeLayerAct);
    return list;
}

void LayerTree::populateTree(View *view)
{
    if (!view) { return; }

    CyanLayerTreeItem *currentLayer = dynamic_cast<CyanLayerTreeItem*>(currentItem());
    if (currentLayer) { // get last item selected
        lastLayerSelected = currentLayer->getLayerID();
    } else { lastLayerSelected = -1; }

    clear();
    setCanvasID(view->getCanvasID());
    _maxLayersOrder = 0;

    QMapIterator<int, CyanImageFormat::CyanLayer> layers(view->getCanvasProject().layers);
    while (layers.hasNext()) {
        layers.next();
        if (layers.value().order > _maxLayersOrder) { _maxLayersOrder = layers.value().order; }

        CyanLayerTreeItem *item = new CyanLayerTreeItem(this);
        blockSignals(true);

        Magick::Image thumb(Magick::Geometry(256, 256),
                            Magick::ColorRGB(0, 0, 0));
        try {
            thumb.quiet(true);
            thumb.read("pattern:checkerboard");
            thumb.scale(Magick::Geometry(32, 32));
            thumb.alpha(false);
        }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
        catch(Magick::Warning &warn_ ) {
            qDebug() << warn_.what();
        }

        try {
            Magick::Image layer(layers.value().image);
            layer.quiet(true);
            layer.depth(8);
            //layer.alpha(false);
            layer.scale(Magick::Geometry(32, 32));
            size_t offX = 0;
            size_t offY = 0;
            if (layer.columns()<thumb.columns()) {
                offX = (thumb.columns()-layer.columns())/2;
            }
            if (layer.rows()<thumb.rows()) {
                offY = (thumb.rows()-layer.rows())/2;
            }
            thumb.composite(layer,
                            offX,
                            offY,
                            Magick::OverCompositeOp);
        }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
        catch(Magick::Warning &warn_ ) {
            qDebug() << warn_.what();
        }

        thumb.magick("BMP");
        Magick::Blob pix;
        thumb.write(&pix);
        QPixmap pixmap = QPixmap::fromImage(QImage::fromData(reinterpret_cast<uchar*>(const_cast<void*>(pix.data())),
                                                                                      static_cast<int>(pix.length())));

        QString layerOrder = QString::number(layers.value().order);
        while (layerOrder.size()<3) {
            layerOrder.prepend("0");
        }
        item->setIconSize(QSize(32, 32));
        item->setIcon(3, QIcon(pixmap));
        item->setText(0, layerOrder);
        item->setText(3, layers.value().label);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setLayerID(layers.key());
        item->setLayerName(layers.value().label);
        item->setComposite(layers.value().composite);
        item->setOpacity(layers.value().opacity);
        item->setVisibility(layers.value().visible);
        item->setCheckState(1, layers.value().visible?Qt::Checked:Qt::Unchecked);
        item->setCheckState(2, layers.value().locked?Qt::Checked:Qt::Unchecked);
        item->setLayerOrder(layers.value().order);
        addTopLevelItem(item);
        blockSignals(false);
    }
    sortByColumn(0, Qt::DescendingOrder);

    if (lastLayerSelected>=0) { // set last item selected
        for (int i=0;i<topLevelItemCount();++i) {
            CyanLayerTreeItem *layer = dynamic_cast<CyanLayerTreeItem*>(topLevelItem(i));
            if (!layer) { continue; }
            if (layer->getLayerID()==lastLayerSelected) {
                setCurrentItem(layer);
                break;
            }
        }
    }
}

void LayerTree::handleItemActivated(QTreeWidgetItem *item, int col)
{
    if (col == 1 || col == 2) { return; } // ignore clicks on visible and locked
    CyanLayerTreeItem *layer = dynamic_cast<CyanLayerTreeItem*>(item);
    if (!item) { return; }
    emit selectedLayer(layer->getLayerID());
}

void LayerTree::handleItemChanged(QTreeWidgetItem *item, int col)
{
    CyanLayerTreeItem *layer = dynamic_cast<CyanLayerTreeItem*>(item);
    if (!item) { return; }
    if (col == 3) {
        emit layerLabelChanged(layer->getLayerID(),
                               item->text(col));
    }
    if (col == 1) {
        bool visible = false;
        if (item->checkState(col) == Qt::Checked) {
            visible = true;
        }
        emit layerVisibilityChanged(layer->getLayerID(),
                                    visible);
    }
    if (col == 2) {
        bool lock = false;
        if (item->checkState(col) == Qt::Checked) {
            lock = true;
        }
        emit layerLockChanged(layer->getLayerID(),
                              lock);
    }
}

void LayerTree::handleContextMenu(const QPoint &pos)
{
    QMenu *menu = new QMenu(this);
    menu->addAction(newImageLayerAct);
    if (_textSupport) {
        menu->addAction(newTextLayerAct);
    }

    bool hasLayer = false;
    int layerOrder,firstLayer = -1;
    CyanLayerTreeItem *layer = dynamic_cast<CyanLayerTreeItem*>(currentItem());
    if (layer) {
        layerOrder = layer->getLayerOrder();
        for (int i=0;i<topLevelItemCount();++i) {
            CyanLayerTreeItem *item = dynamic_cast<CyanLayerTreeItem*>(topLevelItem(i));
            if (!item || item==layer) { continue; }
            int itemOrder = item->getLayerOrder();
            if (itemOrder<layerOrder) { firstLayer = itemOrder; }
        }
        hasLayer = true;
    }
    if (hasLayer) {
        qDebug() << "LAYER ORDER" << layerOrder << firstLayer << _maxLayersOrder;
        menu->addSeparator();
        menu->addAction(duplicateLayerAct);
        bool canMoveUp = layerOrder<_maxLayersOrder;
        bool canMoveDown = layerOrder>firstLayer && firstLayer>-1;
        qDebug() << "MOVEUP?" << canMoveUp << "MOVEDOWN?" << canMoveDown;
        if (canMoveDown || canMoveUp) { menu->addSeparator(); }
        if (canMoveUp) { menu->addAction(moveUpLayerAct); }
        if (canMoveDown) { menu->addAction(moveDownLayerAct); }
        menu->addSeparator();
        menu->addAction(removeLayerAct);
    }

    menu->popup(viewport()->mapToGlobal(pos));
}

void LayerTree::handleNewImageAct(bool triggered)
{
    Q_UNUSED(triggered)
    emit actNewImage();
}

void LayerTree::handleNewTextAct(bool triggered)
{
    Q_UNUSED(triggered)
    emit actNewText();
}

void LayerTree::handleRemoveLayerAct(bool triggered)
{
    Q_UNUSED(triggered)
    emit actRemove();
}

void LayerTree::handleMoveUpLayerAct(bool triggered)
{
    Q_UNUSED(triggered)
    emit actMoveUp();
}

void LayerTree::handleMoveDownLayerAct(bool triggered)
{
    Q_UNUSED(triggered)
    emit actMoveDown();
}

void LayerTree::handleDuplicateLayerAct(bool triggered)
{
    Q_UNUSED(triggered)
    emit actDuplicate();
}

void LayerTree::keyPressEvent(QKeyEvent *e)
{
    qDebug() << "MOVE LAYER KEY EVENT!";
    emit moveLayerEvent(e);
}
