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

#ifndef CYANCANVASVIEW_H
#define CYANCANVASVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QResizeEvent>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPixmapItem>
#include <QFuture>
#include <QKeyEvent>
#include <QList>

#include "cyan_render.h"
#include "cyan_layeritem.h"

#define TILE_Z 6
#define LAYER_Z 100
#define BRUSH_Z 200

class View : public QGraphicsView
{
    Q_OBJECT

public:

    enum InteractiveMode
    {
        InteractiveNoMode,
        IteractiveMoveMode,
        InteractiveDragMode,
        InteractiveDrawMode
    };

    explicit View(QWidget* parent = nullptr,
                  bool setup = false);
    ~View();
    bool fit;

private:

    QString _parentCanvas;
    int _parentLayer;
    CyanCommon::Canvas _canvas;
    Magick::Image _image;
    QGraphicsScene *_scene;
    QGraphicsRectItem *_rect;
    QGraphicsEllipseItem *_brush;
    bool _drawing;
    bool _moving;
    int _selectedLayer;
    QFuture<void> future;
    bool _supportsLayers;

signals:

    void resetZoom();
    void myZoom(double scaleX,
                double scaleY);
    void myFit(bool value);

    void updatedLayers();
    void addedLayer(int layer);
    void selectedLayer(int layer);

    void errorMessage(QString message);
    void warningMessage(QString message);
    void statusMessage(QString message);

    void viewClosed();

    void lockLayers(bool lock);
    void lockLayer(LayerItem *layer,
                   bool lock);

    void isDrag(bool drag);
    void switchMoveTool();

    void setDraw(bool draw);

    void updateBrushSize(bool larger);
    void updatedBrushStroke(int stroke);

    void updateTilePixmap(int id,
                          const QPixmap &pix);

    void openImages(QList<QUrl> urls);
    void openLayers(QList<QUrl> urls);

public slots:

    void doZoom(double scaleX,
                double scaleY);
    void setFit(bool value);
    void resetImageZoom();

    void setLayer(Magick::Image image,
                  int id);
    void addLayer(Magick::Image image,
                  bool updateView = true);
    void addLayer(int id,
                  QSize geo,
                  QSize pos,
                  bool updateView = true);

    CyanCommon::Layer getLayerFromOrder(int order);
    int getLastLayerID();
    int getLastLayerOrder();

    void clearLayers();
    Magick::Image getCanvas();
    QSize getCanvasSize();
    CyanCommon::Canvas getCanvasProject();

    void setLayerVisibility(int layer,
                            bool layerIsVisible);
    bool getLayerVisibility(int layer);

    void setLayerComposite(int layer,
                           Magick::CompositeOperator composite);

    Magick::CompositeOperator getLayerComposite(int layer);
    int getLayerCount();

    QList<QPair<int, int> > getSortedLayers();
    int getLayerOrder(int layer);
    int getLayerMinOrder();
    int getLayerMaxOrder();

    void moveLayerItemDown(int id);
    void moveLayerItemUp(int id);

    int getLayerItemIndex(int id);
    LayerItem* getLayerItemFromId(int id);
    LayerItem* getLayerItemUnderId(int id);
    LayerItem* getLayerItemOverId(int id);

    CyanCommon::Layer getLayer(int layer);
    void setLayer(int layer, Magick::Image image);
    void setLayerFromCanvas(CyanCommon::Canvas canvas,
                            int layer);
    void setLayersFromCanvas(CyanCommon::Canvas canvas);

    void updateCanvas(CyanCommon::Canvas canvas);

    void setLayerOrder(int layer, int order);
    QSize getLayerOffset(int layer);
    void setLayerOffset(int layer,
                        QSize offset);

    QString getLayerName(int layer);
    void setLayerName(int layer,
                      QString name);

    double getLayerOpacity(int layer);
    void setLayerOpacity(int layer,
                         double value,
                         bool update = true);

    void removeLayer(int layer);
    void setLockLayers(bool lock);

    void setCanvasSpecsFromImage(Magick::Image image);

    void setDrawMode(bool draw);
    void setInteractiveMode(View::InteractiveMode mode,
                            bool enable = true);

    void setBrushStroke(int stroke);
    void setBrushColor(const QColor &color);

    void setupCanvas(int width = 1024,
                     int height = 1024,
                     int depth = 8,
                     Magick::ColorspaceType colorspace = Magick::sRGBColorspace);

    bool supportsLayers();
    void enableLayers(bool enable);

    const QString getCanvasID();

    void refreshTiles();

    void setSelectedLayer(int id);

    void moveLayerEvent(QKeyEvent *e);

private slots:

    void handleLayerMoving(QPointF pos,
                           int id,
                           bool forceRender = false);
    void handleLayerMoved(QPointF pos,
                          int id);
    void handleLayerSelected(int id);

    int getParentLayer();
    const QString getParentCanvas();

    void initTiles(int tiles = 8);

    void clearScene();
    void clearTiles();
    QMap<int, CyanCommon::Tile> setupTiles(Magick::Image image,
                                       int tiles = 8);

    void handleLayerOverTiles(LayerItem *layerItem,
                              bool ignoreRunning = false);
    void handleLayerOverTiles(int layer);
    void handleTileStatus();
    void handleBrushOverTile(QPointF pos,
                             bool draw = true);

    void renderTile(int tile,
                           Magick::Image canvas,
                           QMap<int, CyanCommon::Layer> layers,
                           Magick::Geometry crop = Magick::Geometry());

    void paintCanvasBackground();

    void moveSelectedLayer(CyanCommon::MoveLayer gravity, int skip = 1);

protected:

    void wheelEvent(QWheelEvent* event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void resizeEvent(QResizeEvent *e);
    void keyPressEvent(QKeyEvent *e);
};

#endif // CYANCANVASVIEW_H
