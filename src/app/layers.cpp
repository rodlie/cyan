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

#include "editor.h"
#include "colorconvert.h"

#include <QMessageBox>

void Editor::handleLayerCompChanged(const QString &comp)
{
    if (comp.isEmpty()) { return; }
    LayerTreeItem *layer = dynamic_cast<LayerTreeItem*>(layersTree->currentItem());
    QMdiSubWindow *tab = mdi->currentSubWindow();
    if (!layer || !tab) { return; }
    View *view = qobject_cast<View*>(tab->widget());
    if (!view) { return; }
    QMapIterator<Magick::CompositeOperator, QString> i(Common::compositeModes());
    while (i.hasNext()) {
        i.next();
        if (i.value() == comp) {
            layer->setComposite(i.key());
            view->setLayerComposite(layer->getLayerID(), i.key());
            break;
        }
    }
}

void Editor::populateCompBox()
{
    layersComp->clear();

    /*
    gimp modes in cyan:
    normal - ok
    dissolve - ok
    lighten only - ok
    screen - ok
    dodge (color) - ok
    addition (plus) - ok
    darken only - ok
    multiply - ok
    burn (color) - ok
    overlay - ok
    soft light - ok
    hard light - ok
    difference - ok
    substract (minus src) - ok
    grain * - https://www.imagemagick.org/discourse-server/viewtopic.php?t=25085
    divide (src) - ok
    hue - ok
    saturation - ok
    color - ??? https://docs.gimp.org/en/gimp-concepts-layer-modes.html
    value - ??? https://docs.gimp.org/en/gimp-concepts-layer-modes.html
    */

    layersComp->addItem(Common::compositeModes()[MagickCore::OverCompositeOp]);
    layersComp->addItem(Common::compositeModes()[MagickCore::DissolveCompositeOp]);

    layersComp->insertSeparator(layersComp->count());

    layersComp->addItem(Common::compositeModes()[MagickCore::PlusCompositeOp]);
    layersComp->addItem(Common::compositeModes()[MagickCore::MultiplyCompositeOp]);
    layersComp->addItem(Common::compositeModes()[MagickCore::OverlayCompositeOp]);
    layersComp->addItem(Common::compositeModes()[MagickCore::ScreenCompositeOp]);

    layersComp->insertSeparator(layersComp->count());

    layersComp->addItem(Common::compositeModes()[MagickCore::ColorDodgeCompositeOp]);
    layersComp->addItem(Common::compositeModes()[MagickCore::LinearDodgeCompositeOp]);
    layersComp->addItem(Common::compositeModes()[MagickCore::ColorBurnCompositeOp]);
    layersComp->addItem(Common::compositeModes()[MagickCore::LinearBurnCompositeOp]);

    layersComp->insertSeparator(layersComp->count());

    layersComp->addItem(Common::compositeModes()[MagickCore::LightenCompositeOp]);
    layersComp->addItem(Common::compositeModes()[MagickCore::DarkenCompositeOp]);
    layersComp->addItem(Common::compositeModes()[MagickCore::LightenIntensityCompositeOp]);
    layersComp->addItem(Common::compositeModes()[MagickCore::DarkenIntensityCompositeOp]);

    layersComp->insertSeparator(layersComp->count());

    layersComp->addItem(Common::compositeModes()[MagickCore::SoftLightCompositeOp]);
    layersComp->addItem(Common::compositeModes()[MagickCore::HardLightCompositeOp]);
    layersComp->addItem(Common::compositeModes()[MagickCore::VividLightCompositeOp]);
    layersComp->addItem(Common::compositeModes()[MagickCore::PegtopLightCompositeOp]);
    layersComp->addItem(Common::compositeModes()[MagickCore::PinLightCompositeOp]);
    layersComp->addItem(Common::compositeModes()[MagickCore::LinearLightCompositeOp]);

    layersComp->insertSeparator(layersComp->count());

    layersComp->addItem(Common::compositeModes()[MagickCore::DifferenceCompositeOp]);
    layersComp->addItem(Common::compositeModes()[MagickCore::MinusSrcCompositeOp]);
    layersComp->addItem(Common::compositeModes()[MagickCore::DivideSrcCompositeOp]);

    layersComp->insertSeparator(layersComp->count());

    layersComp->addItem(Common::compositeModes()[MagickCore::HueCompositeOp]);
    layersComp->addItem(Common::compositeModes()[MagickCore::SaturateCompositeOp]);

    layersComp->insertSeparator(layersComp->count());

    QMapIterator<Magick::CompositeOperator, QString> i(Common::compositeModes());
    while (i.hasNext()) {
        i.next();
        if (layersComp->findText(i.value(),
                                  Qt::MatchExactly |
                                  Qt::MatchCaseSensitive)==-1)
        {
            layersComp->addItem(i.value());
        }
    }

    layersComp->setCurrentIndex(layersComp->findText(QString("Normal")));
}

