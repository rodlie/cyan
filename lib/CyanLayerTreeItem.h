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

#ifndef CYANLAYERTREEITEM_H
#define CYANLAYERTREEITEM_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QString>

#include <Magick++.h>

class CyanLayerTreeItem : public QTreeWidget, public QTreeWidgetItem
{
    Q_OBJECT

public:

    explicit CyanLayerTreeItem(QTreeWidget *parent = nullptr);
    ~CyanLayerTreeItem();

private:

    Magick::CompositeOperator _composite;
    int _id;
    QString _name;
    double _opacity;
    bool _visible;
    bool _locked;
    int _order;

public slots:

    Magick::CompositeOperator getComposite();
    void setComposite(Magick::CompositeOperator composite);
    int getLayerID();
    void setLayerID(int id);
    int getLayerOrder();
    void setLayerOrder(int order);
    QString getLayerName();
    void setLayerName(QString name);
    double getOpacity();
    void setOpacity(double value);
    bool getVisibility();
    void setVisibility(bool visible);
    bool getLock();
    void setLock(bool lock);
};

#endif // CYANLAYERTREEITEM_H
