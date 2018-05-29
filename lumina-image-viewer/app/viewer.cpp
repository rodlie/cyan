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
    qDebug() << "setup ui";

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
    //connect(layersTree, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(handleLayerActivated(QTreeWidgetItem*,int)));
    connect(layersTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(handleLayerActivated(QTreeWidgetItem*,int)));
    connect(mdi, SIGNAL(subWindowActivated(QMdiSubWindow*)), layersTree, SLOT(handleTabActivated(QMdiSubWindow*)));

    layersComp = new QComboBox(this);
    populateCompBox();
    connect(layersComp, SIGNAL(currentIndexChanged(int)), this, SLOT(handleLayerCompChanged(int)));

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
    paths << QString("%1/.local/%1").arg(QDir::homePath()).arg(suffix);
    paths << QString("%1/../%2").arg(qApp->applicationDirPath()).arg(suffix);
    paths << QString("/usr/%1").arg(suffix);
    paths << QString("/usr/local/%1").arg(suffix);
    paths << QString("%1/plugins").arg(qApp->applicationDirPath());
    qDebug() << "plugin search path" << paths;

    for(int i=0;i<paths.size();++i) {
        QDir dir(paths.at(i));
        foreach (QString fileName, dir.entryList(QDir::Files)) {
            QPluginLoader loader(dir.absoluteFilePath(fileName));
            qDebug() << "Found possible plugin" << loader.fileName();
            QObject *plugin = loader.instance();
            if (!plugin) { continue; }
            qDebug() << "Seems like a plugin, try to add" << loader.fileName();
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
    //connect(view, SIGNAL())




    view->addLayer(image);

    image.flip(); //layer test
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

void Viewer::handleLayerCompChanged(int comp)
{
    qDebug() << "handle layer comp changed" << comp;
    QTreeWidgetItem *layer = layersTree->currentItem();
    QMdiSubWindow *tab = mdi->currentSubWindow();
    if (!layer || !tab) { return; }
    View *view = qobject_cast<View*>(tab->widget());
    if (!view) { return; }
    int layerID = layer->data(0,Qt::UserRole+1).toInt();
    layer->setData(0, Qt::UserRole+2, comp);
    view->setLayerComposite(layerID, magick2comp(comp));
}

/*
  using MagickCore::UndefinedCompositeOp;
  using MagickCore::NoCompositeOp;
  using MagickCore::BumpmapCompositeOp;
  using MagickCore::ChangeMaskCompositeOp;
  using MagickCore::ClearCompositeOp;
  using MagickCore::ColorizeCompositeOp;
  using MagickCore::CopyBlackCompositeOp;
  using MagickCore::CopyBlueCompositeOp;
  using MagickCore::CopyCompositeOp;
  using MagickCore::CopyCyanCompositeOp;
  using MagickCore::CopyGreenCompositeOp;
  using MagickCore::CopyMagentaCompositeOp;
  using MagickCore::CopyOpacityCompositeOp;
  using MagickCore::CopyRedCompositeOp;
  using MagickCore::CopyYellowCompositeOp;
*/

Magick::CompositeOperator Viewer::magick2comp(int comp)
{
    switch(comp) {
    case 2:
        return MagickCore::ModulusAddCompositeOp;
    case 3:
        return MagickCore::AtopCompositeOp;
    case 4:
        return MagickCore::BlendCompositeOp;
    case 5:
        return MagickCore::ColorBurnCompositeOp;
    case 6:
        return MagickCore::ColorDodgeCompositeOp;
    case 7:
        return MagickCore::DarkenCompositeOp;
    case 8:
        return MagickCore::DissolveCompositeOp;
    case 9:
        return MagickCore::HardLightCompositeOp;
    case 10:
        return MagickCore::HueCompositeOp;
    case 11:
        return MagickCore::LightenCompositeOp;
    case 12:
        return MagickCore::MinusDstCompositeOp;
    case 13:
        return MagickCore::MultiplyCompositeOp;
    case 14:
        return MagickCore::SubtractCompositeOp;
    case 15:
        return MagickCore::OverlayCompositeOp;
    case 16:
        return MagickCore::PlusCompositeOp;
    case 17:
        return MagickCore::SaturateCompositeOp;
    case 18:
        return MagickCore::ScreenCompositeOp;
    case 19:
        return MagickCore::AddCompositeOp;
    case 20:
        return MagickCore::MinusCompositeOp;
    case 21:
        return MagickCore::DivideCompositeOp;
    case 22:
        return MagickCore::LightenIntensityCompositeOp;
    case 23:
        return MagickCore::DarkenIntensityCompositeOp;
    case 24:
        return MagickCore::MinusSrcCompositeOp;
    case 25:
        return MagickCore::DivideSrcCompositeOp;
    case 26:
        return MagickCore::MathematicsCompositeOp;
    case 27:
        return MagickCore::LinearBurnCompositeOp;
    case 28:
        return MagickCore::LinearDodgeCompositeOp;
    case 29:
        return MagickCore::PinLightCompositeOp;
    case 30:
        return MagickCore::VividLightCompositeOp;
    case 31:
        return MagickCore::PegtopLightCompositeOp;
    case 32:
        return MagickCore::BlurCompositeOp;
    case 33:
        return MagickCore::DistortCompositeOp;
    case 34:
        return MagickCore::DivideDstCompositeOp;
    case 35:
        return MagickCore::XorCompositeOp;
    case 36:
        return MagickCore::ThresholdCompositeOp;
    case 37:
        return MagickCore::ModulusSubtractCompositeOp;
    case 38:
        return MagickCore::SrcOverCompositeOp;
    case 39:
        return MagickCore::SrcOutCompositeOp;
    case 40:
        return MagickCore::SrcInCompositeOp;
    case 41:
        return MagickCore::SrcCompositeOp;
    case 42:
        return MagickCore::SrcAtopCompositeOp;
    case 43:
        return MagickCore::SoftLightCompositeOp;
    case 44:
        return MagickCore::ReplaceCompositeOp;
    case 45:
        return MagickCore::OutCompositeOp;
    case 46:
        return MagickCore::ModulateCompositeOp;
    case 47:
        return MagickCore::LuminizeCompositeOp;
    case 48:
        return MagickCore::LinearLightCompositeOp;
    case 49:
        return MagickCore::InCompositeOp;
    case 50:
        return MagickCore::HardMixCompositeOp;
    case 51:
        return MagickCore::ExclusionCompositeOp;
    case 52:
        return MagickCore::DisplaceCompositeOp;
    case 53:
        return MagickCore::DifferenceCompositeOp;
    case 54:
        return MagickCore::DstOverCompositeOp;
    case 55:
        return MagickCore::DstOutCompositeOp;
    case 56:
        return MagickCore::DstInCompositeOp;
    case 57:
        return MagickCore::DstCompositeOp;
    case 58:
        return MagickCore::DstAtopCompositeOp;
    default:
        return MagickCore::OverCompositeOp;
    }
}

void Viewer::populateCompBox()
{
    layersComp->addItem(tr("Over"));
    layersComp->addItem(tr("Modulus Add"));
    layersComp->addItem(tr("Atop"));
    layersComp->addItem(tr("Blend"));
    layersComp->addItem(tr("Color Burn"));
    layersComp->addItem(tr("Color Dodge"));
    layersComp->addItem(tr("Darken"));
    layersComp->addItem(tr("Dissolve"));
    layersComp->addItem(tr("Hard Light"));
    layersComp->addItem(tr("Hue"));
    layersComp->addItem(tr("Lighten"));
    layersComp->addItem(tr("Minus Dst"));
    layersComp->addItem(tr("Multiply"));
    layersComp->addItem(tr("Subtract"));
    layersComp->addItem(tr("Overlay"));
    layersComp->addItem(tr("Plus"));
    layersComp->addItem(tr("Saturate"));
    layersComp->addItem(tr("Add"));
    layersComp->addItem(tr("Minus"));
    layersComp->addItem(tr("Divide"));
    layersComp->addItem(tr("Lighten Intensity"));
    layersComp->addItem(tr("Darken Intensity"));
    layersComp->addItem(tr("Minus Src"));
    layersComp->addItem(tr("Divide Src"));
    layersComp->addItem(tr("Mathematics"));
    layersComp->addItem(tr("Linear Burn"));
    layersComp->addItem(tr("Linear Dodge"));
    layersComp->addItem(tr("Pin Light"));
    layersComp->addItem(tr("Vivid Light"));
    layersComp->addItem(tr("Pegtop Light"));
    layersComp->addItem(tr("Blur"));
    layersComp->addItem(tr("Distort"));
    layersComp->addItem(tr("Divide Dst"));
    layersComp->addItem(tr("Xor"));
    layersComp->addItem(tr("Threshold"));
    layersComp->addItem(tr("Modulus Subtract"));
    layersComp->addItem(tr("Src Over"));
    layersComp->addItem(tr("Src Out"));
    layersComp->addItem(tr("Src In"));
    layersComp->addItem(tr("Src"));
    layersComp->addItem(tr("Src Atop"));
    layersComp->addItem(tr("Soft Light"));
    layersComp->addItem(tr("Replace"));
    layersComp->addItem(tr("Out"));
    layersComp->addItem(tr("Modulate"));
    layersComp->addItem(tr("Luminize"));
    layersComp->addItem(tr("Linear Light"));
    layersComp->addItem(tr("In"));
    layersComp->addItem(tr("Hard Mix"));
    layersComp->addItem(tr("Exclusion"));
    layersComp->addItem(tr("Displace"));
    layersComp->addItem(tr("Difference"));
    layersComp->addItem(tr("Dst Over"));
    layersComp->addItem(tr("Dst Out"));
    layersComp->addItem(tr("Dst In"));
    layersComp->addItem(tr("Dst"));
    layersComp->addItem(tr("Dst Atop"));
}

void Viewer::handleLayerActivated(QTreeWidgetItem *item, int col)
{
    qDebug() << "layer activated?";
    Q_UNUSED(col)
    if (!item) { return;}
    qDebug() << item->data(0, Qt::UserRole+2).toInt();
    layersComp->setCurrentIndex(item->data(0, Qt::UserRole+2).toInt());
}

void Viewer::handleLayerSelected(int layer)
{
    QTreeWidgetItem *item = layersTree->topLevelItem(layer);
    if (!item) { return; }
    layersTree->setCurrentItem(item);
}
