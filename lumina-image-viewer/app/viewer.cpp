/*
#
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#
*/

#include "interfaces.h"
#include "viewer.h"
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QImage>
#include <QPixmap>
#include <QPluginLoader>
#include <QApplication>
#include <QToolButton>
#include <QMdiSubWindow>
#include <QVBoxLayout>
#include "common.h"

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

Viewer::Viewer(QWidget *parent)
    : QMainWindow(parent)
    , mdi(0)
    , mainToolBar(0)
    , pluginsToolBar(0)
    , mainMenu(0)
    , mainStatusBar(0)
    , openImageAct(0)
    , saveImageAct(0)
    , newImageAct(0)
    , quitAct(0)
    , filterMenu(0)
    , imageBackend(0)
    , layersTree(0)
    , layersDock(0)
    , layersComp(0)
{
    setWindowTitle(QString("Lumina Pixel"));
    setWindowIcon(QIcon::fromTheme("applications-graphics"));

    qRegisterMetaType<Magick::Image>("Magick::Image");

    imageBackend = new ImageHandler();
    connect(imageBackend, SIGNAL(returnImage(Magick::Image)), this, SLOT(handleNewImage(Magick::Image)));
    connect(imageBackend, SIGNAL(errorMessage(QString)), this, SLOT(handleError(QString)));
    connect(imageBackend, SIGNAL(warningMessage(QString)), this, SLOT(handleWarning(QString)));

    setupUI();
    loadSettings();
    loadPlugins();
}

Viewer::~Viewer()
{
    saveSettings();
}

View *Viewer::getCurrentView()
{
    QMdiSubWindow *tab = mdi->currentSubWindow();
    if (!tab) { return NULL; }
    View *view = qobject_cast<View*>(tab->widget());
    if (!view) { return NULL; }
    return view;
}

