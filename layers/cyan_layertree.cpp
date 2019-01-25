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

#include "cyan_layertree.h"

#include <QDebug>
#include <QLayout>
#include <QHeaderView>
#include <QIcon>

LayerTree::LayerTree(QWidget *parent) : QTreeWidget(parent)
{
    setHeaderLabels(QStringList() << QString("#") << QString("") << QString(""));
    headerItem()->setIcon(2, QIcon::fromTheme("layers"));
    headerItem()->setIcon(1, QIcon::fromTheme("eye"));
    headerItem()->setToolTip(1, tr("Layer visibility"));
    //setColumnWidth(0, 16);
    setColumnHidden(0, true);
    setColumnWidth(1, 16);
    setIconSize(QSize(32, 32));

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
    bool equalID = getCanvasID().isEmpty() && (getCanvasID() == view->getCanvasID());
    if (force) { equalID = false; }
    if (!equalID || view->getLayerCount() != topLevelItemCount()) {
        populateTree(view);
    }
}

void LayerTree::populateTree(View *view)
{
    if (!view) { return; }
    clear();
    setCanvasID(view->getCanvasID());
    QMapIterator<int, CyanCommon::Layer> layers(view->getCanvasProject().layers);
    while (layers.hasNext()) {
        layers.next();
        CyanLayerTreeItem *item = new CyanLayerTreeItem(this);
        blockSignals(true);
        Magick::Image thumb(Magick::Geometry(32, 32),
                            Magick::ColorRGB(0, 0, 0));
        thumb.read("pattern:checkerboard");
        thumb.depth(8);
        thumb.alpha(false);

        try {
            Magick::Image layer(layers.value().image);
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
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

        thumb.magick("BMP");
        Magick::Blob pix;
        thumb.write(&pix);
        QPixmap pixmap = QPixmap::fromImage(QImage::fromData(reinterpret_cast<uchar*>(const_cast<void*>(pix.data())),
                                                                                      static_cast<int>(pix.length())));
        item->setIconSize(QSize(32, 32));
        item->setIcon(2, QIcon(pixmap));

        item->setText(0, QString::number(layers.value().order));
        item->setText(2, layers.value().label);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setLayerID(layers.key());
        item->setLayerName(layers.value().label);
        item->setComposite(layers.value().composite);
        item->setOpacity(layers.value().opacity);
        item->setVisibility(layers.value().visible);
        item->setCheckState(1, layers.value().visible?Qt::Checked:Qt::Unchecked);
        addTopLevelItem(item);
        blockSignals(false);
    }
    sortByColumn(0);
}

void LayerTree::handleItemActivated(QTreeWidgetItem *item, int col)
{
    Q_UNUSED(col)
    CyanLayerTreeItem *layer = dynamic_cast<CyanLayerTreeItem*>(item);
    if (!item) { return; }
    emit selectedLayer(layer->getLayerID());
}

void LayerTree::handleItemChanged(QTreeWidgetItem *item, int col)
{
    CyanLayerTreeItem *layer = dynamic_cast<CyanLayerTreeItem*>(item);
    if (!item) { return; }
    if (col == 2) {
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
}

void LayerTree::keyPressEvent(QKeyEvent *e)
{
    emit moveLayerEvent(e);
}
