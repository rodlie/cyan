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

#ifndef CYANLAYERWIDGET_H
#define CYANLAYERWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QMdiSubWindow>

#include <Magick++.h>

#include "cyan_layertree.h"

class CyanLayerWidget : public QWidget
{
    Q_OBJECT

public:

    explicit CyanLayerWidget(QWidget *parent = nullptr);
    ~CyanLayerWidget();

signals:

    void layerCompositeChanged(const Magick::CompositeOperator &composite,
                               const int &id);
    void layerOpacityChanged(const double &value,
                             const int &id);
    void selectedLayer(int id);
    void layerVisibilityChanged(int id, bool visible);
    void layerLabelChanged(int id, QString label);
    void layerLockChanged(int id, bool lock);
    void moveLayerEvent(QKeyEvent *e);
    void newLayer();
    void removeLayer(int id);
    void moveLayerUp(int id);
    void moveLayerDown(int id);
    void duplicateLayer(int id);

public slots:

    void setCanvasID(const QString &id);
    const QString getCanvasID();

    CyanLayerTreeItem* getCurrentLayer();
    void setCurrentLayer(const int &id);

    void handleTabActivated(QMdiSubWindow *tab,
                            bool force = false);
    void clearTree();

private:

    LayerTree *layerTree;
    QComboBox *layerCompositeBox;
    QSlider *layerOpacitySlider;
    QDoubleSpinBox *layerOpacitySpin;

private slots:

    void populateCompositeBox();

    void handleLayerActivated(QTreeWidgetItem *item,
                              int col);
    void handleLayerActivated(QTreeWidgetItem *item,
                              QTreeWidgetItem *old);

    void handleLayerOpacitySpin(double value);
    void handleLayerOpacitySlider();
    void handleLayerCompositeBox();

    void handleTreeSelectedLayer(int id);
    void handleTreeLayerVisibility(int id,
                                   bool visible);
    void handleTreeLayerLabel(int id,
                              QString label);
    void handleTreeLayerLock(int id,
                             bool lock);
    void handleTreeMoveLayer(QKeyEvent *e);

    void handleNewImageLayer();
    void handleRemoveLayer();
    void handleMoveLayerUp();
    void handleMoveLayerDown();
    void handleDuplicateLayer();
};

#endif // CYANLAYERWIDGET_H
