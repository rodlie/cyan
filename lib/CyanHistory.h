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

#include <Magick++.h>

#include "CyanGlobal.h"

class CYANSHARED_EXPORT CyanHistory: public QObject
{
    Q_OBJECT

public:
    struct CyanHistoryItem
    {
        int layer = 0;
        bool visibility = true;
        bool locked = false;
        Magick::CompositeOperator composite = Magick::OverCompositeOp;
        int order = 0;
        QSize position = QSize(0, 0);
        double opacity = 100.0;
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
};

#endif // CYANHISTORY_H
