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

#include "CyanView.h"

#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include <QDebug>
#include <QPixmap>
#include <QImage>
#include <QTransform>
#include <QtConcurrent/QtConcurrent>
#include <QTimer>
#include <QGridLayout>

View::View(QWidget* parent,
           bool setup,
           bool native) :
    QGraphicsView(parent)
  , fit(false)
  , _scene(nullptr)
  , _rect(nullptr)
  , _brush(nullptr)
  , _drawing(false)
  , _moving(false)
  , _selectedLayer(0)
  , _supportsLayers(true)
  //, _hRuler(nullptr)
  //, _vRuler(nullptr)
  , _showGuides(true)
  , _mode(View::IteractiveMoveMode)
{
    // setup the basics
    setAcceptDrops(true);
    if (!native) {
        setBackgroundBrush(QColor(30, 30, 30));
    }
    setMouseTracking(true);

    // rulers
    /*setViewportMargins(CYAN_RULER_SIZE,
                       CYAN_RULER_SIZE,
                       0,
                       0);
    QGridLayout* gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(0);
    gridLayout->setMargin(0);

    _hRuler = new CyanRuler(CyanRuler::Horizontal,this);
    _vRuler = new CyanRuler(CyanRuler::Vertical, this);

    QWidget* container = new QWidget(this);
    container->setBackgroundRole(QPalette::Window);
    container->setFixedSize(CYAN_RULER_SIZE, CYAN_RULER_SIZE);
    gridLayout->addWidget(container, 0, 0);
    gridLayout->addWidget(_hRuler, 0, 1);
    gridLayout->addWidget(_vRuler, 1, 0);
    gridLayout->addWidget(viewport(), 1, 1);

    setLayout(gridLayout);

    connect(this, SIGNAL(zoomChanged()), this, SLOT(handleZoomChanged()));*/

    // TODO REMOVE
    _parentLayer = 0;
    _parentCanvas = QString();

    // setup scene
    _scene = new QGraphicsScene(this);
    setScene(_scene);

    // setup background rectangle
    _rect = new QGraphicsRectItem();
    QPen rectPen;
    rectPen.setColor(Qt::transparent);
    rectPen.setWidth(0);
    _rect->setPen(rectPen);
    _scene->addItem(_rect);

    // setup paint brush
    _brush = new QGraphicsEllipseItem();
    _brush->setRect(0,0,20,20);
    _brush->setPen(QPen(QColor(Qt::red)));
    _brush->hide();
    _scene->addItem(_brush);

    // connect zoom
    connect(this,
            SIGNAL(resetZoom()),
            this,
            SLOT(resetImageZoom()));

    // setup canvas if needed
    if (setup) { setupCanvas(); }
}

View::~View()
{
    // cleanup
    clearTiles();
    clearLayers();
    clearScene();
    emit viewClosed();
}

void View::wheelEvent(QWheelEvent* event) {
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    double scaleFactor = 1.15;
    if (event->delta() > 0) { // up
        if (_drawing && (event->buttons() & Qt::RightButton)) {
            // if drawing and mouse wheel up + right mouse button = larger brush
            int value =  qRound(_brush->rect().width()+1);
            if (value<0) { value = 1; }
            setBrushStroke(value);
        } else { // zoom in
            fit = false;
            emit myFit(false);
            scale(scaleFactor, scaleFactor);
            emit myZoom(scaleFactor, scaleFactor);
            emit zoomChanged();
        }
    } else { // down
        if (_drawing && (event->buttons() & Qt::RightButton)) {
            // if drawing and mouse wheel down + right mouse button = smaller brush
            int value =  qRound(_brush->rect().width()-1);
            if (value<0) { value = 1; }
            setBrushStroke(value);
        } else { // zoom out
            fit = false;
            emit myFit(false);
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
            emit myZoom(1.0 / scaleFactor, 1.0 / scaleFactor);
            emit zoomChanged();
        }
    }
}

void View::mousePressEvent(QMouseEvent *event)
{
    if (!isInteractive()) { setInteractive(true); }
    if (dragMode() != DragMode::NoDrag) { setDragMode(DragMode::NoDrag); }
    if (event->button() == Qt::MiddleButton) {
        //setDragMode(QGraphicsView::ScrollHandDrag);
        //emit isDrag(true);
        emit switchMoveTool();
        QMouseEvent fake(event->type(),
                         event->pos(),
                         Qt::LeftButton,
                         Qt::LeftButton,
                         event->modifiers());
        QGraphicsView::mousePressEvent(&fake);
    } else if (_drawing) {
        // get draw POS
        QPointF pos = mapToScene(event->pos());
        QPointF newPOS;
        newPOS.setX(pos.x()-(_brush->rect().width()/2));
        newPOS.setY(pos.y()-(_brush->rect().height()/2));
        _brush->setRect(newPOS.x(),
                        newPOS.y(),
                        _brush->rect().width(),
                        _brush->rect().height());
        if (event->buttons() & Qt::LeftButton) {
            // draw over tile at POS
            handleBrushOverTile(pos);
            //QtConcurrent::run(this, &View::handleBrushOverTile, pos, true);
        }
    }
    //else if ((event->buttons() & Qt::LeftButton) && (event->buttons() & Qt::RightButton)) { emit resetZoom(); }
    //else if (event->buttons() & Qt::RightButton) { qDebug() << "fofo"; }
    else if (event->buttons() & Qt::RightButton) {
        setDragMode(DragMode::ScrollHandDrag);
        setInteractive(false);
        QMouseEvent fake(event->type(),
                         event->pos(),
                         Qt::LeftButton,
                         Qt::LeftButton,
                         event->modifiers());
        QGraphicsView::mousePressEvent(&fake);
    }
    QGraphicsView::mousePressEvent(event);
}

