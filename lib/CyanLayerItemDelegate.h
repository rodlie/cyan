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

#ifndef CYANLAYERITEMDELEGATE_H
#define CYANLAYERITEMDELEGATE_H

#include <QPainter>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>
#include <QModelIndex>
#include <QRect>
#include <QSize>

class CyanLayerItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
        CyanLayerItemDelegate(QObject *parent = nullptr);
        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;
        QSize sizeHint(const QStyleOptionViewItem &option,
                       const QModelIndex &index) const;

private:
        void drawCheckBox(QPainter *painter,
                          const QRect &checkRect,
                          Qt::CheckState checkState,
                          const QModelIndex &index) const;
};

#endif // CYANLAYERITEMDELEGATE_H
