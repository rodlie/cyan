/*
* Cyan <https://github.com/olear/cyan>,
* Copyright (C) 2016 Ole-André Rodlie
*
* Cyan is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as published
* by the Free Software Foundation.
*
* Cyan is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Cyan.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>
*/

#include "cyan.h"
#include <Magick++.h>
#include <QCoreApplication>
#include <QLabel>
#include <QVBoxLayout>

CyanView::CyanView(QWidget* parent) : QGraphicsView(parent) {
}

void CyanView::wheelEvent(QWheelEvent* event) {
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    double scaleFactor = 1.15;
    if(event->delta() > 0) {
        scale(scaleFactor, scaleFactor);
        emit myZoom(scaleFactor,scaleFactor);
    }
    else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        emit myZoom(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}

void CyanView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        emit resetZoom();
    } else {
        if (event->button() == Qt::RightButton) {
            emit proof();
        } else {
            QGraphicsView::mousePressEvent(event);
        }
    }
}

void CyanView::doZoom(double scaleX, double scaleY)
{
    scale(scaleX,scaleY);
}

Cyan::Cyan(QWidget *parent)
    : QMainWindow(parent)
    , scene(0)
    , view(0)
    , mainBar(0)
    , convertBar(0)
    , profileBar(0)
    , rgbProfile(0)
    , cmykProfile(0)
    , grayProfile(0)
    , inputProfile(0)
    , outputProfile(0)
    , monitorProfile(0)
    , renderingIntent(0)
    , blackPoint(0)
    , modulateDock(0)
    , brightDial(0)
    , hueDial(0)
    , satDial(0)
{
    Magick::InitializeMagick(NULL);

    setWindowTitle(qApp->applicationName());
    setWindowIcon(QIcon(":/cyan.png"));

    scene = new QGraphicsScene();
    view = new CyanView();

    view->setAcceptDrops(true);
    view->setBackgroundBrush(Qt::darkGray);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setScene(scene);

    setCentralWidget(view);

    mainBar = new QToolBar();
    convertBar = new QToolBar();
    profileBar = new QToolBar();
    modulateDock = new QDockWidget();

    mainBar->setObjectName("MainToolbar");
    mainBar->setWindowTitle(tr("Main Toolbar"));

    convertBar->setObjectName("ConvertToolbar");
    convertBar->setWindowTitle(tr("Convert Toolbar"));

    profileBar->setObjectName("ProfileToolbar");
    profileBar->setWindowTitle(tr("Profiles Toolbar"));

    modulateDock->setObjectName("ModulateDock");
    modulateDock->setWindowTitle(tr("Adjust"));
    modulateDock->setFeatures(QDockWidget::DockWidgetMovable);

    addToolBar(Qt::TopToolBarArea,mainBar);
    addToolBar(Qt::TopToolBarArea,convertBar);
    addToolBar(Qt::BottomToolBarArea,profileBar);
    addDockWidget(Qt::RightDockWidgetArea,modulateDock);

    rgbProfile = new QComboBox();
    cmykProfile = new QComboBox();
    grayProfile = new QComboBox();
    inputProfile = new QComboBox();
    outputProfile = new QComboBox();
    monitorProfile = new QComboBox();
    renderingIntent = new QComboBox();
    blackPoint = new QCheckBox();

    rgbProfile->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    cmykProfile->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    grayProfile->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    inputProfile->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    outputProfile->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    monitorProfile->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    renderingIntent->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    QLabel *inputLabel = new QLabel();
    QLabel *outputLabel = new QLabel();
    QLabel *monitorLabel = new QLabel();
    QLabel *renderLabel = new QLabel();
    QLabel *blackLabel = new QLabel();
    QLabel *rgbLabel = new QLabel();
    QLabel *cmykLabel = new QLabel();
    QLabel *grayLabel = new QLabel();

    inputLabel->setText(tr("Input"));
    outputLabel->setText(tr("Output"));
    monitorLabel->setText(tr("Monitor"));
    renderLabel->setText(tr("Intent"));
    blackLabel->setText(tr("Black Point"));
    rgbLabel->setText(tr("RGB"));
    cmykLabel->setText(tr("CMYK"));
    grayLabel->setText(tr("GRAY"));

    convertBar->addWidget(inputLabel);
    convertBar->addWidget(inputProfile);
    convertBar->addWidget(outputLabel);
    convertBar->addWidget(outputProfile);
    convertBar->addWidget(renderLabel);
    convertBar->addWidget(renderingIntent);
    convertBar->addWidget(blackLabel);
    convertBar->addWidget(blackPoint);

    profileBar->addWidget(rgbLabel);
    profileBar->addWidget(rgbProfile);
    profileBar->addWidget(cmykLabel);
    profileBar->addWidget(cmykProfile);
    profileBar->addWidget(grayLabel);
    profileBar->addWidget(grayProfile);
    profileBar->addWidget(monitorLabel);
    profileBar->addWidget(monitorProfile);

    brightDial = new QDial();
    hueDial = new QDial();
    satDial = new QDial();

    QVBoxLayout *modulateLayout = new QVBoxLayout();

    modulateLayout->addWidget(brightDial);
    modulateLayout->addWidget(hueDial);
    modulateLayout->addWidget(satDial);
    modulateLayout->addStretch();

    QWidget *modulateWidget = new QWidget();
    modulateWidget->setLayout(modulateLayout);
    modulateDock->setWidget(modulateWidget);
}

Cyan::~Cyan()
{

}