void View::mouseReleaseEvent(QMouseEvent *event)
{
    if (!isInteractive()) { setInteractive(true); }
    if (dragMode() != DragMode::NoDrag) { setDragMode(DragMode::NoDrag); }
    if (event->button() == Qt::MiddleButton /*&& dragMode() != QGraphicsView::ScrollHandDrag*/) {
        //setDragMode(QGraphicsView::NoDrag);
        QMouseEvent fake(event->type(),
                         event->pos(),
                         Qt::LeftButton,
                         Qt::LeftButton,
                         event->modifiers());
        QGraphicsView::mouseReleaseEvent(&fake);
        emit isDrag(false);
        return;
    } /*else if (event->button() == Qt::LeftButton) {
        if (_drawing) {
            QPointF pos = mapToScene(event->pos());
            QPointF newPOS;
            newPOS.setX(pos.x()-(_brush->rect().width()/2));
            newPOS.setY(pos.y()-(_brush->rect().height()/2));
            _brush->setRect(newPOS.x(),
                            newPOS.y(),
                            _brush->rect().width(),
                            _brush->rect().height());
            // update tile at POS
            handleBrushOverTile(pos, false);
        }
    }*/
    /*else if (event->buttons() & Qt::RightButton) {
        setDragMode(DragMode::ScrollHandDrag);
        setInteractive(false);
        QMouseEvent fake(event->type(),
                         event->pos(),
                         Qt::LeftButton,
                         Qt::LeftButton,
                         event->modifiers());
        QGraphicsView::mousePressEvent(&fake);
    }*/
    QGraphicsView::mouseReleaseEvent(event);
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    //if (!isInteractive()) { setInteractive(true); }
    //if (dragMode() != DragMode::NoDrag) { setDragMode(DragMode::NoDrag); }
    if (_drawing) {
        // get draw POS
        QPointF pos = mapToScene(event->pos());
        QPointF newPOS;
        newPOS.setX(pos.x()-(_brush->rect().width()/2));
        newPOS.setY(pos.y()-(_brush->rect().height()/2));
        _brush->setRect(newPOS.x(),
                        newPOS.y(),
                        _brush->rect().width(),
                        _brush->rect().height());
        if (event->buttons() & Qt::LeftButton) {
            // draw over tile at POS
            handleBrushOverTile(pos);
            //QtConcurrent::run(this, &View::handleBrushOverTile, pos, true);
        }
    }
    /*else if (event->buttons() & Qt::RightButton) {
        setDragMode(DragMode::ScrollHandDrag);
        setInteractive(false);
    }*/
     QGraphicsView::mouseMoveEvent(event);
}

void View::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void View::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void View::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void View::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    qDebug() << mimeData->formats();
    if (mimeData->hasUrls()) {
        if (!_supportsLayers) { emit openImages(mimeData->urls()); }
        else { emit openLayers(mimeData->urls()); }
    }
}

void View::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e)
    if (fit) {
        fitInView(0,
                  0,
                  scene()->width(),
                  scene()->height(),
                  Qt::KeepAspectRatio);
    }
}

void View::keyPressEvent(QKeyEvent *e)
{
    int skip = 1;
    if(e->modifiers() & Qt::ShiftModifier) { skip = 10; }
    switch (e->key()) {
    case Qt::Key_Left:
        moveSelectedLayer(MoveLayerLeft, skip);
        return;
    case Qt::Key_Right:
        moveSelectedLayer(MoveLayerRight, skip);
        return;
    case Qt::Key_Up:
        moveSelectedLayer(MoveLayerUp, skip);
        return;
    case Qt::Key_Down:
        moveSelectedLayer(MoveLayerDown, skip);
        return;
    case Qt::Key_PageUp:
        moveSelectedLayerUp();
        return;
    case Qt::Key_PageDown:
        moveSelectedLayerDown();
        return;
    default:;
    }
    QGraphicsView::keyPressEvent(e);
}

void View::doZoom(double scaleX,
                  double scaleY)
{
    scale(scaleX,
          scaleY);
    emit zoomChanged();
}

void View::setFit(bool value)
{
    if (!scene()) { return; }
    fit = value;
    if (fit) {
        fitInView(0,
                  0,
                  scene()->width(),
                  scene()->height(),
                  Qt::KeepAspectRatio);
        emit zoomChanged();
    } else {
        resetImageZoom();
    }
}

bool View::isFit()
{
    return fit;
}

void View::resetImageZoom()
{
    fit = false;
    emit myFit(false);
    QMatrix matrix;
    matrix.scale(1.0, 1.0);
    setMatrix(matrix);
    emit zoomChanged();
}

void View::setLayer(Magick::Image image,
                    int id)
{
    _canvas.layers[id].image = image;
    refreshTiles();
}

void View::addLayer(Magick::Image image,
                    bool updateView,
                    bool isLocked,
                    bool isText)
{
    int id = getLastLayerID()+1;
    _canvas.layers[id].image = image;
    _canvas.layers[id].order = getLastLayerOrder()+1;
    _canvas.layers[id].isText = isText;
    //_canvas.layers[id].visible = true;
    _canvas.layers[id].composite = image.compose();

    qDebug() << "ADD IMAGE LAYER" << QString::fromStdString(image.label()) << id;
    if (!image.label().empty()) {
        _canvas.layers[id].label = QString::fromStdString(image.label());
    }

    LayerItem *layer = new LayerItem();
    qDebug() << "set layer rect size" << image.columns() << image.rows();
    layer->setLock(isLocked);
    layer->setRect(0,
                   0,
                   image.columns(),
                   image.rows());
    layer->setData(1,
                   id);

    connect(layer, SIGNAL(movingItem(QPointF,int)),
            this, SLOT(handleLayerMoving(QPointF,int)));
    connect(layer, SIGNAL(movedItem(QPointF,QPointF,int)),
            this, SLOT(handleLayerMoved(QPointF,QPointF,int)));
    connect(layer, SIGNAL(movedItem(QPointF,QPointF,int)),
            this, SLOT(handleLayerMovedUndo(QPointF,QPointF,int)));
    connect(layer, SIGNAL(selectedItem(int)),
            this, SLOT(handleLayerSelected(int)));
    connect(this, SIGNAL(viewClosed()),
            layer, SLOT(deleteLater()));
    connect(this, SIGNAL(lockLayer(LayerItem*,bool)),
            layer, SLOT(setMovable(LayerItem*,bool)));
    connect(this, SIGNAL(lockLayers(bool)),
            layer, SLOT(setMovable(bool)));
/*    connect(this, SIGNAL(isDrag(bool)),
            layer, SLOT(setDrag(bool)));*/
    connect(this, SIGNAL(setDraw(bool)),
            layer, SLOT(setDraw(bool)));

    _scene->addItem(layer);
    layer->setMovable(!_drawing);
    layer->setZValue(LAYER_Z);


    emit addedLayer(id);
    emit updatedLayers();

    handleCanvasChanged();

    if (updateView) { refreshTiles(); }
}

