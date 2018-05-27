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
    , quitAct(0)
    , filterMenu(0)
    , imageBackend(0)
{
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
    view->setImage(image);
    view->setFit(true);
    tab->setWidget(view);
    tab->setAttribute(Qt::WA_DeleteOnClose);
    tab->show/*Maximized*/();
}
