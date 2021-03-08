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

void Editor::newTab(CyanImageFormat::CyanCanvas canvas)
{
    qDebug() << "new tab from project";
    MdiSubWindow *tab = new MdiSubWindow(mdi);
    tab->setAttribute(Qt::WA_DeleteOnClose);

    View *view = new View(tab, false, _native);
    connectView(view);

    // get guides
    QString vguides = QString::fromStdString(canvas.image.attribute(QString(CYAN_IMAGE_V_GUIDES)
                                                                    .toStdString()));
    QString hguides = QString::fromStdString(canvas.image.attribute(QString(CYAN_IMAGE_H_GUIDES)
                                                                    .toStdString()));

    qDebug() << "GUIDES?" << vguides << hguides;

    // setup view
    view->setCanvasSpecsFromImage(canvas.image);
    view->setLayersFromCanvas(canvas);
    view->setFit(viewZoomFitAct->isChecked());
    //view->setBrushColor(colorPicker->currentColor());

    // add guides
    if (!vguides.isEmpty()) {
        QStringList splitGuides = vguides.split(";");
        for (int i=0;i<splitGuides.size();++i) {
            QString guide = splitGuides.at(i);
            QStringList splitGuide = guide.split("x");
            qDebug() << "VGUIDE?" << splitGuide.size() << guide;
            if (splitGuide.size()!=2) { continue; }
            QString pX = splitGuide.at(0);
            QString pY = splitGuide.at(1);
            qDebug() << "VGUIDE POS?" << pX << pY;
            view->addGuide(false,
                           QPointF(pX.toDouble(),
                                   pY.toDouble()));
        }
    }
    if (!hguides.isEmpty()) {
        QStringList splitGuides = hguides.split(";");
        for (int i=0;i<splitGuides.size();++i) {
            QString guide = splitGuides.at(i);
            QStringList splitGuide = guide.split("x");
            qDebug() << "HGUIDE?" << splitGuide.size() << guide;
            if (splitGuide.size()!=2) { continue; }
            QString pX = splitGuide.at(0);
            QString pY = splitGuide.at(1);
            qDebug() << "HGUIDE POS?" << pX << pY;
            view->addGuide(true,
                           QPointF(pX.toDouble(),
                                   pY.toDouble()));
        }
    }

    view->setDirty(false);

    tab->setWidget(view);
    tab->showMaximized();
    tab->setWindowIcon(QIcon::fromTheme("applications-graphics"));

    /*if (viewMoveAct->isChecked()) {
        view->setInteractiveMode(View::IteractiveMoveMode);
    } else if (viewDragAct->isChecked()) {
        view->setInteractiveMode(View::InteractiveDragMode);
    } else if (viewDrawAct->isChecked()) {
        view->setInteractiveMode(View::InteractiveDrawMode);
    }*/
    setViewTool(view);
    updateTabTitle(view);
    handleTabActivated(tab);
    setCurrentZoom();
}

void Editor::newTab(Magick::Image image, QSize geo)
{
    qDebug() << "new tab from image";
    //if (!image.isValid()) { return; }
    /*if ((image.columns()==0 || image.rows() == 0) && geo.width()==0) {
        emit statusMessage(tr("Invalid image/size"));
        return new View(this);
    }*/

    MdiSubWindow *tab = new MdiSubWindow(mdi);
    tab->setAttribute(Qt::WA_DeleteOnClose);

    View *view = new View(tab, false, _native);
    connectView(view);

    if (geo.width()>0) {
        view->setupCanvas(geo.width(), geo.height());
    } else if (image.columns()>0) {
        view->setCanvasSpecsFromImage(image);
    }
    if (geo.width()>0) { view->addLayer(view->getCanvas()); }
    else { view->addLayer(image); }
    view->setFit(viewZoomFitAct->isChecked());
    //view->setBrushColor(colorPicker->currentColor());

    tab->setWidget(view);
    tab->showMaximized();
    tab->setWindowIcon(QIcon::fromTheme("applications-graphics"));

    /*if (viewMoveAct->isChecked()) {
        view->setInteractiveMode(View::IteractiveMoveMode);
    } else if (viewDragAct->isChecked()) {
        view->setInteractiveMode(View::InteractiveDragMode);
    } else if (viewDrawAct->isChecked()) {
        view->setInteractiveMode(View::InteractiveDrawMode);
    }*/
    setViewTool(view);
    updateTabTitle(view);
    handleTabActivated(tab);
    setCurrentZoom();
}

void Editor::handleTabActivated(QMdiSubWindow *tab)
{
    qDebug() << "handle tab activated";

    setActionsDisabled(true);
    setProjectSaveDisabled(true);

    if (!tab) { return; }
    View *view = qobject_cast<View*>(tab->widget());
    if (!view) { return; }

    setActionsDisabled(false);

    handleSwitchMoveTool(view->getInteractiveMode());
    updateTabTitle();
    handleBrushSize();
    viewZoomFitAct->setChecked(view->isFit());
    setCurrentZoom();
    handleCanvasStatus();
}

void Editor::updateTabTitle(View *view)
{
    if (!view) { view = qobject_cast<View*>(getCurrentCanvas()); }
    if (!view) { return; }
    QString title = CyanCommon::canvasWindowTitle(view->getCanvas());
    qDebug() << "update canvas title" << title;
    view->setWindowTitle(title);
}