void View::addLayer(int id,
                    QSize geo,
                    QSize pos,
                    bool updateView,
                    bool isLocked)
{
    if (!_canvas.layers.contains(id) || id<0) {
        qDebug() << "can't add layer item without layer!" << id;
        return;
    }
    qDebug() << "==> ADD ITEM LAYER" << id << _canvas.layers[id].label << _canvas.layers[id].order << _canvas.layers[id].isText;
    LayerItem *layer = new LayerItem();
    layer->setLock(isLocked);
    layer->setRect(0,
                   0,
                   geo.width(),
                   geo.height());
    layer->setData(1,
                   id);

    connect(layer, SIGNAL(movingItem(QPointF,int)),
            this, SLOT(handleLayerMoving(QPointF,int)));
    connect(layer, SIGNAL(movedItem(QPointF,QPointF,int)),
            this, SLOT(handleLayerMoved(QPointF,QPointF,int)));
    connect(layer, SIGNAL(movedItem(QPointF,QPointF,int)),
            this, SLOT(handleLayerMovedUndo(QPointF,QPointF,int)));
    connect(layer, SIGNAL(selectedItem(int)),
            this, SLOT(handleLayerSelected(int)));
    connect(this, SIGNAL(viewClosed()),
            layer, SLOT(deleteLater()));
    connect(this, SIGNAL(lockLayer(LayerItem*,bool)),
            layer, SLOT(setMovable(LayerItem*,bool)));
    connect(this, SIGNAL(lockLayers(bool)),
            layer, SLOT(setMovable(bool)));
    /*connect(this, SIGNAL(isDrag(bool)),
            layer, SLOT(setDrag(bool)));*/
    connect(this, SIGNAL(setDraw(bool)),
            layer, SLOT(setDraw(bool)));

    _scene->addItem(layer);
    layer->setMovable(!_drawing);
    layer->setZValue(LAYER_Z);
    layer->setPos(pos.width(), pos.height());

    emit addedLayer(id);
    emit updatedLayers();

    handleCanvasChanged();
    if (updateView) { /*handleLayerOverTiles(layer);*/ refreshTiles(); }
}

void View::duplicateLayer(int id)
{
    if (id<0) { return; }
    qDebug() << "canvas duplicate layer" << id;

    addLayer(_canvas.layers[id].image, false);

    int origOrder = getLayerOrder(id);
    int dupOrder = getLastLayerOrder();
    int dupID = getLastLayerID();

    _canvas.layers[dupID].label = QString(tr("Copy of %1").arg(_canvas.layers[id].label));
    _canvas.layers[dupID].composite = _canvas.layers[id].composite;
    _canvas.layers[dupID].locked = _canvas.layers[id].locked;
    _canvas.layers[dupID].opacity = _canvas.layers[id].opacity;
    _canvas.layers[dupID].position = _canvas.layers[id].position;
    _canvas.layers[dupID].isText = _canvas.layers[id].isText;
    _canvas.layers[dupID].html = _canvas.layers[id].html;
    _canvas.layers[dupID].textAlign = _canvas.layers[id].textAlign;
    _canvas.layers[dupID].textRotate = _canvas.layers[id].textRotate;

    QMapIterator<int, CyanImageFormat::CyanLayer> layer(_canvas.layers);
    while (layer.hasNext()) {
        layer.next();
        if ((layer.value().order > origOrder) &&
            (layer.value().order < dupOrder))
        {
            _canvas.layers[layer.key()].order++;
        }
    }
    _canvas.layers[dupID].order = origOrder+1;

    for (int i=origOrder+1;i<dupOrder;++i) {
        moveLayerItemDown(dupID);
    }

    LayerItem *dupItem = getLayerItemFromId(dupID);
    if (dupItem) {
        dupItem->setPos(_canvas.layers[dupID].position.width(),
                        _canvas.layers[dupID].position.height());
    }

    handleCanvasChanged();
    emit updatedLayers();
    refreshTiles();
}

CyanImageFormat::CyanLayer View::getLayerFromOrder(int order)
{
    QMapIterator<int, CyanImageFormat::CyanLayer> layer(_canvas.layers);
    while (layer.hasNext()) {
        layer.next();
        if (layer.value().order==order) {
            return layer.value();
        }
    }
    return  CyanImageFormat::CyanLayer();
}

int View::getLastLayerID()
{
    int id = -1;
    QMapIterator<int, CyanImageFormat::CyanLayer> layer(_canvas.layers);
    while (layer.hasNext()) {
        layer.next();
        if (layer.key()>id) { id = layer.key(); }
    }
    return id;
}

int View::getLastLayerOrder()
{
    int order = -1;
    QMapIterator<int, CyanImageFormat::CyanLayer> layer(_canvas.layers);
    while (layer.hasNext()) {
        layer.next();
        if (layer.value().order>order) { order = layer.value().order; }
    }
    return order;
}

void View::clearLayers()
{
    _canvas.layers.clear();
    handleCanvasChanged();
    emit updatedLayers();
}

Magick::Image View::getCanvas()
{
    return _canvas.image;
}

QSize View::getCanvasSize()
{
    return QSize(static_cast<int>(_canvas.image.columns()),
                 static_cast<int>(_canvas.image.rows()));
}

CyanImageFormat::CyanCanvas View::getCanvasProject()
{
    return _canvas;
}

void View::setLayerVisibility(int layer,
                              bool layerIsVisible,
                              bool addToUndo)
{
    if (!_canvas.layers.contains(layer)) { return; }
    if (_canvas.layers[layer].visible != layerIsVisible) {
        if (addToUndo) { addUndo(layer); }
        _canvas.layers[layer].visible = layerIsVisible;
        handleLayerOverTiles(layer);
    }
    handleCanvasChanged();
}

bool View::getLayerVisibility(int layer)
{
    if (_canvas.layers.contains(layer)) {
        return _canvas.layers[layer].visible;
    }
    return false;
}

void View::setLayerLock(int layer, bool layerIsLocked)
{
    if (!_canvas.layers.contains(layer)) { return; }
    if (_canvas.layers[layer].locked != layerIsLocked) {
        _canvas.layers[layer].locked = layerIsLocked;
        LayerItem *item = getLayerItemFromId(layer);
        if (!item) { return; }
        qDebug() << "SET LAYER LOCK" << layer << layerIsLocked;
        item->setLock(layerIsLocked);
    }
    handleCanvasChanged();
}

bool View::getLayerLock(int layer)
{
    if (_canvas.layers.contains(layer)) {
        return _canvas.layers[layer].locked;
    }
    return false;
}

void View::setLayerComposite(int layer,
                             Magick::CompositeOperator composite)
{
    if (_canvas.layers[layer].composite != composite) {
        _canvas.layers[layer].composite = composite;
        handleLayerOverTiles(layer);
    }
    handleCanvasChanged();
}

Magick::CompositeOperator View::getLayerComposite(int layer)
{
    if (_canvas.layers.contains(layer)) {
        return _canvas.layers[layer].composite;
    }
    return Magick::NoCompositeOp;
}

int View::getLayerCount()
{
    return  _canvas.layers.size();
}

QList<QPair<int, int> > View::getSortedLayers()
{
    QList<QPair<int, int> > order;
    QMapIterator<int, CyanImageFormat::CyanLayer> layer(_canvas.layers);
    while (layer.hasNext()) {
        layer.next();
        QPair<int, int> pair(layer.value().order,
                             layer.key());
        order.append(pair);
    }
    std::sort(order.begin(),
              order.end(),
              CyanImageFormat::QPairSortFirst());
    return order;
}

int View::getLayerOrder(int layer)
{
    if (_canvas.layers.contains(layer)) {
        return _canvas.layers[layer].order;
    }
    return -1;
}

