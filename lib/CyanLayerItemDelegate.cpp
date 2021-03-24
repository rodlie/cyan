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

#include "CyanLayerItemDelegate.h"
#include <QApplication>

CyanLayerItemDelegate::CyanLayerItemDelegate(QObject *parent):
    QStyledItemDelegate(parent)
{
}

void CyanLayerItemDelegate::paint(QPainter * painter,
                                  const QStyleOptionViewItem & option,
                                  const QModelIndex & index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    const QWidget *widget = option.widget;
    QStyle *style = widget?widget->style():QApplication::style();

    style->drawPrimitive(QStyle::PE_PanelItemViewItem,
                         &opt,
                         painter,
                         widget);

    QRect checkRect = style->subElementRect(QStyle::SE_ItemViewItemCheckIndicator,
                                            &opt,
                                            widget);
    drawCheckBox(painter,
                 checkRect,
                 opt.checkState,
                 index);
}

QWidget *CyanLayerItemDelegate::createEditor(QWidget *parent,
                                             const QStyleOptionViewItem &option,
                                             const QModelIndex &index) const
{
    Q_UNUSED(parent)
    Q_UNUSED(option)
    Q_UNUSED(index)
    return nullptr;
}

QSize CyanLayerItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const
{
    Q_UNUSED(index)
    return QSize(option.rect.width(),
                 option.rect.height());
}

void CyanLayerItemDelegate::drawCheckBox(QPainter *painter,
                                         const QRect &checkRect,
                                         Qt::CheckState checkState,
                                         const QModelIndex &index) const
{
    QIcon icon;
    switch (index.column()) {
    case 1:
        icon = QIcon::fromTheme(checkState==Qt::Checked?"eye":"eye_close_gray");
        icon.paint(painter, checkRect, Qt::AlignCenter|Qt::AlignHCenter);
        break;
    case 2:
        icon = QIcon::fromTheme(checkState==Qt::Checked?"lock":"lock_open_gray");
        icon.paint(painter, checkRect, Qt::AlignCenter|Qt::AlignHCenter);
        break;
    default:
        painter->fillRect(checkRect,
                          checkState==Qt::Checked?Qt::green:Qt::red);
    }
}
