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

#ifndef CYANLAYERTREE_H
#define CYANLAYERTREE_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMdiSubWindow>
#include <QKeyEvent>

#include "CyanView.h"
#include "CyanLayerTreeItem.h"

#include "CyanGlobal.h"

class CYANSHARED_EXPORT LayerTree : public QTreeWidget
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
    void actNewImage();
    void actRemove();
    void actDuplicate();
    void actMoveUp();
    void actMoveDown();


private:
    QString _canvasID;
    int lastLayerSelected;
    int _maxLayersOrder;
    QAction *newImageLayerAct;
    QAction *removeLayerAct;
    QAction *moveUpLayerAct;
    QAction *moveDownLayerAct;
    QAction *duplicateLayerAct;

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
    void handleContextMenu(const QPoint &pos);
    void handleNewImageAct(bool triggered);
    void handleRemoveLayerAct(bool triggered);
    void handleMoveUpLayerAct(bool triggered);
    void handleMoveDownLayerAct(bool triggered);
    void handleDuplicateLayerAct(bool triggered);

protected:
    void keyPressEvent(QKeyEvent *e);
};

#endif // CYANLAYERTREE_H
