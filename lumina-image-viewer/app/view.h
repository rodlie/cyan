/*
#
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#
*/
#ifndef VIEW_H
#define VIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QResizeEvent>
#include <QGraphicsScene>
#include <Magick++.h>

class View : public QGraphicsView
{
    Q_OBJECT

public:
    explicit View(QWidget* parent = NULL, int width = 640, int height = 480, int depth = 8, Magick::ColorspaceType colorspace = MagickCore::sRGBColorspace);
    bool fit;

private:
    Magick::Image _image;
    Magick::Image _canvas;
    QGraphicsScene *_scene;
    QMap<int, Magick::Image> _layers;
    QMap<int, QSize> _layersPOS;
    QMap<int, Magick::CompositeOperator> _layersComp;
    QMap<int, QVector<Magick::Image> > _layersHistory;
    QMap<int, bool> _layersVisibility;

signals:
    void resetZoom();
    void myZoom(double scaleX, double scaleY);
    void myFit(bool value);
    void openImage(QString filename);
    void openProfile(QString filename);
    void updatedLayers();

public slots:
    void doZoom(double scaleX, double scaleY);
    void setFit(bool value);
    void resetImageZoom();
    Magick::Image getImage();
    void setImage(Magick::Image image);
    void addLayer(Magick::Image image);
    void clearLayers();
    Magick::Image getCanvas();
    void setLayerVisibility(int layer, bool layerIsVisible);
    void setLayerComposite(int layer, Magick::CompositeOperator composite);
    Magick::CompositeOperator getLayerComposite(int layer);
    int getLayerCount();
    Magick::Image getLayer(int layer);
    void setLayer(int layer, Magick::Image image);
    QSize getLayerOffset(int layer);
    void setLayerOffset(int layer, QSize offset);
    QString getLayerName(int layer);
    void setLayerName(int layer, QString name);
    void removeLayer(int layer);

private slots:
    void clearCanvas(int width = 640, int height = 480, int depth = 8, Magick::ColorspaceType colorspace = MagickCore::sRGBColorspace);
    void setCanvasSpecsFromImage(Magick::Image image);
    void procLayers();
    void viewImage();
    Magick::Blob makePreview();

protected:
    void wheelEvent(QWheelEvent* event);
    void mousePressEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void resizeEvent(QResizeEvent *event);
};

#endif // VIEW_H