void Editor::handleLayerActivated(QTreeWidgetItem *item,
                                  int col)
{
    Q_UNUSED(col)
    handleLayerActivated(item, item);
}

void Editor::handleLayerActivated(QTreeWidgetItem *item,
                                  QTreeWidgetItem *old)
{
    Q_UNUSED(old)
    LayerTreeItem *layer = dynamic_cast<LayerTreeItem*>(item);
    if (!layer) { return; }
    layersComp->setCurrentIndex(layersComp->findText(Common::compositeModes()[layer->getComposite()]));
    //qDebug() << "???" << layer->getOpacity() << layer->getOpacity()*100;
    layersOpacity->setValue(qRound(layer->getOpacity()*100));
}

// TODO
void Editor::handleLayerDoubleclicked(QTreeWidgetItem *item,
                                      int col)
{
    qDebug() << "layer tree item double clicked";
    Q_UNUSED(col)
    LayerTreeItem *layer = dynamic_cast<LayerTreeItem*>(item);
    View *view = qobject_cast<View*>(getCurrentView());
    if (!layer || !view) { return; }
    //newTabFromLayer(view, layer->getLayerID());
}

void Editor::handleLayerSelected(int layer)
{
    for (int i=0;i<layersTree->topLevelItemCount();++i) {
        LayerTreeItem *item = dynamic_cast<LayerTreeItem*>(layersTree->topLevelItem(i));
        if (!item) { continue; }
        if (layer == item->getLayerID()) {
            layersTree->setCurrentItem(item);
            handleLayerActivated(item, 0);
            return;
        }
    }
}

void Editor::handleLayersOpacity()
{
    double value = layersOpacity->value();
    LayerTreeItem *layer = dynamic_cast<LayerTreeItem*>(layersTree->currentItem());
    if (!layer) { return; }

    qDebug() << "set layer opacity" << value << value/100;
    QMdiSubWindow *tab = mdi->currentSubWindow();
    if (!layer || !tab) { return; }
    View *view = qobject_cast<View*>(tab->widget());
    if (!view) { return; }
    layer->setOpacity(value/100);
    view->setLayerOpacity(layer->getLayerID(), value/100);
}

void Editor::handleLayersUpdated()
{
    layersTree->handleTabActivated(mdi->currentSubWindow());
}

void Editor::handleLayerTreeSelectedLayer(int id)
{
    qDebug() << "set selected layer" << id;
    if (!getCurrentView()) { return; }
    getCurrentView()->setSelectedLayer(id);
}

void Editor::handleLayerVisibility(int id,
                                   bool visible)
{
    if (!getCurrentView()) { return; }
    getCurrentView()->setLayerVisibility(id, visible);
}

void Editor::handleLayerLabel(int id,
                              const QString &label)
{
    if (!getCurrentView()) { return; }
    if (getCurrentView()->getLayerName(id) != label) {
        getCurrentView()->setLayerName(id, label);
    }
}

