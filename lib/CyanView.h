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

#include "CyanGlobal.h"
#include "CyanImageFormat.h"
//#include "cyan_layeritem.h"
#include "CyanLayerItem.h"
//#include "CyanRuler.h"
#include "CyanHistory.h"

#define TILE_Z 6
#define LAYER_Z 100
#define BRUSH_Z 200
#define GUIDE_Z 300

class CYANSHARED_EXPORT View : public QGraphicsView
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
    enum MoveLayer {
        MoveLayerUp,
        MoveLayerDown,
        MoveLayerLeft,
        MoveLayerRight
    };

    explicit View(QWidget* parent = nullptr,
                  bool setup = false,
                  bool native = false);
    ~View();
    bool fit;

private:
    QString _parentCanvas;
    int _parentLayer;
    CyanImageFormat::CyanCanvas _canvas;
    Magick::Image _image;
    QGraphicsScene *_scene;
    QGraphicsRectItem *_rect;
    QGraphicsEllipseItem *_brush;
    bool _drawing;
    bool _moving;
    int _selectedLayer;
    QFuture<void> future;
    bool _supportsLayers;
    //CyanRuler *_hRuler;
    //CyanRuler *_vRuler;
    bool _showGuides;
    View::InteractiveMode _mode;
    CyanHistory _history;

signals:
    void zoomChanged();
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
    void canvasStatusChanged();
    void moveLayerSignalUp(int id);
    void moveLayerSignalDown(int id);

public slots:
    void doZoom(double scaleX,
                double scaleY);
    void setFit(bool value);
    bool isFit();
    void resetImageZoom();

    void setLayer(Magick::Image image,
                  int id);
    void addLayer(Magick::Image image,
                  bool updateView = true,
                  bool isLocked = false,
                  bool isText = false);
    void addLayer(int id,
                  QSize geo,
                  QSize pos,
                  bool updateView = true,
                  bool isLocked = false);
    void duplicateLayer(int id);
    CyanImageFormat::CyanLayer getLayerFromOrder(int order);
    int getLastLayerID();
    int getLastLayerOrder();
    void clearLayers();
    Magick::Image getCanvas();
    QSize getCanvasSize();
    CyanImageFormat::CyanCanvas getCanvasProject();
    void setLayerVisibility(int layer,
                            bool layerIsVisible,
                            bool addToUndo = false);
    bool getLayerVisibility(int layer);
    void setLayerLock(int layer,
                      bool layerIsLocked);
    bool getLayerLock(int layer);
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
    void moveSelectedLayerDown();
    void moveSelectedLayerUp();
    int getLayerItemIndex(int id);
    LayerItem* getLayerItemFromId(int id);
    LayerItem* getLayerItemUnderId(int id);
    LayerItem* getLayerItemOverId(int id);
    CyanImageFormat::CyanLayer getLayer(int layer);
    void setLayer(int layer,
                  Magick::Image image);
    void setLayerFromCanvas(CyanImageFormat::CyanCanvas canvas,
                            int layer);
    void setLayersFromCanvas(CyanImageFormat::CyanCanvas canvas);

    void updateCanvas(CyanImageFormat::CyanCanvas canvas);
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
    View::InteractiveMode getInteractiveMode();
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
    void setLayerText(int id,
                      const QString &text,
                      int align = 0,
                      int rotate = 0,
                      bool update = true);
    qreal getZoomValue();
    bool isDirty();
    void setDirty(bool dirty);
    void addGuide(bool isHorizontal = false,
                  QPointF pos = QPointF(0,0));
    void showGuides(bool visibility);
    void setFilename(const QString &filename);
    const QString getFilename();

    void setUndo(bool state = true);
    void setRedo();

private slots:
    void handleLayerMoving(QPointF pos,
                           int id,
                           bool forceRender = false);
    void handleLayerMoved(QPointF pos,
                          QPointF lpos,
                          int id);
    void handleLayerMovedUndo(QPointF pos,
                              QPointF lpos,
                              int id);
    void handleLayerSelected(int id);
    int getParentLayer();
    const QString getParentCanvas();
    void initTiles(int tiles = 8);
    void clearScene();
    void clearTiles();
    QMap<int, CyanImageFormat::CyanTile> setupTiles(Magick::Image image,
                                                    int tiles = 8);
    void handleLayerOverTiles(LayerItem *layerItem,
                              bool ignoreRunning = false);
    void handleLayerOverTiles(int layer);
    void handleTileStatus();
    void handleBrushOverTile(QPointF pos,
                             bool draw = true);
    void renderTile(int tile,
                    Magick::Image canvas,
                    QMap<int, CyanImageFormat::CyanLayer> layers,
                    Magick::Geometry crop = Magick::Geometry());
    void paintCanvasBackground();
    void moveSelectedLayer(MoveLayer gravity,
                           int skip = 1);
    void handleCanvasChanged();
    void handleZoomChanged();
    void handleGuideMoved();
    void renderLayerText(int id, bool update);

    void addUndo(int id, QSize pos = QSize(0, 0), bool usePos = false);

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