void Viewer::setupUI()
{
    mdi = new QMdiArea(this);
    setCentralWidget(mdi);

    mainToolBar = new QToolBar(this);
    mainToolBar->setObjectName(QString("mainToolBar"));
    addToolBar(mainToolBar);

    pluginsToolBar = new QToolBar(this);
    pluginsToolBar->setObjectName(QString("pluginsToolBar"));
    addToolBar(/*Qt::LeftToolBarArea,*/ pluginsToolBar);

    mainMenu = new QMenuBar(this);
    mainMenu->setObjectName(QString("mainMenu"));
    setMenuBar(mainMenu);

    mainStatusBar = new QStatusBar(this);
    mainStatusBar->setObjectName(QString("mainStatusBar"));
    setStatusBar(mainStatusBar);

    newImageAct = new QAction(this);
    newImageAct->setText(tr("New Image"));
    newImageAct->setIcon(QIcon::fromTheme("document-new"));
    connect(newImageAct, SIGNAL(triggered(bool)), this, SLOT(newImage()));
    mainToolBar->addAction(newImageAct);

    openImageAct = new QAction(this);
    openImageAct->setText(tr("Open Image"));
    openImageAct->setIcon(QIcon::fromTheme("document-open"));
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

    filterMenu = new QMenu(this);
    filterMenu->setTitle(tr("Options"));
    filterMenu->setVisible(false);
    filterMenu->setEnabled(false);

    mainMenu->addMenu(fileMenu);
    mainMenu->addMenu(filterMenu);

    layersTree = new LayerTree(this);
    connect(layersTree, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(handleLayerActivated(QTreeWidgetItem*,int)));
    connect(layersTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(handleLayerActivated(QTreeWidgetItem*,int)));
    connect(layersTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(handleLayerActivated(QTreeWidgetItem*,QTreeWidgetItem*)));
    connect(mdi, SIGNAL(subWindowActivated(QMdiSubWindow*)), layersTree, SLOT(handleTabActivated(QMdiSubWindow*)));

    layersComp = new QComboBox(this);
    populateCompBox();
    connect(layersComp, SIGNAL(currentTextChanged(QString)), this, SLOT(handleLayerCompChanged(QString)));

    layersDock = new QDockWidget(this);
    layersDock->setWindowTitle(tr("Layers"));

    QWidget *layersContainer = new QWidget(this);
    QVBoxLayout *layersContainerLayout = new QVBoxLayout(layersContainer);

    layersContainerLayout->addWidget(layersComp);
    layersContainerLayout->addWidget(layersTree);
    layersDock->setWidget(layersContainer);

    addDockWidget(Qt::RightDockWidgetArea, layersDock);
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

void Viewer::handleNewImage(Magick::Image image)
{
    if (image.columns()>0 && image.rows()>0) { newTab(image); }
}

void Viewer::handleError(QString message)
{
    qDebug() << "error" << message;
}

void Viewer::handleWarning(QString message)
{
    qDebug() << "warning" << message;
}

void Viewer::addPlugin(QObject *plugin, QString filename)
{
    FilterInterface *filter = qobject_cast<FilterInterface *>(plugin);
    if (!filter) { return; }
    qDebug() << "Loaded filter" << filename << filter->filters();
    addToMenu(plugin, filter->filters(), filterMenu, SLOT(applyFilter()));
}

void Viewer::loadPlugins()
{
    QStringList paths;
    QString suffix = QString("share/%1/plugins").arg(qApp->applicationName());
    paths << QString("%1/.local/%2").arg(QDir::homePath()).arg(suffix);
    paths << QString("%1/../%2").arg(qApp->applicationDirPath()).arg(suffix);
    paths << QString("/usr/%1").arg(suffix);
    paths << QString("/usr/local/%1").arg(suffix);
    paths << QString("%1/plugins").arg(qApp->applicationDirPath());
    qDebug() << "plugin search path" << paths;

    for(int i=0;i<paths.size();++i) {
        QDir dir(paths.at(i));
        foreach (QString fileName, dir.entryList(QDir::Files)) {
            QPluginLoader loader(dir.absoluteFilePath(fileName));
            QObject *plugin = loader.instance();
            if (!plugin) { continue; }
            addPlugin(plugin, loader.fileName());
        }
    }

    filterMenu->setEnabled(!filterMenu->actions().isEmpty());
    filterMenu->setVisible(!filterMenu->actions().isEmpty());
}

void Viewer::applyFilter()
{
    // TODO: fix layer support
    QAction *action = qobject_cast<QAction *>(sender());
    FilterInterface *filter =qobject_cast<FilterInterface *>(action->parent());
    if (!filter || action->data().toString().isEmpty()) { return; }
    if (!getCurrentView()) { return; }
    getCurrentView()->setImage(filter->filterImage(action->data().toString(), getCurrentView()->getImage()));
}

void Viewer::addToMenu(QObject *plugin, const QStringList &texts, QMenu *menu, const char *member, QActionGroup *actionGroup)
{
    foreach (QString text, texts) {
        QString filterCategory = text.split("/").takeFirst();
        QString filterName = text.split("/").takeLast();
        QAction *action = new QAction(filterName, plugin);
        action->setIcon(QIcon::fromTheme("applications-other"));
        action->setData(text);
        connect(action, SIGNAL(triggered()), this, member);
        bool hasCategory = false;
        foreach (QAction *menuAction, menu->actions()) {
            if (!menuAction->menu()) { continue; }
            if (menuAction->menu()->title() == filterCategory) {
                hasCategory = true;
                menuAction->menu()->addAction(action);
                break;
            }
        }
        if(!hasCategory && !filterCategory.isEmpty()) {
            QMenu *catMenu = new QMenu(this);
            catMenu->setTitle(filterCategory);
            catMenu->setIcon(QIcon::fromTheme("applications-other"));
            catMenu->addAction(action);
            menu->addMenu(catMenu);
            QToolButton *catButton = new QToolButton(this);
            catButton->setText(filterCategory);
            catButton->setToolTip(filterCategory);
            catButton->setIcon(QIcon::fromTheme("applications-other"));
            catButton->setMenu(catMenu);
            catButton->setPopupMode(QToolButton::InstantPopup);
            pluginsToolBar->addWidget(catButton);
        } else if (filterCategory.isEmpty()) {
            menu->addAction(action);
            pluginsToolBar->addAction(action);
        }
        if (actionGroup) {
            action->setCheckable(true);
            actionGroup->addAction(action);
        }
    }
}

void Viewer::newTab(Magick::Image image)
{
    QMdiSubWindow *tab = new QMdiSubWindow(mdi);
    View *view = new View(this);
    connect(view, SIGNAL(selectedLayer(int)), this, SLOT(handleLayerSelected(int)));

    view->addLayer(image);
    view->addLayer(image); // layer test
    view->setFit(true);

    tab->setWidget(view);
    tab->setWindowTitle(QString::fromStdString(image.fileName()));
    tab->setAttribute(Qt::WA_DeleteOnClose);
    tab->showMaximized();
}

void Viewer::newImage()
{
    qDebug() << "new image";

    // tmp just to test ...
    QMdiSubWindow *tab = new QMdiSubWindow(mdi);
    View *view = new View(this);
    view->setFit(true);
    tab->setWidget(view);
    tab->setWindowTitle(tr("New Image"));
    tab->setAttribute(Qt::WA_DeleteOnClose);
    tab->showMaximized();

}

void Viewer::handleLayerCompChanged(QString comp)
{
    if (comp.isEmpty()) { return; }
    LayerTreeItem *layer = dynamic_cast<LayerTreeItem*>(layersTree->currentItem());
    QMdiSubWindow *tab = mdi->currentSubWindow();
    if (!layer || !tab) { return; }
    View *view = qobject_cast<View*>(tab->widget());
    if (!view) { return; }
    QMapIterator<Magick::CompositeOperator, QString> i(Common::compositeModes());
    while (i.hasNext()) {
        i.next();
        if (i.value() == comp) {
            layer->setComposite(i.key());
            view->setLayerComposite(layer->getLayerID(), i.key());
            break;
        }
    }
}

void Viewer::populateCompBox()
{
    layersComp->clear();
    QMapIterator<Magick::CompositeOperator, QString> i(Common::compositeModes());
    while (i.hasNext()) {
        i.next();
        layersComp->addItem(i.value());
    }
    layersComp->setCurrentText(tr("Over"));
}

void Viewer::handleLayerActivated(QTreeWidgetItem *item, int col)
{
    Q_UNUSED(col)
    handleLayerActivated(item, item);
}

void Viewer::handleLayerActivated(QTreeWidgetItem *item, QTreeWidgetItem *old)
{
    Q_UNUSED(old)
    LayerTreeItem *layer = dynamic_cast<LayerTreeItem*>(item);
    if (!layer) { return; }
    layersComp->setCurrentText(Common::compositeModes()[layer->getComposite()]);
}

void Viewer::handleLayerSelected(int layer)
{
    for (int i=0;i<layersTree->topLevelItemCount();++i) {
        LayerTreeItem *item = dynamic_cast<LayerTreeItem*>(layersTree->topLevelItem(i));
        if (!item) { continue; }
        if (layer == item->getLayerID()) {
            layersTree->setCurrentItem(item);
            handleLayerActivated(item, 0);
            return;
        }
    }
}
