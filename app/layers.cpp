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

#include <QMessageBox>

#include "cyan_colorconvert.h"

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

    if (layer.isText) {
        textWidget->setEnabled(true);
        textWidget->setText(layer.html);
        textWidget->setTextAlign(layer.textAlign);
        textWidget->setTextRotate(layer.textRotate);
    } else {
        textWidget->setEnabled(false);
        textWidget->setText(QString());
        textWidget->setTextAlign(0);
        textWidget->setTextRotate(0);
    }
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
                                    tr("Are you sure you want to remove layer %1?")
                                    .arg(id));
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

void Editor::handleCurrentLayerTextChanged()
{
    qDebug() << "handle current layer text changed";
    if (!getCurrentCanvas()) { return; }
    CyanLayerTreeItem *layerItem = layersWidget->getCurrentLayer();
    if (!layerItem) { return; }
    getCurrentCanvas()->setLayerText(layerItem->getLayerID(),
                                     textWidget->getText(),
                                     textWidget->getTextAlign(),
                                     textWidget->getTextRotate());
}
