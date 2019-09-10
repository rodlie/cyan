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

#include "editor.h"

void Editor::handleSwitchMoveTool(View::InteractiveMode tool)
{
    qDebug() << "handle switch view tool" << tool;
    View::InteractiveMode mode = View::InteractiveNoMode;
    if (tool == View::InteractiveNoMode) {
        if (viewMoveAct->isChecked()) {
            /*// drag
            viewMoveAct->setChecked(false);
            viewDragAct->setChecked(true);
            viewDrawAct->setChecked(false);
            mode = View::InteractiveDragMode;
        } else if (viewDragAct->isChecked()) {*/
            // draw
            viewMoveAct->setChecked(false);
            //viewDragAct->setChecked(false);
            viewDrawAct->setChecked(true);
            mode = View::InteractiveDrawMode;
        } else if (viewDrawAct->isChecked()) {
            // move
            viewMoveAct->setChecked(true);
            //viewDragAct->setChecked(false);
            viewDrawAct->setChecked(false);
            mode = View::IteractiveMoveMode;
        }
    } else {
        mode = tool;
        switch(mode) {
        /*case View::InteractiveDragMode:
            viewMoveAct->setChecked(false);
            viewDragAct->setChecked(true);
            viewDrawAct->setChecked(false);
            break;*/
        case View::InteractiveDrawMode:
            viewMoveAct->setChecked(false);
            //viewDragAct->setChecked(false);
            viewDrawAct->setChecked(true);
            break;
        case View::IteractiveMoveMode:
            viewMoveAct->setChecked(true);
            //viewDragAct->setChecked(false);
            viewDrawAct->setChecked(false);
            break;
        default:;
        }
    }
    if (viewMoveAct->isChecked()) {
        moveButton->setDefaultAction(viewMoveAct);
    } else if (viewDrawAct->isChecked()) {
        moveButton->setDefaultAction(viewDrawAct);
    }
    if (!getCurrentCanvas()) { return; }
    getCurrentCanvas()->setInteractiveMode(mode);
}

void Editor::handleSetMoveMode(bool triggered)
{
    qDebug() << "set move mode" << triggered;
    if (!triggered) {
        viewMoveAct->setChecked(true);
        moveButton->setDefaultAction(viewMoveAct);
        return;
    }
    handleSwitchMoveTool(View::IteractiveMoveMode);
}

void Editor::handleSetDrawMode(bool triggered)
{
    if (!triggered) {
        viewDrawAct->setChecked(true);
        moveButton->setDefaultAction(viewDrawAct);
        return;
    }
    handleSwitchMoveTool(View::InteractiveDrawMode);
}

void Editor::handleBrushSize()
{
    /*QList<QMdiSubWindow*> list = mdi->subWindowList();
    for (int i=0;i<list.size();++i) {
        QMdiSubWindow *window = qobject_cast<QMdiSubWindow*>(list.at(i));
        if (!window) { return; }
        View *view = qobject_cast<View*>(window->widget());
        if (!view) { return; }
        view->setBrushStroke(brushSize->value());
    }*/
}

void Editor::handleUpdateBrushSize(int stroke)
{
    Q_UNUSED(stroke)
    qDebug() << "brush size changed" << stroke;
    //brushSize->setValue(stroke);
}
