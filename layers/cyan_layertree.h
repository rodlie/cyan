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

#ifndef CYANLAYERTREE_H
#define CYANLAYERTREE_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMdiSubWindow>
#include <QKeyEvent>

#include "cyan_view.h"
#include "cyan_layertreeitem.h"

class LayerTree : public QTreeWidget
{
    Q_OBJECT

public:

    explicit LayerTree(QWidget *parent = nullptr);
    ~LayerTree();

signals:

    void selectedLayer(int id);
    void moveLayerEvent(QKeyEvent *e);
    void layerVisibilityChanged(int id,
                                bool visible);
    void layerLabelChanged(int id,
                           const QString &label);
    void layerLockChanged(int id,
                          bool lock);

private:

    QString _canvasID;
    int lastLayerSelected;

public slots:

    void setCanvasID(const QString &id);
    const QString getCanvasID();
    void handleTabActivated(QMdiSubWindow *tab, bool force = false);

private slots:

    void populateTree(View *view);
    void handleItemActivated(QTreeWidgetItem *item,
                             int col);
    void handleItemChanged(QTreeWidgetItem *item,
                           int col);

protected:

    void keyPressEvent(QKeyEvent *e);
};

#endif // CYANLAYERTREE_H
