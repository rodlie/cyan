/*
#
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Cyan is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.
#
# Cyan is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Cyan.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>
#
*/

#ifndef LAYERTREE_H
#define LAYERTREE_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMdiSubWindow>
#include "view.h"

class LayerTreeItem : public QTreeWidget, public QTreeWidgetItem
{
    Q_OBJECT

public:
    explicit LayerTreeItem(QTreeWidget *parent = NULL);
    ~LayerTreeItem();
    Magick::CompositeOperator _composite;
    int _id;
    QString _name;

public slots:
    Magick::CompositeOperator getComposite();
    void setComposite(Magick::CompositeOperator composite);
    int getLayerID();
    void setLayerID(int id);
    QString getLayerName();
    void setLayerName(QString name);
};

class LayerTree : public QTreeWidget
{
    Q_OBJECT

public:
    explicit LayerTree(QWidget *parent = NULL);
    ~LayerTree();

public slots:
    void handleTabActivated(QMdiSubWindow *tab);
private slots:
    void populateTree(View *image);
};

#endif // LAYERTREE_H
