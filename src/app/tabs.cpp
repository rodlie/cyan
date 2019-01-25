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

#include "editor.h"

void Editor::newTab(Common::Canvas canvas)
{
    qDebug() << "new tab from canvas/project";
    QMdiSubWindow *tab = new QMdiSubWindow(mdi);
    tab->setAttribute(Qt::WA_DeleteOnClose);

    View *view = new View(tab);
    connectView(view);

    view->setCanvasSpecsFromImage(canvas.image);
    view->setLayersFromCanvas(canvas);
    view->setFit(true);
    view->setBrushColor(colorPicker->currentColor());

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
}

void Editor::newTab(Magick::Image image, QSize geo)
{
    qDebug() << "new tab from image";
    //if (!image.isValid()) { return; }
    /*if ((image.columns()==0 || image.rows() == 0) && geo.width()==0) {
        emit statusMessage(tr("Invalid image/size"));
        return new View(this);
    }*/

    QMdiSubWindow *tab = new QMdiSubWindow(mdi);
    tab->setAttribute(Qt::WA_DeleteOnClose);

    View *view = new View(tab);
    connectView(view);

    if (geo.width()>0) {
        view->setupCanvas(geo.width(), geo.height());
    } else if (image.columns()>0) {
        view->setCanvasSpecsFromImage(image);
    }
    if (geo.width()>0) { view->addLayer(view->getCanvas()); }
    else { view->addLayer(image); }
    view->setFit(true);
    view->setBrushColor(colorPicker->currentColor());

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
    //return view;
}

void Editor::handleTabActivated(QMdiSubWindow *tab)
{
    qDebug() << "handle tab activated";
    if (!tab) { return; }
    View *view = qobject_cast<View*>(tab->widget());
    if (!view) { return; }

    /*if (viewDragAct->isChecked()) {
        view->setDragMode(QGraphicsView::ScrollHandDrag);
        view->setInteractive(false);
    } else {
        view->setDragMode(QGraphicsView::NoDrag);
        view->setInteractive(true);
    }*/
    updateTabTitle();
    handleBrushSize();
}

void Editor::updateTabTitle(View *view)
{
    if (!view) { view = qobject_cast<View*>(getCurrentCanvas()); }
    if (!view) { return; }
    QString title = Common::canvasWindowTitle(view->getCanvas());
    qDebug() << "update canvas title" << title;
    view->setWindowTitle(title);
}
