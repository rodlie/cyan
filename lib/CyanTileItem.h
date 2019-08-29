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

#ifndef CYANTILEITEM_H
#define CYANTILEITEM_H

#include "CyanGlobal.h"

#include <QObject>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QPixmap>

class CYANSHARED_EXPORT CyanTileItem: public QObject, public QGraphicsRectItem

{
  Q_OBJECT

public:

    CyanTileItem(QGraphicsItem *parent = nullptr,
                 QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem());
    QMap<int, bool> layers;

private:

    QGraphicsPixmapItem *_pixmap;

public slots:

    void setPixmapItem(QGraphicsPixmapItem *pixmapItem);
    QGraphicsPixmapItem* getPixmapItem();
    void setPixmap(const QPixmap &pixmap);
    void setPixmap(int id,
                   const QPixmap &pixmap);
};

#endif // CYANTILEITEM_H