int View::getLayerMinOrder()
{
    return 0;
}

int View::getLayerMaxOrder()
{
    int index = -1;
    QList<QPair<int, int> > order = getSortedLayers();
    for (int i=0;i<order.size();++i) {
        int currentOrder = order.at(i).first;
        if (currentOrder>index) { index = currentOrder; }
    }
    return index;
}

void View::moveLayerItemDown(int id)
{
    int layerOverIndex=getLayerItemIndex(id);
    LayerItem* layerOver = getLayerItemFromId(id);
    if (layerOverIndex==-1 || !layerOver) { return; }
    LayerItem* layerUnder =  getLayerItemUnderId(id);
    if (!layerUnder) { return; }

    qDebug() << "RESTACK ITEMS (move layer down) ...";
    layerOver->stackBefore(layerUnder);
    handleCanvasChanged();
}

void View::moveLayerItemUp(int id)
{
    int layerUnderIndex = getLayerItemIndex(id);
    LayerItem* layerUnder = getLayerItemFromId(id);
    if (layerUnderIndex==-1 || !layerUnder) { return; }
    LayerItem* layerOver =  getLayerItemOverId(id);
    if (!layerOver) { return; }

    qDebug() << "RESTACK ITEMS (move layer up) ...";
    layerOver->stackBefore(layerUnder);
    handleCanvasChanged();
}

void View::moveSelectedLayerDown()
{
    qDebug() << "move selected layer down" << _selectedLayer;
    for (int i=0;i<_scene->items().size();++i) {
        LayerItem *item = dynamic_cast<LayerItem*>(_scene->items().at(i));
        if (!item) { continue; }
        if (item->getID() != _selectedLayer) { continue; }
        emit moveLayerSignalDown(item->getID());
        return;
    }
}

void View::moveSelectedLayerUp()
{
    qDebug() << "move selected layer up" << _selectedLayer;
    for (int i=0;i<_scene->items().size();++i) {
        LayerItem *item = dynamic_cast<LayerItem*>(_scene->items().at(i));
        if (!item) { continue; }
        if (item->getID() != _selectedLayer) { continue; }
        emit moveLayerSignalUp(item->getID());
        return;
    }
}

int View::getLayerItemIndex(int id)
{
    if (id>-1) {
        QList<QGraphicsItem*> items = _scene->items();
        for (int i=0;i<items.size();++i) {
            LayerItem *item = dynamic_cast<LayerItem*>(items.at(i));
            if (!item) { continue; }
            if (item->getID()==id) {
                qDebug() << "found item at index" << i;
                return i;
            }
        }
    }
    return -1;
}

LayerItem *View::getLayerItemFromId(int id)
{
    QList<QGraphicsItem*> items = _scene->items();
    for (int i=0;i<items.size();++i) {
        LayerItem *item = dynamic_cast<LayerItem*>(items.at(i));
        if (!item) { continue; }
        if (item->getID()==id) {
            return item;
        }
    }
    return nullptr;
}

LayerItem *View::getLayerItemUnderId(int id)
{
    int index = -1;
    QList<QGraphicsItem*> items = _scene->items();
    for (int i=0;i<items.size();++i) {
        LayerItem *item = dynamic_cast<LayerItem*>(items.at(i));
        if (!item) { continue; }
        if (item->getID()==id) {
            index = i;
            break;
        }
    }
    if (index>-1 && items.size()>=index) {
        return dynamic_cast<LayerItem*>(items.at(index+1));
    }
    return nullptr;
}

LayerItem *View::getLayerItemOverId(int id)
{
    int index = -1;
    QList<QGraphicsItem*> items = _scene->items();
    for (int i=0;i<items.size();++i) {
        LayerItem *item = dynamic_cast<LayerItem*>(items.at(i));
        if (!item) { continue; }
        if (item->getID()==id && i>0) {
            qDebug() << "found layer under at" <<i;
            index = i;
            break;
        }
    }
    if (index>-1 && items.size()>=index) {
        return dynamic_cast<LayerItem*>(items.at(index-1));
    }
    return nullptr;
}

CyanImageFormat::CyanLayer View::getLayer(int layer)
{
    if (_canvas.layers.contains(layer)) {
        return _canvas.layers[layer];
    }
    return  CyanImageFormat::CyanLayer();
}

void View::setLayer(int layer, Magick::Image image)
{
    _canvas.layers[layer].image = image;
    emit updatedLayers();
}

void View::setLayerFromCanvas(CyanImageFormat::CyanCanvas canvas,
                              int layer)
{
    _canvas.layers[layer].image = canvas.image;
    _canvas.layers[layer].layers = canvas.layers;
    emit updatedLayers();
}

void View::setLayersFromCanvas(CyanImageFormat::CyanCanvas canvas)
{
    if (_canvas.layers.size()>0) {
        qDebug() << "DONT SET LAYERS IF EXISTS!";
        return;
    }
    _canvas.layers = canvas.layers;

    // re-order layers for correct stacking
    QList<QPair<int, int> > order;
    QMapIterator<int, CyanImageFormat::CyanLayer> layer(_canvas.layers);
    while (layer.hasNext()) {
        layer.next();
        QPair<int, int> pair(layer.value().order,
                             layer.key());
        order.append(pair);
    }
    std::sort(order.begin(),
              order.end(),
              CyanImageFormat::QPairSortFirst());
    for (int i=0;i<order.size();++i) { // add layer item
        int id = order.at(i).second;
        if (!_canvas.layers[id].html.isEmpty() && _canvas.layers[id].isText) {
            qDebug() << "need to render text layer...";
            _canvas.layers[id].image = CyanImageFormat::renderText(_canvas.layers[id]);
        }
        qDebug() << "add layer item for id" << id;
        addLayer(id,
                 QSize(static_cast<int>(_canvas.layers[id]
                                        .image.columns()),
                       static_cast<int>(_canvas.layers[id]
                                        .image.rows())),
                 _canvas.layers[id].position,
                 false,
                 _canvas.layers[id].locked);
    }



    /*QMapIterator<int, CyanCommon::Layer> layers(_canvas.layers);
    while (layers.hasNext()) {
        layers.next();
        if (!layers.value().text.isEmpty() && layers.value().isText) {
            qDebug() << "need to render text layer...";
            _canvas.layers[layers.key()].image = Render::renderText(layers.value());
        }
        qDebug() << "add layer" << layers.key();
        addLayer(layers.key(),
                 QSize(static_cast<int>(layers.value()
                                        .image.columns()),
                       static_cast<int>(layers.value()
                                        .image.rows())),
                 layers.value().pos,
                 false,
                 layers.value().locked);
        //handleLayerOverTiles(layers.key());
    }*/
    emit updatedLayers();
    refreshTiles();
}

