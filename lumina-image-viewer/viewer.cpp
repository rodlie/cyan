#include "viewer.h"
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QMimeData>
#include <QFileInfo>
#include <QMatrix>
#include <QImage>
#include <QPixmap>
#include <QUrl>

ImageHandler::ImageHandler(QObject *parent) :
    QObject(parent)
{
    moveToThread(&t);
    t.start();
}

ImageHandler::~ImageHandler()
{
    t.quit();
    t.wait();
}

void ImageHandler::requestImage(QString filename)
{
    QMetaObject::invokeMethod(this,"readImage", Q_ARG(QString, filename));
}

void ImageHandler::readImage(QString filename)
{
    if (filename.isEmpty()) { return; }
    try {
        Magick::Image image;
        image.read(filename.toUtf8().data());
        if (image.rows()>0 && image.columns()>0) {
            emit returnImage(image);
        }
    }
    catch(Magick::Error &error_ ) {
        emit errorMessage(error_.what());
        return;
    }
    catch(Magick::Warning &warn_ ) {
        emit warningMessage(warn_.what());
    }
}

View::View(QWidget* parent) : QGraphicsView(parent)
, fit(false) {
    setAcceptDrops(true);
    setBackgroundBrush(Qt::darkGray);
    setDragMode(QGraphicsView::ScrollHandDrag);
}

void View::wheelEvent(QWheelEvent* event) {
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    double scaleFactor = 1.15;
    if (event->delta() > 0) { // up
        fit = false;
        scale(scaleFactor, scaleFactor);
        emit myZoom(scaleFactor, scaleFactor);
    } else { // down
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        emit myZoom(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}

void View::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        fit = false;
        emit resetZoom();
    } else {
        if (event->button() == Qt::RightButton) { setFit(true); }
        else { QGraphicsView::mousePressEvent(event); }
    }
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
    if (mimeData->hasUrls()) {
        if (!mimeData->urls().at(0).isEmpty()) {
            QUrl url = mimeData->urls().at(0);
            QString suffix = QFileInfo(url.toLocalFile()).suffix().toUpper();
            if (suffix == "PNG"
                || suffix == "JPG"
                || suffix == "JPEG"
                || suffix == "TIF"
                || suffix == "TIFF"
                || suffix == "PSD")
            {
                emit openImage(url.toLocalFile());
            } else if (suffix == "ICC" || suffix == "ICM") {
                emit openProfile(url.toLocalFile());
            }
        }
    }
}

void View::resizeEvent(QResizeEvent */*event*/)
{
    if (fit) {
        fitInView(0, 0, scene()->width(), scene()->height(), Qt::KeepAspectRatio);
    }
}

void View::doZoom(double scaleX, double scaleY)
{
    scale(scaleX,scaleY);
}

void View::setFit(bool value)
{
    if (!scene()) { return; }
    fit = value;
    fitInView(0, 0, scene()->width(), scene()->height(), Qt::KeepAspectRatio);
}

Viewer::Viewer(QWidget *parent)
    : QMainWindow(parent)
    , mainToolBar(0)
    , mainMenu(0)
    , mainStatusBar(0)
    , mainView(0)
    , mainScene(0)
    , openImageAct(0)
    , saveImageAct(0)
    , quitAct(0)
    , imageBackend(0)
{
    qRegisterMetaType<Magick::Image>("Magick::Image");
    imageBackend = new ImageHandler();
    connect(imageBackend, SIGNAL(returnImage(Magick::Image)), this, SLOT(handleImage(Magick::Image)));
    connect(imageBackend, SIGNAL(errorMessage(QString)), this, SLOT(handleError(QString)));
    connect(imageBackend, SIGNAL(warningMessage(QString)), this, SLOT(handleWarning(QString)));
    setupUI();
    loadSettings();
}

Viewer::~Viewer()
{
    saveSettings();
}

void Viewer::setupUI()
{
    qDebug() << "setup ui";

    mainToolBar = new QToolBar(this);
    mainToolBar->setObjectName(QString("mainToolBar"));
    addToolBar(mainToolBar);

    mainMenu = new QMenuBar(this);
    mainMenu->setObjectName(QString("mainMenu"));
    setMenuBar(mainMenu);

    mainStatusBar = new QStatusBar(this);
    mainStatusBar->setObjectName(QString("mainStatusBar"));
    setStatusBar(mainStatusBar);

    mainView = new View(this);
    mainView->fit = true;
    connect(mainView, SIGNAL(resetZoom()), this, SLOT(resetImageZoom()));
    connect(mainView, SIGNAL(openImage(QString)), this, SLOT(loadImage(QString)));
    setCentralWidget(mainView);

    mainScene = new QGraphicsScene(this);
    mainView->setScene(mainScene);

    openImageAct = new QAction(this);
    openImageAct->setText(tr("Open Image"));
    connect(openImageAct, SIGNAL(triggered(bool)), this, SLOT(loadImageDialog()));
    mainToolBar->addAction(openImageAct);

    quitAct = new QAction(this);
    quitAct->setText(tr("Quit"));
    connect(quitAct, SIGNAL(triggered(bool)), this, SLOT(close()));

    QMenu *fileMenu = new QMenu(this);
    fileMenu->setTitle(tr("File"));
    fileMenu->addAction(openImageAct);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAct);

    mainMenu->addMenu(fileMenu);
}

void Viewer::saveSettings()
{
    qDebug() << "save settings";
}

void Viewer::loadSettings()
{
    qDebug() << "load settings";
}

void Viewer::saveImage(QString filename)
{
    qDebug() << "save image" << filename;
}

void Viewer::loadImage(QString filename)
{
    if (filename.isEmpty()) { return; }
    imageBackend->requestImage(filename);
}

void Viewer::saveImageDialog()
{
    qDebug() << "save image dialog";
}

void Viewer::loadImageDialog()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Image"), QDir::homePath(), tr("Image Files (*.*)"));
    if (!filename.isEmpty()) { loadImage(filename); }
}

void Viewer::handleImage(Magick::Image image)
{
    if (image.columns()>0 && image.rows()>0) {
        clearImage();
        imageData = image;
        viewImage();
    }
}

void Viewer::handleError(QString message)
{
    qDebug() << "error" << message;
}

void Viewer::handleWarning(QString message)
{
    qDebug() << "warning" << message;
}

void Viewer::clearImage()
{
    imageData = Magick::Image();
    resetImageZoom();
}

void Viewer::resetImageZoom()
{
    QMatrix matrix;
    matrix.scale(1.0, 1.0);
    mainView->setMatrix(matrix);
}

void Viewer::viewImage()
{
    if (imageData.rows()==0 || imageData.columns()==0) { return; }
    Magick::Blob preview = makePreview();
    if (preview.length()==0) { return; }
    QPixmap pixmap(QPixmap::fromImage(QImage::fromData(QByteArray((char*)preview.data(), preview.length()))));
    if (pixmap.isNull()) { return; }
    mainScene->clear();
    mainScene->addPixmap(pixmap);
    mainScene->setSceneRect(0, 0, pixmap.width(), pixmap.height());
}

Magick::Blob Viewer::makePreview()
{
    try {
        Magick::Image preview = imageData;
        Magick::Blob result;
        if (preview.depth()>8) { preview.depth(8); }
        preview.strip();
        preview.magick("BMP");
        preview.write(&result);
        return result;
    }
    catch(Magick::Error &error_ ) {
        qDebug() << error_.what();
    }
    catch(Magick::Warning &warn_ ) {
        qDebug() << warn_.what();
    }
    return Magick::Blob();
}
