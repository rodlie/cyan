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

#ifndef CYAN_HISTORY_H
#define CYAN_HISTORY_H

#include <QObject>
#include <QVector>
#include <QSize>
#include <QPointF>

#include <Magick++.h>

#include "CyanGlobal.h"

class CYANSHARED_EXPORT CyanHistory: public QObject
{
    Q_OBJECT

public:
    enum CyanHistoryType
    {
        CyanHistoryTypeDefault,
        CyanHistoryTypePosition,
        CyanHistoryTypeVisibility,
        CyanHistoryTypeLocked,
        CyanHistoryTypeOrder,
        CyanHistoryTypeOpacity
    };

    struct CyanHistoryPosition
    {
        QPointF state = QPointF(0, 0);
        QPointF undo = QPointF(0, 0);
        QPointF redo = QPointF(0, 0);
    };

    struct CyanHistoryVisibility
    {
        bool state = true;
        bool undo = true;
        bool redo = true;
    };

    struct CyanHistoryLocked
    {
        bool state = false;
        bool undo = false;
        bool redo = false;
    };

    struct CyanHistoryOrder
    {
        int state = -1;
        int undo = -1;
        int redo = -1;
    };

    struct CyanHistoryOpacity
    {
        double state = 100.0;
        double undo = 100.0;
        double redo = 100.0;
    };

    struct CyanHistoryItem
    {
        int layer = -1;
        CyanHistoryVisibility visibility;
        CyanHistoryLocked locked;
        Magick::CompositeOperator composite = Magick::OverCompositeOp;
        CyanHistoryOrder order;
        CyanHistoryPosition pos;
        CyanHistoryOpacity opacity;
        CyanHistoryType type = CyanHistoryTypeDefault;

        QSize position = QSize(0, 0);
        QSize redoPOS = QSize(0, 0);
        QSize undoPOS = QSize(0, 0);
    };

    CyanHistory(QObject *parent = nullptr);

private:
    QVector<CyanHistoryItem> undoStorage;
    QVector<CyanHistoryItem> redoStorage;

public slots:
    void addUndo(CyanHistoryItem history);
    void addRedo(CyanHistoryItem history);

    CyanHistoryItem getUndo();
    CyanHistoryItem getRedo();

    int getUndoTotal();
    int getRedototal();

    void clearUndo();
    void clearRedo();

    void clearLastUndo();
    void clearLastRedo();

    void clearAll();
};

#endif // CYANHISTORY_H