void View::updateCanvas(CyanImageFormat::CyanCanvas canvas)
{
    _image = canvas.image;
    _canvas = canvas;
    handleCanvasChanged();
    refreshTiles();
}

void View::setLayerOrder(int layer, int order)
{
    _canvas.layers[layer].order = order;
    handleCanvasChanged();
    emit updatedLayers();
    refreshTiles();
}

QSize View::getLayerOffset(int layer)
{
    return _canvas.layers[layer].position;
}

void View::setLayerOffset(int layer,
                          QSize offset)
{
    _canvas.layers[layer].position = offset;
    handleCanvasChanged();
}

QString View::getLayerName(int layer)
{
    return  _canvas.layers[layer].label;
}

void View::setLayerName(int layer,
                        QString name)
{
    _canvas.layers[layer].label = name;
    handleCanvasChanged();
}

double View::getLayerOpacity(int layer)
{
    return _canvas.layers[layer].opacity;
}

void View::setLayerOpacity(int layer,
                           double value,
                           bool update)
{
    _canvas.layers[layer].opacity = value;
    if (update) { handleLayerOverTiles(layer); }
    handleCanvasChanged();
}

void View::removeLayer(int layer)
{
    if (layer<0) { return; }
    QList<QGraphicsItem*> items = _scene->items();
    for (int i=0;i<items.size();++i) {
        LayerItem *item = dynamic_cast<LayerItem*>(items.at(i));
        if (!item) { continue; }
        if (item->getID()==layer) {
            qDebug() << "remove layer graphics item";
            _scene->removeItem(item);
            item->deleteLater();
            break;
        }
    }
    _canvas.layers.remove(layer);
    handleCanvasChanged();
    emit updatedLayers();
    emit statusMessage(tr("Removed layer %1 from canvas")
                       .arg(layer));
    refreshTiles();
}

void View::setupCanvas(int width,
                       int height,
                       int depth,
                       Magick::ColorspaceType colorspace)
{
    qDebug() << "setup canvas" << width << height << depth;

    // setup new image
    _image.size(Magick::Geometry(static_cast<size_t>(width),
                                 static_cast<size_t>(height)));
    _image.depth(static_cast<size_t>(depth));
    _image.colorSpace(colorspace);

    // set transparent image
    _image.alpha(true);
    _image.evaluate(Magick::AlphaChannel,
                    Magick::MultiplyEvaluateOperator,
                    0.0);

    // set scene size
    qDebug() << "set scene size" << width << height;
    _scene->setSceneRect(0,
                         0,
                         width,
                         height);

    // set background rectangle size
    _rect->setRect(_scene->sceneRect());

    // copy canvas
    _canvas.image = _image;

    // set dirty
    _canvas.dirty = true;

    // set timestamp
    //_canvas.timestamp = CyanCommon::timestamp();

    // setup canvas tiles
    initTiles();

    // paint background
    paintCanvasBackground();

    // reset zoom
    resetImageZoom();

    // update
    emit updatedLayers();
}

bool View::supportsLayers()
{
    return _supportsLayers;
}

void View::enableLayers(bool enable)
{
    _supportsLayers = enable;
}

const QString View::getCanvasID()
{
    return _canvas.uid;
}

void View::refreshTiles()
{
    qDebug() << "REFRESH TILES";

    QMapIterator<int, CyanImageFormat::CyanTile> tiles(_canvas.tiles);
    while (tiles.hasNext()) {
        tiles.next();
        int tile = tiles.key();
        // get crop info
        Magick::Geometry geo(static_cast<size_t>(_canvas.tileSize.width()),
                             static_cast<size_t>(_canvas.tileSize.height()),
                             static_cast<ssize_t>(_canvas.tiles[tile]
                                                  .rect->boundingRect()
                                                  .topLeft().x()),
                             static_cast<ssize_t>(_canvas.tiles[tile]
                                                  .rect->boundingRect()
                                                  .topLeft().y()));
        // render tile
        /*future =*/ QtConcurrent::run(this,
                                   &View::renderTile,
                                   tile,
                                   _image,
                                   _canvas.layers,
                                   geo);
    }
}

void View::setSelectedLayer(int id)
{
    _selectedLayer = id;
    //emit selectedLayer(id);
}

void View::moveLayerEvent(QKeyEvent *e)
{
    View::keyPressEvent(e);
}

void View::setLayerText(int id,
                        const QString &text,
                        int align,
                        int rotate,
                        bool update)
{
    if (!_canvas.layers.contains(id)) { return; }
    qDebug() << "view set layer text" << id << text;

    if (!_canvas.layers[id].isText) {
        emit statusMessage(tr("Not a text layer"));
        return;
    }

    bool needUpdate = false;
    if (align != _canvas.layers[id].textAlign) {
        _canvas.layers[id].textAlign = align;
        needUpdate = true;
    }
    if (rotate != _canvas.layers[id].textRotate) {
        _canvas.layers[id].textRotate = rotate;
        needUpdate = true;
    }
    if (_canvas.layers[id].html == text && !needUpdate) { return; }

    _canvas.layers[id].html = text;
    handleCanvasChanged();

    QtConcurrent::run(this, &View::renderLayerText, id, update);
}

qreal View::getZoomValue()
{
    return transform().m11();
}

bool View::isDirty()
{
    return _canvas.dirty;
}

void View::setDirty(bool dirty)
{
    _canvas.dirty = dirty;
    emit canvasStatusChanged();
}

void View::addGuide(bool isHorizontal, QPointF pos)
{
    qDebug() << "add guide!" << isHorizontal << pos;
    int id = _canvas.guides.size();
    _canvas.guides[id] = new CyanGuideItem(nullptr, isHorizontal);
    _scene->addItem(_canvas.guides[id]);
    if (isHorizontal) { _canvas.guides[id]->setRect(0,0,_scene->width(), 0); }
    else { _canvas.guides[id]->setRect(0,0,0,_scene->height()); }
    if (pos.x() != 0 || pos.y() != 0) {
        _canvas.guides[id]->setPos(pos);
    }
    _canvas.guides[id]->setZValue(GUIDE_Z);
    connect(_canvas.guides[id], SIGNAL(movedItem()), this, SLOT(handleGuideMoved()));
    handleCanvasChanged();
}

void View::showGuides(bool visibility)
{
    qDebug() << "view show guides?" << visibility;
    if (_showGuides == visibility) { return; }
    _showGuides = visibility;
    for (int i=0;i<_canvas.guides.size();++i) {
        _canvas.guides[i]->setVisible(visibility);
    }
}

void View::setFilename(const QString &filename)
{
    if (filename.isEmpty()) { return; }
    _canvas.filename = filename;
}

const QString View::getFilename()
{
    return _canvas.filename;
}

