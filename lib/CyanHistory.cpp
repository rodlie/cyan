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

#include "CyanHistory.h"

#include <QDebug>

CyanHistory::CyanHistory(QObject *parent): QObject (parent)
{
}

void CyanHistory::addUndo(CyanHistory::CyanHistoryItem history)
{
    qDebug() << "ADD UNDO ITEM" << history.layer << history.order << history.locked << history.position << history.composite << history.visibility << "TOTAL" << undoStorage.length();
    undoStorage.push_back(history);
}

void CyanHistory::addRedo(CyanHistory::CyanHistoryItem history)
{
    qDebug() << "add redo" << history.layer;
    redoStorage.push_back(history);
}

CyanHistory::CyanHistoryItem CyanHistory::getUndo()
{
    return undoStorage.last();
}

CyanHistory::CyanHistoryItem CyanHistory::getRedo()
{
    return redoStorage.last();
}

int CyanHistory::getUndoTotal()
{
    return undoStorage.length();
}

int CyanHistory::getRedototal()
{
    return redoStorage.length();
}

void CyanHistory::clearUndo()
{
    undoStorage.clear();
}

void CyanHistory::clearRedo()
{
    redoStorage.clear();
}
