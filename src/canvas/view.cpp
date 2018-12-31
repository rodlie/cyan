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

#include "view.h"

#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include <QDebug>
#include <QPixmap>
#include <QImage>
#include <QTransform>
#include <QtConcurrent/QtConcurrent>
#include <QTimer>

#include "common.h"

View::View(QWidget* parent, bool setup) :
    QGraphicsView(parent)
  , fit(false)
  , _scene(nullptr)
  , _rect(nullptr)
  , _brush(nullptr)
  , _drawing(false)
  , _moving(false)
  , _selectedLayer(0)
  , _supportsLayers(true)
{
    // setup the basics
    setAcceptDrops(true);
    setBackgroundBrush(QColor(30,30,30));
    setMouseTracking(true);


    // TODO
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
    connect(this, SIGNAL(resetZoom()),
            this, SLOT(resetImageZoom()));

    // setup canvas
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
            scale(scaleFactor, scaleFactor);
            emit myZoom(scaleFactor, scaleFactor);
        }
    } else { // down
        if (_drawing && (event->buttons() & Qt::RightButton)) {
            // if drawing and mouse wheel down + right mouse button = smaller brush
            int value =  qRound(_brush->rect().width()-1);
            if (value<0) { value = 1; }
            setBrushStroke(value);
        } else { // zoom out
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
            emit myZoom(1.0 / scaleFactor, 1.0 / scaleFactor);
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
        moveSelectedLayer(Common::MoveLayerLeft, skip);
        return;
    case Qt::Key_Right:
        moveSelectedLayer(Common::MoveLayerRight, skip);
        return;
    case Qt::Key_Up:
        moveSelectedLayer(Common::MoveLayerUp, skip);
        return;
    case Qt::Key_Down:
        moveSelectedLayer(Common::MoveLayerDown, skip);
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
}

void View::setFit(bool value)
{
    if (!scene()) { return; }
    fit = value;
    fitInView(0,
              0,
              scene()->width(),
              scene()->height(),
              Qt::KeepAspectRatio);
}

void View::resetImageZoom()
{
    fit = false;
    QMatrix matrix;
    matrix.scale(1.0, 1.0);
    setMatrix(matrix);
}

void View::setLayer(Magick::Image image,
                    int id)
{
    _canvas.layers[id].image = image;
    refreshTiles();
}

void View::addLayer(Magick::Image image,
                    bool updateView)
{
    qDebug() << "ADD LAYER" << QString::fromStdString(image.label());
    int id = _canvas.layers.size();
    _canvas.layers[id].image = image;
    //_canvas.layers[id].visible = true;
    if (!image.label().empty()) {
        _canvas.layers[id].label = QString::fromStdString(image.label());
    }

    LayerItem *layer = new LayerItem();
    layer->setRect(0,
                   0,
                   image.columns(),
                   image.rows());
    layer->setData(1,
                   id);

    connect(layer, SIGNAL(movingItem(QPointF,int)),
            this, SLOT(handleLayerMoving(QPointF,int)));
    connect(layer, SIGNAL(movedItem(QPointF,int)),
            this, SLOT(handleLayerMoved(QPointF,int)));
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
    layer->setMovable(true);
    layer->setZValue(LAYER_Z);

    emit addedLayer(id);
    emit updatedLayers();

    if (updateView) { /*handleLayerOverTiles(layer);*/ refreshTiles(); }
}

void View::addLayer(int id,
                    QSize geo,
                    QSize pos,
                    bool updateView)
{
    qDebug() << "ADD LAYER" << id;
   // _canvas.layers[id].visible = true;
    LayerItem *layer = new LayerItem();
    layer->setRect(0,
                   0,
                   geo.width(),
                   geo.height());
    layer->setData(1,
                   id);

    connect(layer, SIGNAL(movingItem(QPointF,int)),
            this, SLOT(handleLayerMoving(QPointF,int)));
    connect(layer, SIGNAL(movedItem(QPointF,int)),
            this, SLOT(handleLayerMoved(QPointF,int)));
    connect(layer, SIGNAL(selectedItem(int)),
            this, SLOT(handleLayerSelected(int)));
    connect(this, SIGNAL(viewClosed()),
            layer, SLOT(deleteLater()));
    connect(this, SIGNAL(lockLayer(LayerItem*,bool)),
            layer, SLOT(setMovable(LayerItem*,bool)));
    connect(this, SIGNAL(lockLayers(bool)),
            layer, SLOT(setMovable(bool)));
    connect(this, SIGNAL(isDrag(bool)),
            layer, SLOT(setDrag(bool)));
    connect(this, SIGNAL(setDraw(bool)),
            layer, SLOT(setDraw(bool)));

    _scene->addItem(layer);
    layer->setMovable(true);
    layer->setZValue(LAYER_Z);

    layer->setPos(pos.width(), pos.height());

    emit addedLayer(id);
    emit updatedLayers();

    if (updateView) { /*handleLayerOverTiles(layer);*/ refreshTiles(); }
}

void View::clearLayers()
{
    _canvas.layers.clear();
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

Common::Canvas View::getCanvasProject()
{
    return _canvas;
}

void View::setLayerVisibility(int layer,
                              bool layerIsVisible)
{
    if (_canvas.layers[layer].visible != layerIsVisible) {
        _canvas.layers[layer].visible = layerIsVisible;
        handleLayerOverTiles(layer);
    }
}

bool View::getLayerVisibility(int layer)
{
    return _canvas.layers[layer].visible;
}

void View::setLayerComposite(int layer,
                             Magick::CompositeOperator composite)
{
    if (_canvas.layers[layer].composite != composite) {
        _canvas.layers[layer].composite = composite;
        handleLayerOverTiles(layer);
    }
}

Magick::CompositeOperator View::getLayerComposite(int layer)
{
    return _canvas.layers[layer].composite;
}

int View::getLayerCount()
{
    return  _canvas.layers.size();
}

Common::Layer View::getLayer(int layer)
{
    return _canvas.layers[layer];
}

void View::setLayer(int layer, Magick::Image image)
{
    _canvas.layers[layer].image = image;
    emit updatedLayers();
}

void View::setLayerFromCanvas(Common::Canvas canvas,
                              int layer)
{
    _canvas.layers[layer].image = canvas.image;
    _canvas.layers[layer].layers = canvas.layers;
    emit updatedLayers();
}

void View::setLayersFromCanvas(Common::Canvas canvas)
{
    if (_canvas.layers.size()>0) {
        qWarning() << "DONT SET LAYERS IF EXISTS!";
        return;
    }
    _canvas.layers = canvas.layers;
    QMapIterator<int, Common::Layer> layers(_canvas.layers);
    while (layers.hasNext()) {
        layers.next();
        addLayer(layers.key(),
                 QSize(static_cast<int>(layers.value()
                                        .image.columns()),
                       static_cast<int>(layers.value()
                                        .image.rows())),
                 layers.value().pos,
                 false);
        handleLayerOverTiles(layers.key());
    }
    emit updatedLayers();
    refreshTiles();
}

void View::updateCanvas(Common::Canvas canvas)
{
    _image = canvas.image;
    _canvas = canvas;
    refreshTiles();
}

QSize View::getLayerOffset(int layer)
{
    return _canvas.layers[layer].pos;
}

void View::setLayerOffset(int layer,
                          QSize offset)
{
    _canvas.layers[layer].pos = offset;
}

QString View::getLayerName(int layer)
{
    return  _canvas.layers[layer].label;
}

void View::setLayerName(int layer,
                        QString name)
{
    _canvas.layers[layer].label = name;
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
}

void View::removeLayer(int layer)
{
    if (layer<0) { return; }
    QList<QGraphicsItem*> items = _scene->items();
    for (int i=0;i<items.size();++i) {
        LayerItem *item = dynamic_cast<LayerItem*>(items.at(i));
        if (!item) { continue; }
        if (item->getID()==layer) {
            // remove layer graphics item
            _scene->removeItem(item);
            item->deleteLater();
            break;
        }
    }
    _canvas.layers.remove(layer);
    emit updatedLayers();
    emit statusMessage(tr("Removed layer %1 from canvas")
                       .arg(layer));
}

void View::setupCanvas(int width,
                       int height,
                       int depth,
                       Magick::ColorspaceType colorspace)
{
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
    _scene->setSceneRect(0,
                         0,
                         width,
                         height);

    // set background rectangle size
    _rect->setRect(0,
                   0,
                   width,
                   height);

    // copy canvas
    _canvas.image = _image;

    // set timestamp
    _canvas.timestamp = Common::timestamp();

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
    return _canvas.timestamp;
}

void View::refreshTiles()
{
    qDebug() << "REFRESH TILES";
    QMapIterator<int, Common::Tile> tiles(_canvas.tiles);
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

// TODO
void View::setCanvasSpecsFromImage(Magick::Image image)
{
    qDebug() << "set canvas specs from image";
    _image.label(image.label());
    _image.size(Magick::Geometry(image.columns(), image.rows()));
    _image.depth(image.depth());
    _image.colorSpace(image.colorSpace());

    _image.alpha(true);
    _image.evaluate(Magick::AlphaChannel,
                    Magick::MultiplyEvaluateOperator,
                    0.0);

    _scene->setSceneRect(0, 0, image.columns(), image.rows());
    _rect->setRect(0, 0, image.columns(), image.rows());
    _canvas.image = _image;

    // save color profile
    _canvas.profile = image.iccColorProfile();
    if (_canvas.profile.length()==0) {
        emit errorMessage(tr("Missing color profile!"));
    }

    // setup canvas tiles
    initTiles();

    paintCanvasBackground();

    emit updatedLayers();
}

void View::setDrawMode(bool draw)
{
    emit setDraw(draw);
    _drawing = draw;
    _brush->setVisible(draw);
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
    _canvas.brushColor = color;
}

void View::handleLayerMoving(QPointF pos, int id, bool forceRender)
{
    if (!_canvas.layers.contains(id) || id<0) { return; }

    _canvas.layers[id].pos = QSize(static_cast<int>(pos.x()),
                                   static_cast<int>(pos.y()));

    LayerItem *layerItem = dynamic_cast<LayerItem*>(sender());
    if (!layerItem) { return; }

    handleLayerOverTiles(layerItem, forceRender);
}

void View::handleLayerMoved(QPointF pos,
                            int id)
{
    handleLayerMoving(pos,
                      id,
                      true);
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

    // setup tiles
    _canvas.tiles = setupTiles(_canvas.image, tiles);
}

void View::clearScene()
{
    _scene->clear();
}

void View::clearTiles()
{
    QMapIterator<int, Common::Tile> i(_canvas.tiles);
    while(i.hasNext()) {
        i.next();
        _scene->removeItem(i.value().rect);
        i.value().rect->deleteLater();
    }
    _canvas.tiles.clear();
}

QMap<int, Common::Tile> View::setupTiles(Magick::Image image,
                                         int tiles)
{
    QMap<int, Common::Tile> result;

    // tile size
    int width = static_cast<int>(image.columns())/tiles;
    int height = static_cast<int>(image.rows())/tiles;

    int row = 0;
    int rowItems = 0;
    for (int i=0;i<(tiles*tiles);++i) {
        result[i].rect = new TileItem(nullptr,
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
        if (rowItems == tiles-1) {
            row++;
            rowItems=0;
        } else { rowItems++; }
    }
    return  result;
}

void View::handleLayerOverTiles(LayerItem *layerItem,
                                bool ignoreRunning)
{
    //if (future.isRunning() || !layerItem) { return; }
    if (!ignoreRunning) {
        if (future.isRunning()) { return; }
    }
    if (!layerItem) { return; }

    QList<QGraphicsItem*> items = _scene->collidingItems(layerItem);
    for (int i=0;i<items.size();++i) {
        TileItem *item = dynamic_cast<TileItem*>(items.at(i));
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
    QTimer::singleShot(10, this, SLOT(handleTileStatus()));
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

    QMapIterator<int, Common::Tile> tiles(_canvas.tiles);
    while (tiles.hasNext()) {
        tiles.next();
        int tile = tiles.key();

        QVector<int> colliding;
        QList<QGraphicsItem*> items = _scene->collidingItems(_canvas.tiles[tile].rect);
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

void View::handleBrushOverTile(QPointF pos,
                               bool draw)
{
    QList<QGraphicsItem*> items = _scene->collidingItems(_brush);
    bool foundLayer = false;
    for (int i=0;i<items.size();++i) {

        LayerItem *layerItem = dynamic_cast<LayerItem*>(items.at(i));
        if (layerItem && draw && !foundLayer) { // draw
            foundLayer = true;
            QPointF epos;
            int id = layerItem->getID();
            epos.setX(pos.x()-_canvas.layers[id].pos.width());
            epos.setY(pos.y()-_canvas.layers[id].pos.height());

            _canvas.layers[id].image.strokeAntiAlias(_canvas.brushAA);
            _canvas.layers[id].image.strokeLineCap(_canvas.brushLineCap);
            _canvas.layers[id].image.strokeLineJoin(_canvas.brushLineJoin);
            _canvas.layers[id].image.strokeWidth(_brush->rect().width());

            if (_canvas.layers[id].image.colorSpace() == Magick::CMYKColorspace) {
                // TODO! : need im workaround for this to work
                _canvas.layers[id].image.strokeColor(Magick::ColorRGB(_canvas.brushColor.cyanF(),
                                                                      _canvas.brushColor.magentaF(),
                                                                      _canvas.brushColor.yellowF()));
            } else {
                _canvas.layers[id].image.strokeColor(Magick::ColorRGB(_canvas.brushColor.redF(),
                                                                      _canvas.brushColor.greenF(),
                                                                      _canvas.brushColor.blueF()));
            }
            _canvas.layers[id].image.draw(Magick::DrawableLine(epos.x()-1,
                                                               epos.y()-1,
                                                               epos.x(),
                                                               epos.y()));
        }

        TileItem *item = dynamic_cast<TileItem*>(items.at(i));
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

void View::renderTile(int tile, Magick::Image canvas, QMap<int, Common::Layer> layers, Magick::Geometry crop)
{
    canvas.quiet(true);
    if (crop.width()==0 || tile==-1 || layers.size()==0 || canvas.columns()==0) { return; }

    // comp tile and write pixmap
    Magick::Image tmp = Common::compLayers(canvas, layers, crop);
    tmp.quiet(true);
    tmp.magick("BMP");
    Magick::Blob preview;
    tmp.write(&preview);

    // update tile pixmap
    if (preview.length()>0) {
        QPixmap pix = QPixmap::fromImage(QImage::fromData(reinterpret_cast<uchar*>(const_cast<void*>(preview.data())),
                                                          static_cast<int>(preview.length())));
        if (pix.isNull()) { return; }
        emit updateTilePixmap(tile, pix);
    }
}

void View::paintCanvasBackground()
{ // paint a checkerboard background to show transparency in image
    try {
        Magick::Image cb(_canvas.image);
        cb.quiet(true);
        cb.alpha(true);
        cb.crop(Magick::Geometry(512, 512));
        cb.extent(Magick::Geometry(512, 512));
        cb.read("pattern:checkerboard");
        cb.magick("BMP");
        cb.colorSpace(Magick::sRGBColorspace);
        cb.depth(8);
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

void View::moveSelectedLayer(Common::MoveLayer gravity, int skip)
{
    qDebug() << "move selected layer" << _selectedLayer << gravity;
    for (int i=0;i<_scene->items().size();++i) {
        LayerItem *item = dynamic_cast<LayerItem*>(_scene->items().at(i));
        if (!item) { continue; }

        QPen newPen(Qt::transparent);
        newPen.setWidth(0);
        item->setPen(newPen);

        if (item->getID() != _selectedLayer) { continue; }
        QPointF pos = item->pos();
        switch (gravity) {
        case Common::MoveLayerUp:
            pos.setY(pos.y()-skip);
            break;
        case Common::MoveLayerDown:
            pos.setY(pos.y()+skip);
            break;
        case Common::MoveLayerLeft:
            pos.setX(pos.x()-skip);
            break;
        case Common::MoveLayerRight:
            pos.setX(pos.x()+skip);
            break;
        }
        item->setPos(pos);
        handleLayerMoved(pos, _selectedLayer);
        handleLayerOverTiles(item);
    }

}

void View::setLockLayers(bool lock)
{
    emit lockLayers(lock);
}