void Editor::addLayerToView(Magick::Image image,
                            View *view)
{
    if (!view || image.columns()==0 || image.rows()==0) { return; }
    try {
        if (image.iccColorProfile().length()==0) {
            qDebug() << "layer is missing color profile, add default";
            QString defPro;
            switch(image.colorSpace()) {
            case Magick::CMYKColorspace:
                defPro = selectedDefaultColorProfile(colorProfileCMYKMenu);
                break;
            case Magick::GRAYColorspace:
                defPro = selectedDefaultColorProfile(colorProfileGRAYMenu);
                break;
            default:
                defPro = selectedDefaultColorProfile(colorProfileRGBMenu);
            }
            qDebug() << "has default profile?" << defPro;
            image = ColorConvert::convertColorspace(image,
                                              Magick::Blob(),
                                              defPro);
        }
        qDebug() << "convert layer to canvas color profile";
        image = ColorConvert::convertColorspace(image,
                                          image.iccColorProfile(),
                                          view->getCanvasProject().profile);
        view->addLayer(image);
    }
    catch(Magick::Error &error_ ) { emit errorMessage(error_.what()); }
    catch(Magick::Warning &warn_ ) { emit warningMessage(warn_.what()); }
}

void Editor::handleRemoveLayer()
{
    if (!getCurrentView()) { return; }
    LayerTreeItem *item = dynamic_cast<LayerTreeItem*>(layersTree->currentItem());
    if (!item) { return; }
    int id = item->getLayerID();
    int ret = QMessageBox::question(this,
                                    tr("Remove layer"),
                                    tr("Are you sure you want to remove layer %1?")
                                    .arg(id));
    if (ret != QMessageBox::Yes) { return; }
    getCurrentView()->removeLayer(id);
}

void Editor::handleMoveLayerDown()
{
    if (!getCurrentView()) { return; }
    LayerTreeItem *item = dynamic_cast<LayerTreeItem*>(layersTree->currentItem());
    if (!item) { return; }
    int topID = item->getLayerID();
    int bottomID = -1;
    int topOrder = getCurrentView()->getLayerOrder(topID);
    if (topOrder<1) { return; }
    int bottomOrder = -1;
    QList<QPair<int, int> > order = getCurrentView()->getSortedLayers();
    for (int i=0;i<order.size();++i) {
        int id = order.at(i).second;
        if (id==topID && i>0) {
            bottomID = order.at(i-1).second;
            bottomOrder = order.at(i-1).first;
        } else { continue; }
    }
    if (bottomOrder<0 || bottomID<0) { return; }
    getCurrentView()->setLayerOrder(topID, bottomOrder);
    getCurrentView()->setLayerOrder(bottomID, topOrder);
    layersTree->handleTabActivated(mdi->currentSubWindow(), true /* force */);

    getCurrentView()->moveLayerItemDown(topID);
}

void Editor::handleMoveLayerUp()
{
    if (!getCurrentView()) { return; }
    LayerTreeItem *item = dynamic_cast<LayerTreeItem*>(layersTree->currentItem());
    if (!item) { return; }
    int currentID = item->getLayerID();
    int overID = -1;
    int currentOrder = getCurrentView()->getLayerOrder(currentID);
    if (currentOrder<0 || currentOrder>=getCurrentView()->getLayerMaxOrder()) { return; }

    int overOrder = -1;
    QList<QPair<int, int> > order = getCurrentView()->getSortedLayers();
    for (int i=0;i<order.size();++i) {
        int id = order.at(i).second;
        if (id==currentID && order.size()>i) {
            overID = order.at(i+1).second;
            overOrder = order.at(i+1).first;
        } else { continue; }
    }
    if (overOrder<0 || overID<0 || overOrder>getCurrentView()->getLayerMaxOrder()) { return; }
    getCurrentView()->setLayerOrder(currentID, overOrder);
    getCurrentView()->setLayerOrder(overID, currentOrder);
    layersTree->handleTabActivated(mdi->currentSubWindow(), true /* force */);

    getCurrentView()->moveLayerItemUp(currentID);
}
