/*
#
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#
*/

#ifndef VIEWER_H
#define VIEWER_H

#include <QMainWindow>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QThread>
#include <QActionGroup>
#include <QMdiArea>
#include <Magick++.h>
#include <QDockWidget>

#include "view.h"
#include "layertree.h"

class ImageHandler : public QObject
{
    Q_OBJECT

public:
    explicit ImageHandler(QObject *parent = 0);
    ~ImageHandler();

signals:
    void returnImage(Magick::Image image);
    void errorMessage(QString message);
    void warningMessage(QString message);

public slots:
    void requestImage(QString filename);
    void readImage(QString filename);

private:
    QThread t;
};


class Viewer : public QMainWindow
{
    Q_OBJECT

public:
    Viewer(QWidget *parent = NULL);
    ~Viewer();

signals:
    void openImage(QString filename);

private:
    QMdiArea *mdi;
    QToolBar *mainToolBar;
    QToolBar *pluginsToolBar;
    QMenuBar *mainMenu;
    QStatusBar *mainStatusBar;

    QAction *openImageAct;
    QAction *saveImageAct;
    QAction *newImageAct;
    QAction *quitAct;

    QMenu *filterMenu;

    Magick::Image imageData;
    ImageHandler *imageBackend;
    LayerTree *layersTree;
    QDockWidget *layersDock;

public slots:
    View* getCurrentView();

private slots:
    void setupUI();
    void saveSettings();
    void loadSettings();
    void saveImage(QString filename);
    void loadImage(QString filename);
    void saveImageDialog();
    void loadImageDialog();
    void handleNewImage(Magick::Image image);
    void handleError(QString message);
    void handleWarning(QString message);
    void addPlugin(QObject *plugin, QString filename);
    void loadPlugins();
    void applyFilter();
    void addToMenu(QObject *plugin, const QStringList &texts, QMenu *menu, const char *member, QActionGroup *actionGroup = 0);
    void newTab(Magick::Image image);
    void newImage();
};

#endif // VIEWER_H
