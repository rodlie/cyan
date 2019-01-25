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

#ifndef CYANCANVASTILEITEM_H
#define CYANCANVASTILEITEM_H

#include <QObject>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QPixmap>

#include "common_global.h"

class CYAN_TILEITEM_EXPORT TileItem: public QObject, public QGraphicsRectItem

{
  Q_OBJECT

public:

    TileItem(QGraphicsItem *parent = nullptr,
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

#endif // CYANCANVASTILEITEM_H