void View::setUndo(bool state)
{
    int total = 0;
    if (state) {
        total = _history.getUndoTotal();
    } else {
        total = _history.getRedototal();
    }
    if (total < 1) {
        emit statusMessage(tr("%1: Buffer is empty").arg(state?tr("Undo"):tr("Redo")));
        return;
    }
    CyanHistory::CyanHistoryItem undo = state?_history.getUndo():_history.getRedo();
    if (!_canvas.layers.contains(undo.layer)) {
        emit statusMessage(tr("%1: Layer %2 is not valid anymore").arg(state?tr("Undo"):tr("Redo")).arg(undo.layer));
        return;
    }

    // position
    if (_canvas.layers[undo.layer].position != undo.position) {
        qDebug() << "SET POSITION";
        _canvas.layers[undo.layer].position = undo.position;
        for (int i=0;i<_scene->items().size();++i) {
            LayerItem *item = dynamic_cast<LayerItem*>(_scene->items().at(i));
            if (!item) { continue; }
            if (item->getID() != undo.layer) { continue; }
            item->setPos(undo.position.width(), undo.position.height());
            handleLayerOverTiles(item);
            break;
        }
    }

    // visibility
    /*if (_canvas.layers[undo.layer].visible != undo.visibility) {
        qDebug() << "SET VISIBILITY";
        setLayerVisibility(undo.layer, undo.visibility, false);
        emit updatedLayers();
    }*/

    handleCanvasChanged();
    if (state) {_history.clearLastUndo(); }
    else { _history.clearLastRedo(); }
}

void View::setRedo()
{
    setUndo(false);
}

// TODO
void View::setCanvasSpecsFromImage(Magick::Image image)
{
    qDebug() << "set canvas specs from image" << image.columns() << image.rows();
    _image.label(image.label());
    _image.size(Magick::Geometry(image.columns(), image.rows()));
    _image.depth(image.depth());
    _image.colorSpace(image.colorSpace());
    _image.iccColorProfile(image.iccColorProfile());

    _image.alpha(true);
    _image.evaluate(Magick::AlphaChannel,
                    Magick::MultiplyEvaluateOperator,
                    0.0);

    qDebug() << "set scene size" << _image.columns() << image.rows();
    _scene->setSceneRect(0, 0, _image.columns(), _image.rows());
    _rect->setRect(_scene->sceneRect());
    _canvas.image = _image;
    _canvas.filename = QString::fromStdString(image.fileName());

    _canvas.dirty = true; // dirty

    // save color profile
    _canvas.profile = image.iccColorProfile();
    if (_canvas.profile.length()==0) {
        emit errorMessage(tr("Missing color profile!"));
    }

    // set timestamp
    /*if (_canvas.timestamp.isEmpty()) {
        _canvas.timestamp = CyanCommon::timestamp();
    }*/

    // setup canvas tiles
    initTiles();

    paintCanvasBackground();

    emit updatedLayers();
}

void View::setDrawMode(bool draw)
{
    qDebug() << "set draw mode" << draw;
    emit setDraw(draw);
    _drawing = draw;
    _brush->setVisible(draw);
    //_mode = View::InteractiveDrawMode;
    QPoint cursor = mapFromGlobal(QCursor::pos());
    QPointF pos = mapToScene(cursor);
    _brush->setRect(pos.x()-(_brush->rect().width()/2),
                    pos.y()-(_brush->rect().height()/2),
                    _brush->rect().width(),
                    _brush->rect().height());
    _brush->setZValue(BRUSH_Z);
}

void View::setInteractiveMode(View::InteractiveMode mode, bool enable)
{
    Q_UNUSED(enable)
    qDebug() << "set interactive mode" << mode << enable;
    _mode = mode;
    switch(mode) {
    case View::InteractiveMode::IteractiveMoveMode:
        setDragMode(QGraphicsView::NoDrag);
        setInteractive(true);
        setDrawMode(false);
        _moving = true;
        setCursor(Qt::CrossCursor);
        break;
    /*case View::InteractiveMode::InteractiveDragMode:
        setDragMode(QGraphicsView::ScrollHandDrag);
        setInteractive(true);
        setDrawMode(false);
        _moving = false;
        //setCursor(Qt::OpenHandCursor);
        break;*/
    case View::InteractiveMode::InteractiveDrawMode:
        setDragMode(QGraphicsView::NoDrag);
        setInteractive(true);
        setDrawMode(true);
        _moving = false;
        setCursor(Qt::PointingHandCursor);
        break;
    default:;
    }
}

View::InteractiveMode View::getInteractiveMode()
{
    return _mode;
}

void View::setBrushStroke(int stroke)
{
    QPoint cursor = mapFromGlobal(QCursor::pos());
    QPointF pos = mapToScene(cursor);
    _brush->setRect(pos.x()-(_brush->rect().width()/2),
                    pos.y()-(_brush->rect().height()/2),
                    stroke,
                    stroke);
    emit updatedBrushStroke(stroke);
}

void View::setBrushColor(const QColor &color)
{
    _canvas.brush.color = color;
}

void View::handleLayerMoving(QPointF pos, int id, bool forceRender)
{
    if (!_canvas.layers.contains(id) || id<0) { return; }

    _canvas.layers[id].position = QSize(static_cast<int>(pos.x()),
                                        static_cast<int>(pos.y()));

    LayerItem *layerItem = dynamic_cast<LayerItem*>(sender());
    if (!layerItem) { return; }

    handleLayerOverTiles(layerItem, forceRender);
}

void View::handleLayerMoved(QPointF pos,
                            QPointF lpos,
                            int id)
{
    Q_UNUSED(lpos)
    handleLayerMoving(pos,
                      id,
                      true);
    handleCanvasChanged();
}

void View::handleLayerMovedUndo(QPointF pos, QPointF lpos, int id)
{
    qDebug() << "handleLayerMovedUndo" << id << pos << lpos;
    QSize npos = QSize(0, 0);
    bool usePOS = false;
    if (pos != lpos) {
        npos = QSize(lpos.x(), lpos.y());
        usePOS = true;
    }
    addUndo(id, npos, usePOS);
}

void View::handleLayerSelected(int id)
{
    _selectedLayer = id;
    emit selectedLayer(id);
}

int View::getParentLayer()
{
    return _parentLayer;
}

const QString View::getParentCanvas()
{
    return _parentCanvas;
}

void View::initTiles(int tiles)
{
    // clear existing
    clearTiles();

    // setup tile size
    int width = static_cast<int>(_canvas.image.columns())/tiles;
    int height =static_cast<int>(_canvas.image.rows())/tiles;
    _canvas.tileSize = QSize(width, height);

    qDebug() << "INIT TILES" << _canvas.tileSize << _canvas.image.columns() << _canvas.image.rows();

    // setup tiles
    _canvas.tiles = setupTiles(_canvas.image, tiles);
}

