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

#include "editor.h"

#include <QMessageBox>

#include "CyanColorConvert.h"

void Editor::handleLayerCompChanged(Magick::CompositeOperator composite, int id)
{
    if (id<0) { return; }
    QMdiSubWindow *tab = mdi->currentSubWindow();
    if (!tab) { return; }
    View *view = qobject_cast<View*>(tab->widget());
    if (!view) { return; }
    qDebug() << "editor set composite";
    view->setLayerComposite(id, composite);
}

void Editor::handleLayersOpacity(double value, int id)
{
    QMdiSubWindow *tab = mdi->currentSubWindow();
    if (!tab) { return; }
    View *view = qobject_cast<View*>(tab->widget());
    if (!view) { return; }
    view->setLayerOpacity(id,
                          value);
}

void Editor::handleLayersUpdated()
{
    layersWidget->handleTabActivated(mdi->currentSubWindow());
}

void Editor::handleLayerTreeSelectedLayer(int id)
{
    qDebug() << "HANDLE LAYER TREE SELECTED LAYER" << id;
    if (!getCurrentCanvas()) { return; }
    getCurrentCanvas()->setSelectedLayer(id);
    CyanImageFormat::CyanLayer layer = getCurrentCanvas()->getLayer(id);
    if (!layer.image.isValid()) { return; }

    emit currentTextChanged(layer.html, layer.textAlign, layer.textRotate, layer.isText);
}

void Editor::handleLayerVisibility(int id,
                                   bool visible)
{
    if (!getCurrentCanvas()) { return; }
    getCurrentCanvas()->setLayerVisibility(id,
                                         visible);
}

void Editor::handleLayerLabel(int id,
                              const QString &label)
{
    if (!getCurrentCanvas()) { return; }
    if (getCurrentCanvas()->getLayerName(id) != label) {
        getCurrentCanvas()->setLayerName(id,
                                         label);
    }
}

void Editor::handleLayerLock(int id, bool lock)
{
    if (!getCurrentCanvas()) { return; }
    qDebug() << "handle layer lock changed" << id << lock;
    getCurrentCanvas()->setLayerLock(id,
                                     lock);
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
                defPro = selectedDefaultColorProfile(profileCMYKGroup);
                break;
            case Magick::GRAYColorspace:
                defPro = selectedDefaultColorProfile(profileGRAYGroup);
                break;
            default:
                defPro = selectedDefaultColorProfile(profileRGBGroup);
            }
            if (defPro.isEmpty()) {
                QMessageBox::warning(this,
                                     tr("Missing default profile"),
                                     tr("Missing default profile for the selected color space!"));
                return;
            }
            qDebug() << "has default profile?" << defPro;
            image = ColorConvert::convertColorspace(image,
                                              Magick::Blob(),
                                              defPro);
        }

        image = ColorConvert::convertColorspace(image,
                                          image.iccColorProfile(),
                                          view->getCanvasProject().profile);
        view->addLayer(image);
    }
    catch(Magick::Error &error_ ) { emit errorMessage(error_.what()); }
    catch(Magick::Warning &warn_ ) { emit warningMessage(warn_.what()); }
}

void Editor::handleRemoveLayer(int id)
{
    if (!getCurrentCanvas() || id<0) { return; }
    int ret = QMessageBox::question(this,
                                    tr("Remove layer"),
                                    QString("%2 %1?")
                                    .arg(id)
                                    .arg(tr("Are you sure you want to remove layer")));
    if (ret != QMessageBox::Yes) { return; }

    getCurrentCanvas()->removeLayer(id);
}

void Editor::handleMoveLayerDown(int id)
{
    if (!getCurrentCanvas() || id<0) { return; }
    int bottomID = -1;
    int topOrder = getCurrentCanvas()->getLayerOrder(id);
    if (topOrder<1) { return; }
    int bottomOrder = -1;

    QList<QPair<int, int> > order = getCurrentCanvas()->getSortedLayers();
    for (int i=0;i<order.size();++i) {
        if (order.at(i).second ==id && i>0) {
            bottomID = order.at(i-1).second;
            bottomOrder = order.at(i-1).first;
        } else { continue; }
    }

    if (bottomOrder<0 || bottomID<0) { return; }
    getCurrentCanvas()->setLayerOrder(id, bottomOrder);
    getCurrentCanvas()->setLayerOrder(bottomID, topOrder);

    layersWidget->handleTabActivated(mdi->currentSubWindow(),
                                     true);

    getCurrentCanvas()->moveLayerItemDown(id);
}

void Editor::handleMoveLayerUp(int id)
{
    if (!getCurrentCanvas() || id<0) { return; }
    int overID = -1;
    int currentOrder = getCurrentCanvas()->getLayerOrder(id);
    if (currentOrder<0 || currentOrder>=getCurrentCanvas()->getLayerMaxOrder()) { return; }
    int overOrder = -1;

    QList<QPair<int, int> > order = getCurrentCanvas()->getSortedLayers();
    for (int i=0;i<order.size();++i) {
        if (order.at(i).second ==id && order.size()>i) {
            overID = order.at(i+1).second;
            overOrder = order.at(i+1).first;
        } else { continue; }
    }

    if (overOrder<0 || overID<0 || overOrder>getCurrentCanvas()->getLayerMaxOrder()) { return; }
    getCurrentCanvas()->setLayerOrder(id, overOrder);
    getCurrentCanvas()->setLayerOrder(overID, currentOrder);

    layersWidget->handleTabActivated(mdi->currentSubWindow(),
                                     true);

    getCurrentCanvas()->moveLayerItemUp(id);
}

void Editor::handleDuplicateLayer(int id)
{
    if (!getCurrentCanvas() || id<0) { return; }
    getCurrentCanvas()->duplicateLayer(id);
}

void Editor::handleCurrentLayerTextChanged(const QString &html,
                                           int align,
                                           int rotate,
                                           bool enabled)
{
    Q_UNUSED(enabled)
    qDebug() << "handle current layer text changed";
    if (!getCurrentCanvas()) { return; }
    CyanLayerTreeItem *layerItem = layersWidget->getCurrentLayer();
    if (!layerItem) { return; }
    getCurrentCanvas()->setLayerText(layerItem->getLayerID(),
                                     html,
                                     align,
                                     rotate);
}