void View::clearScene()
{
    _scene->clear();
}

void View::clearTiles()
{
    QMapIterator<int, CyanImageFormat::CyanTile> i(_canvas.tiles);
    while(i.hasNext()) {
        i.next();
        _scene->removeItem(i.value().rect);
        i.value().rect->deleteLater();
    }
    _canvas.tiles.clear();
}

QMap<int, CyanImageFormat::CyanTile> View::setupTiles(Magick::Image image,
                                                      int tiles)
{
    QMap<int, CyanImageFormat::CyanTile> result;

    int wTiles = tiles;
    int hTiles = tiles;

    // tile size
    int width = static_cast<int>(image.columns())/tiles;
    int height = static_cast<int>(image.rows())/tiles;

    float fWidth = static_cast<float>(image.columns())/tiles;
    float fHeight = static_cast<float>(image.rows())/tiles;

    if (fWidth != width) {
        wTiles++;
    }
    if (fHeight != height) {
        hTiles++;
    }

    qDebug() << "SETUP TILES" << width << height << wTiles << hTiles;
    int row = 0;
    int rowItems = 0;
    for (int i=0;i<(wTiles*hTiles);++i) {
        result[i].rect = new CyanTileItem(nullptr,
                                          _scene->addPixmap(QPixmap(/*width, height*/)));

        connect(this,
                SIGNAL(updateTilePixmap(int, QPixmap)),
                result[i].rect,
                SLOT(setPixmap(int, QPixmap)));

        // set tile id
        result[i].rect->setData(0, i);

        QPointF offset;
        offset.setX(i==0?0:width*rowItems);
        offset.setY(i==0?0:height*row);

        result[i].rect->getPixmapItem()->setOffset(offset);
        result[i].rect->setRect(i==0?0:width*rowItems,
                                i==0?0:height*row,
                                width,
                                height);

        _scene->addItem(result[i].rect);
        result[i].rect->setZValue(TILE_Z);
        if (rowItems == wTiles-1) {
            row++;
            rowItems=0;
        } else { rowItems++; }
    }
    return  result;
}

void View::handleLayerOverTiles(LayerItem *layerItem,
                                bool ignoreRunning)
{
    if (!ignoreRunning && future.isRunning()) { return; }
    if (!layerItem) { return; }

    QList<QGraphicsItem*> items = _scene->collidingItems(layerItem, Qt::IntersectsItemBoundingRect);
    for (int i=0;i<items.size();++i) {
        CyanTileItem *item = dynamic_cast<CyanTileItem*>(items.at(i));
        if (!item) { continue; }

        // get tile id
        int tile=item->data(0).toInt();

#ifdef SHOW_TILES
        QPen newPen(Qt::green);
        newPen.setWidth(0);
        _canvas.tiles[tile].rect->setPen(newPen);
#endif

        _canvas.tiles[tile].rect->layers[layerItem->getID()] = true;

        // get crop info
        Magick::Geometry geo(static_cast<size_t>(_canvas.tileSize.width()),
                             static_cast<size_t>(_canvas.tileSize.height()),
                             static_cast<ssize_t>(_canvas.tiles[tile]
                                                  .rect->boundingRect()
                                                  .topLeft().x()),
                             static_cast<ssize_t>(_canvas.tiles[tile]
                                                  .rect->boundingRect()
                                                  .topLeft().y()));

        // render tile
        future = QtConcurrent::run(this,
                                   &View::renderTile,
                                   tile,
                                   _image,
                                   _canvas.layers,
                                   geo);
    }
    handleTileStatus();
}

void View::handleLayerOverTiles(int layer)
{
    for (int i=0;i<_scene->items().size();++i) {
        LayerItem *item = dynamic_cast<LayerItem*>(_scene->items().at(i));
        if (!item) { continue; }
        if (item->getID() == layer) {
            handleLayerOverTiles(item);
            break;
        }
    }
}

void View::handleTileStatus()
{
    /*if (future.isRunning()) {
        QTimer::singleShot(50, this, SLOT(handleTileStatus()));
        return;
    }*/

    QMapIterator<int, CyanImageFormat::CyanTile> tiles(_canvas.tiles);
    while (tiles.hasNext()) {
        tiles.next();
        int tile = tiles.key();

        QVector<int> colliding;
        QList<QGraphicsItem*> items = _scene->collidingItems(_canvas.tiles[tile].rect, Qt::IntersectsItemBoundingRect);
        for (int i=0;i<items.size();++i) {
            LayerItem *item = dynamic_cast<LayerItem*>(items.at(i));
            if (!item) { continue; }
            _canvas.tiles[tile].rect->layers[item->getID()] = true;
            colliding.append(item->getID());
        }
        QMapIterator<int, bool> layer(_canvas.tiles[tile].rect->layers);
        while (layer.hasNext()) {
            layer.next();
            if (!colliding.contains(layer.key()) && layer.value()) {
                _canvas.tiles[tile].rect->layers[layer.key()] = false;
#ifdef SHOW_TILES
                QPen newPen(Qt::red);
                newPen.setWidth(0);
                _canvas.tiles[tile].rect->setPen(newPen);
#endif
                // get crop
                Magick::Geometry crop(static_cast<size_t>(_canvas.tileSize.width()),
                                      static_cast<size_t>(_canvas.tileSize.height()),
                                      static_cast<ssize_t>(_canvas.tiles[tile]
                                                           .rect->boundingRect()
                                                           .topLeft().x()),
                                      static_cast<ssize_t>(_canvas.tiles[tile]
                                                           .rect->boundingRect()
                                                           .topLeft().y()));

                // render tile
                /*future =*/QtConcurrent::run(this,
                                           &View::renderTile,
                                           tile,
                                           _image,
                                           _canvas.layers,
                                           crop);
            }
        }
    }
}

void View::handleBrushOverTile(QPointF pos, bool draw)
{
    if (_scene->items().size()==0) { return; }
    QList<QGraphicsItem*> items = _scene->collidingItems(_brush);
    bool foundLayer = false;
    for (int i=0;i<items.size();++i) {

        LayerItem *layerItem = dynamic_cast<LayerItem*>(items.at(i));
        if (layerItem && draw && !foundLayer) { // draw
            foundLayer = true;
            QPointF epos;
            int id = layerItem->getID();
            if (!_canvas.layers.contains(id)) { continue; }
            if (_canvas.layers[id].isText) { continue; }
            epos.setX(pos.x()-_canvas.layers[id].position.width());
            epos.setY(pos.y()-_canvas.layers[id].position.height());

            _canvas.layers[id].image.strokeAntiAlias(_canvas.brush.antialiasing);
            _canvas.layers[id].image.strokeLineCap(_canvas.brush.lineCap);
            _canvas.layers[id].image.strokeLineJoin(_canvas.brush.lineJoin);
            _canvas.layers[id].image.strokeWidth(_brush->rect().width());

            if (_canvas.layers[id].image.colorSpace() == Magick::CMYKColorspace) {
                // TODO! : need im workaround for this to work
                /*_canvas.layers[id].image.strokeColor(Magick::ColorRGB(_canvas.brush.color.cyanF(),
                                                                      _canvas.brush.color.magentaF(),
                                                                      _canvas.brush.color.yellowF()));*/
                qWarning() << "DRAW ON CMYK NOT SUPPORTED";
            } else {
                _canvas.layers[id].image.strokeColor(Magick::ColorRGB(_canvas.brush.color.redF(),
                                                                      _canvas.brush.color.greenF(),
                                                                      _canvas.brush.color.blueF()));
            }
            try {
                _canvas.layers[id].image.draw(Magick::DrawableLine(epos.x()-1,
                                                                   epos.y()-1,
                                                                   epos.x(),
                                                                   epos.y()));
            }
            catch(Magick::Error &error_ ) { emit errorMessage(error_.what()); }
            catch(Magick::Warning &warn_ ) { emit warningMessage(warn_.what()); }
        }

        CyanTileItem *item = dynamic_cast<CyanTileItem*>(items.at(i));
        if (!item) { continue; }

        // get tile id
        int tile=item->data(0).toInt();

        // get crop
        Magick::Geometry crop(static_cast<size_t>(_canvas.tileSize.width()),
                              static_cast<size_t>(_canvas.tileSize.height()),
                              static_cast<ssize_t>(_canvas.tiles[tile]
                                                   .rect->boundingRect()
                                                   .topLeft().x()),
                              static_cast<ssize_t>(_canvas.tiles[tile]
                                                   .rect->boundingRect()
                                                   .topLeft().y()));

        // render tile
        renderTile(tile,
                   _image,
                   _canvas.layers,
                   crop);
    }
}

void View::renderTile(int tile,
                      Magick::Image canvas,
                      QMap<int, CyanImageFormat::CyanLayer> layers,
                      Magick::Geometry crop)
{
    canvas.quiet(true);
    if (crop.width()==0 || tile==-1 || layers.size()==0 || canvas.columns()==0) { return; }

    // comp tile and write pixmap
    Magick::Image tmp = CyanImageFormat::compLayers(canvas, layers, crop);
    Magick::Blob preview;
    tmp.quiet(true);
    try {
        tmp.magick("BMP");
        tmp.write(&preview);
    }
    catch(Magick::Error &error_ ) { emit errorMessage(error_.what()); }
    catch(Magick::Warning &warn_ ) { emit warningMessage(warn_.what()); }

    // update tile pixmap
    if (preview.length()>0) {
        QPixmap pix = QPixmap::fromImage(QImage::fromData(reinterpret_cast<uchar*>(const_cast<void*>(preview.data())),
                                                          static_cast<int>(preview.length())));
        if (pix.isNull()) { return; }
        emit updateTilePixmap(tile, pix);
    }
}

// paint a checkerboard background to show transparency in image
void View::paintCanvasBackground()
{
    try {
        Magick::Image cb;
        cb.quiet(true);
        cb.alpha(true);
        cb.size(Magick::Geometry(30, 30));
        cb.alpha(true);
        cb.read("pattern:checkerboard");
        cb.scale(Magick::Geometry(256, 256));
        cb.magick("BMP");

        Magick::Blob pix;
        cb.write(&pix);
        if (pix.length()>0) {
            _rect->setBrush(QBrush(QImage::fromData(reinterpret_cast<uchar*>(const_cast<void*>(pix.data())),
                                                    static_cast<int>(pix.length()))));
        }
    }
    catch(Magick::Error &error_ ) { emit errorMessage(error_.what()); }
    catch(Magick::Warning &warn_ ) { emit warningMessage(warn_.what()); }
}

void View::moveSelectedLayer(MoveLayer gravity, int skip)
{
    qDebug() << "move selected layer" << _selectedLayer << gravity;
    addUndo(_selectedLayer);
    for (int i=0;i<_scene->items().size();++i) {
        LayerItem *item = dynamic_cast<LayerItem*>(_scene->items().at(i));
        if (!item) { continue; }

        QPen newPen(Qt::transparent);
        newPen.setWidth(0);
        item->setPen(newPen);

        if (item->getID() != _selectedLayer) { continue; }
        QPointF pos = item->pos();
        switch (gravity) {
        case MoveLayerUp:
            pos.setY(pos.y()-skip);
            break;
        case MoveLayerDown:
            pos.setY(pos.y()+skip);
            break;
        case MoveLayerLeft:
            pos.setX(pos.x()-skip);
            break;
        case MoveLayerRight:
            pos.setX(pos.x()+skip);
            break;
        }
        item->setPos(pos);
        handleLayerMoved(pos, pos, _selectedLayer);
        handleLayerOverTiles(item);
    }

}

void View::handleCanvasChanged()
{
    // canvas changed
    if (!_canvas.dirty) { _canvas.dirty = true; }
    emit canvasStatusChanged();
}

void View::handleZoomChanged()
{
    /*qreal zoom = getZoomValue();
    qDebug() << "zoom" << zoom;
    _hRuler->setRulerZoom(zoom);
    _vRuler->setRulerZoom(zoom);*/
}

void View::handleGuideMoved()
{
    qDebug() << "handle guide moved";
    handleCanvasChanged();
}

void View::renderLayerText(int id, bool update)
{
    if (!_canvas.layers.contains(id) || id<0) { return; }
    _canvas.layers[id].image = CyanImageFormat::renderText(_canvas.layers[id]);
    if (update) { handleLayerOverTiles(id); }
}

void View::addUndo(int id, QSize pos, bool usePos)
{
    if (!_canvas.layers.contains(id)) { return; }
    CyanImageFormat::CyanLayer layer = _canvas.layers[id];
    CyanHistory::CyanHistoryItem item;
    item.layer = id;
    //item.order = layer.order;
    //item.locked = layer.locked;
    if (usePos) {
        // LEGACY
        item.position = pos;
        item.undoPOS = pos;
        item.redoPOS = layer.position;
        // NEW
        item.pos.state = QPointF(pos.width(), pos.height());
        item.pos.undo = item.pos.state;
        item.pos.redo = QPointF(layer.position.width(), layer.position.height());
    }
    else {
        // LEGACY
        item.position = layer.position;
        item.undoPOS = layer.position;
        item.redoPOS = layer.position;
        // NEW
        item.pos.state = QPointF(layer.position.width(), layer.position.height());
        item.pos.undo = item.pos.state;
        item.pos.redo = item.pos.state;
    }
    //item.opacity = layer.opacity;
    item.composite = layer.composite;
    _history.addUndo(item);
}

void View::setLockLayers(bool lock)
{
    emit lockLayers(lock);
}
