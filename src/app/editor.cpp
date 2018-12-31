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

#include "editor.h"

#include <QFileDialog>
#include <QDir>
#include <QImage>
#include <QPixmap>
#include <QPluginLoader>
#include <QApplication>
#include <QToolButton>
#include <QMdiSubWindow>
#include <QVBoxLayout>
#include <QTimer>

#include <QMessageBox>
#include <QImage>
#include <QSettings>
#include <QHeaderView>
#include <QKeySequence>
#include <QDebug>
#include <QMimeDatabase>
#include <QMimeType>


#include "newmediadialog.h"
#include "convertdialog.h"
#include "colorconvert.h"
#include "render.h"

#ifdef WITH_FFMPEG
#include "videodialog.h"
#endif

#ifdef Q_OS_MAC
#define CYAN_FONT_SIZE 10
#else
#define CYAN_FONT_SIZE 8
#endif

Editor::Editor(QWidget *parent)
    : QMainWindow(parent)
    , mdi(nullptr)
    , mainToolBar(nullptr)
    , mainMenu(nullptr)
    , mainStatusBar(nullptr)
    , newImageAct(nullptr)
    , openImageAct(nullptr)
    , saveImageAct(nullptr)
    , saveProjectAct(nullptr)
    , saveProjectAsAct(nullptr)
    , newLayerAct(nullptr)
    , openLayerAct(nullptr)
    , saveLayerAct(nullptr)
    , quitAct(nullptr)
    , viewMoveAct(nullptr)
    , viewDrawAct(nullptr)
    , aboutImageMagickAct(nullptr)
    , aboutLcmsAct(nullptr)
    , aboutQtAct(nullptr)
    , convertRGBAct(nullptr)
    , convertCMYKAct(nullptr)
    , convertGRAYAct(nullptr)
    , convertAssignAct(nullptr)
    , convertExtractAct(nullptr)
    , fileMenu(nullptr)
    , optMenu(nullptr)
    , helpMenu(nullptr)
    , newMenu(nullptr)
    , saveMenu(nullptr)
    , colorMenu(nullptr)
    , colorProfileRGBMenu(nullptr)
    , colorProfileCMYKMenu(nullptr)
    , colorProfileGRAYMenu(nullptr)
    , colorIntentMenu(nullptr)
    , newButton(nullptr)
    , saveButton(nullptr)
    , layersTree(nullptr)
    , layersDock(nullptr)
    , layersComp(nullptr)
    , layersOpacity(nullptr)
    , brushSize(nullptr)
    , brushDock(nullptr)
    , colorTriangle(nullptr)
    , colorPicker(nullptr)
    , newLayerButton(nullptr)
    , removeLayerButton(nullptr)
    , moveLayerUpButton(nullptr)
    , moveLayerDownButton(nullptr)
    , mergeLayerDownButton(nullptr)
{
    setWindowTitle(qApp->applicationName());
    setAttribute(Qt::WA_QuitOnClose);

    // register Magick types used
    qRegisterMetaType<Magick::Image>("Magick::Image");
    qRegisterMetaType<Magick::Drawable>("Magick::Drawable");
    qRegisterMetaType<Magick::Geometry>("Magick::Geometry");

    setupUI();
    loadSettings();

#ifdef QT_DEBUG
    qDebug() << "color profile path" << Common::getColorProfilesPath();
    qDebug() << "rgb color profiles" << Common::getColorProfiles(Magick::sRGBColorspace);
    qDebug() << "cmyk color profiles" << Common::getColorProfiles(Magick::CMYKColorspace);
    qDebug() << "gray color profiles" << Common::getColorProfiles(Magick::GRAYColorspace);
    qDebug() << "q" << Common::supportedQuantumDepth();
    qDebug() << "jpeg" << Common::supportsJpeg();
    qDebug() << "png" << Common::supportsPng();
    qDebug() << "tiff" << Common::supportsTiff();
    qDebug() << "lcms" << Common::supportsLcms();
    qDebug() << "hdri" << Common::supportsHdri();
    qDebug() << "openmp" << Common::supportsOpenMP();
    qDebug() << "bzlib" << Common::supportsBzlib();
    qDebug() << "cairo" << Common::supportsCairo();
    qDebug() << "fontconfig" << Common::supportsFontConfig();
    qDebug() << "freetype" << Common::supportsFreeType();
    qDebug() << "jp2" << Common::supportsJP2();
    qDebug() << "lzma" << Common::supportsLzma();
    qDebug() << "openexr" << Common::supportsOpenExr();
    qDebug() << "pangocairo" << Common::supportsPangoCairo();
    qDebug() << "raw" << Common::supportsRaw();
    qDebug() << "rsvg" << Common::supportsRsvg();
    qDebug() << "webp" << Common::supportsWebp();
    qDebug() << "xml" << Common::supportsXml();
    qDebug() << "zlib" << Common::supportsZlib();
#endif
}

Editor::~Editor()
{
    saveSettings();
}

View *Editor::getCurrentView()
{
    QMdiSubWindow *tab = mdi->currentSubWindow();
    if (!tab) { return nullptr; }
    View *view = qobject_cast<View*>(tab->widget());
    if (!view) { return nullptr; }
    return view;
}





void Editor::saveSettings()
{
    emit statusMessage(tr("Saving settings ..."));
    QSettings settings;

    settings.beginGroup("engine");
    settings.setValue("disk_limit",
                      Common::getDiskResource());
    settings.setValue("memory_limit",
                      Common::getMemoryResource());
    settings.endGroup();

    settings.beginGroup("gui");
    settings.setValue("editor_state",
                      saveState());
    settings.setValue("editor_geometry",
                      saveGeometry());
    settings.setValue("editor_maximized",
                      isMaximized());
    /*settings.setValue("infotree_header_state",
                      imageInfoTree->header()->saveState());
    settings.setValue("infotree_header_geometry",
                      imageInfoTree->header()->saveGeometry());*/
    settings.endGroup();

    settings.beginGroup(QString("color"));
    settings.setValue(QString("rgb_profile"),
                      selectedDefaultColorProfile(colorProfileRGBMenu));
    settings.setValue(QString("cmyk_profile"),
                      selectedDefaultColorProfile(colorProfileCMYKMenu));
    settings.setValue(QString("gray_profile"),
                      selectedDefaultColorProfile(colorProfileGRAYMenu));
    settings.setValue(QString("blackpoint"),
                      blackPointAct->isChecked());
    settings.endGroup();
}

void Editor::loadSettings()
{
    emit statusMessage(tr("Loading settings ..."));
    QSettings settings;

    settings.beginGroup("engine");
    Common::setDiskResource(settings
                            .value("disk_limit", 0).toInt());
    Common::setMemoryResource(settings
                              .value("memory_limit", 8).toInt());
    settings.endGroup();

    settings.beginGroup("gui");
    if (settings.value("editor_state").isValid()) {
        restoreState(settings
                     .value("editor_state").toByteArray());
    }
    if (settings.value("editor_geometry").isValid()) {
        restoreGeometry(settings
                        .value("editor_geometry").toByteArray());
    }
    /*if (settings.value("infotree_header_state").isValid()) {
        imageInfoTree->header()->restoreState(settings
                                              .value("infotree_header_state").toByteArray());
    }
    if (settings.value("infotree_header_geometry").isValid()) {
        imageInfoTree->header()->restoreGeometry(settings
                                                 .value("infotree_header_geometry").toByteArray());
    }*/
    if (settings.value("editor_maximized").toBool()) { showMaximized(); }
    settings.endGroup();

    emit statusMessage(tr("Engine disk cache limit: %1 GB")
                       .arg(Common::getDiskResource()));
    emit statusMessage(tr("Engine memory limit: %1 GB")
                       .arg(Common::getMemoryResource()));

    setDefaultColorProfiles(colorProfileRGBMenu);
    setDefaultColorProfiles(colorProfileCMYKMenu);
    setDefaultColorProfiles(colorProfileGRAYMenu);
    loadDefaultColorIntent();
    settings.beginGroup(QString("color"));
    blackPointAct->setChecked(settings.value(QString("blackpoint"),
                                             true)
                              .toBool());
    settings.endGroup();

    // quit if no color profiles are available
    hasColorProfiles();
}

void Editor::loadProject(const QString &filename)
{
    if (filename.isEmpty()) { return; }
    QFileInfo fileInfo(filename);
    if (fileInfo.suffix().toLower() != "miff") { return; }
    if (Common::isValidCanvas(filename)) {
        Common::Canvas canvas = Common::readCanvas(filename);
        newTab(canvas);
    }
}

void Editor::saveProject(const QString &filename)
{
    if (filename.isEmpty() || !getCurrentView()) { return; }
    qDebug() << "save project" << filename;

    if (Common::writeCanvas(getCurrentView()->getCanvasProject(), filename)) {
        /*Common::Canvas canvas = Common::readCanvas(filename);
        qDebug() << "====> canvas" << canvas.label << canvas.layers.size();
        newTab(canvas);
        //view->loadCanvas(canvas);
        mdi->tileSubWindows();*/
    } else {
        QMessageBox::warning(this,
                             tr("Cyan Error"),
                             tr("Failed to save the project!"));
    }
}

void Editor::saveImage(const QString &filename)
{
    qDebug() << "save image" << filename;
}

void Editor::loadImage(const QString &filename)
{
    if (filename.isEmpty()) { return; }
    if (Common::isValidCanvas(filename)) {
        emit statusMessage(tr("Loading canvas %1").arg(filename));
        loadProject(filename);
        emit statusMessage(tr("Done"));
    } else {
        qDebug() << "HAS LAYERS?" << Common::hasLayers(filename);
        emit statusMessage(tr("Loading image %1").arg(filename));
        readImage(filename);
        emit statusMessage(tr("Done"));
    }
}

void Editor::readImage(Magick::Blob blob,
                       const QString &filename)
{
    Magick::Image image;
    image.quiet(false);

    try {
        if (blob.length()>0) {
            image.read(blob);
        } else if (!filename.isEmpty()) {
            image.read(filename.toStdString());
        }
    }
    catch(Magick::Error &error_ ) {
        emit errorMessage(error_.what());
        return;
    }
    catch(Magick::Warning &warn_ ) { emit warningMessage(warn_.what()); }

    try {
        image.magick("MIFF");
        image.fileName(filename.toStdString());
        if (image.label().empty()) {
            QFileInfo fileInfo(filename);
            image.label(fileInfo.baseName().toStdString());
        }

        if (image.iccColorProfile().length()==0) {
            QString defPro;
            switch(image.colorSpace()) {
            case Magick::CMYKColorspace:
                defPro = selectedDefaultColorProfile(colorProfileCMYKMenu);
                break;
            case Magick::GRAYColorspace:
                defPro = selectedDefaultColorProfile(colorProfileGRAYMenu);
                break;
            default:
                defPro = selectedDefaultColorProfile(colorProfileRGBMenu);
            }
            ConvertDialog *dialog = new ConvertDialog(this,
                                                      tr("Assign Color Profile"),
                                                      defPro,
                                                      image.colorSpace());
            int ret = dialog->exec();
            if (ret == QDialog::Accepted &&
                !dialog->getProfile().isEmpty())
            {
                Magick::Blob profile;
                Magick::Image input;
                input.read(dialog->getProfile().toStdString());
                input.write(&profile);
                image = ColorConvert::convertColorspace(image,
                                                  Magick::Blob(),
                                                  profile);
                if (image.columns()>0 &&
                    image.rows()>0 &&
                    image.iccColorProfile().length()>0)
                {
                    newTab(image);
                }
            }
            QTimer::singleShot(100,
                               dialog,
                               SLOT(deleteLater()));
        } else {
            if (image.columns()>0 &&
                image.rows()>0)
            {
                newTab(image);
            }
        }
    }
    catch(Magick::Error &error_ ) { emit errorMessage(error_.what()); }
    catch(Magick::Warning &warn_ ) { emit warningMessage(warn_.what()); }
}

void Editor::readImage(const QString &filename)
{
    if (filename.isEmpty()) { return; }
    readImage(Magick::Blob(), filename);
}

void Editor::writeImage(const QString &filename)
{
    if (filename.isEmpty() || !getCurrentView()) { return; }

    Magick::Image image = Render::renderCanvasToImage(getCurrentView()->getCanvasProject());
    // TODO: add options for file format
    try {
        QFileInfo info(filename);
        if (info.suffix().toLower() == "tiff" ||
            info.suffix().toLower() == "tif")
        {
            image.magick("TIFF");
        }
        else if (info.suffix().toLower() == "jpg" ||
                   info.suffix().toLower() == "jpeg")
        {
            image.magick("JPG");
        }
        else if (info.suffix().toLower() == "png")
        {
            image.magick("PNG");
        }
        else if (info.suffix().toLower() == "bmp")
        {
            image.magick("BMP");
        }
        image.write(filename.toStdString());
    }
    catch(Magick::Error &error_ ) { emit errorMessage(error_.what()); }
    catch(Magick::Warning &warn_ ) { emit warningMessage(warn_.what()); }
}

void Editor::writeLayer(const QString &filename, int id)
{
    if (filename.isEmpty() || !getCurrentView() || id<0) { return; }

    Magick::Image image = getCurrentView()->getCanvasProject().layers[id].image;
    // TODO: add options for file format
    try {
        QFileInfo info(filename);
        if (info.suffix().toLower() == "tiff" ||
            info.suffix().toLower() == "tif")
        {
            image.magick("TIFF");
        }
        else if (info.suffix().toLower() == "jpg" ||
                   info.suffix().toLower() == "jpeg")
        {
            image.magick("JPG");
        }
        else if (info.suffix().toLower() == "png")
        {
            image.magick("PNG");
        }
        else if (info.suffix().toLower() == "bmp")
        {
            image.magick("BMP");
        }
        image.write(filename.toStdString());
    }
    catch(Magick::Error &error_ ) { emit errorMessage(error_.what()); }
    catch(Magick::Warning &warn_ ) { emit warningMessage(warn_.what()); }
}

#ifdef WITH_FFMPEG
void Editor::readAudio(const QString &filename)
{
    if (filename.isEmpty()) { return; }
    QByteArray coverart = common.getEmbeddedCoverArt(filename);
    if (coverart.size()==0) { return; }
    qDebug() << "found image in audio!";
    try {
        Magick::Image image;
        readImage(Magick::Blob(coverart.data(),
                               static_cast<size_t>(coverart.size())),
                  filename);
    }
    catch(Magick::Error &error_ ) { emit errorMessage(error_.what()); }
    catch(Magick::Warning &warn_ ) { emit warningMessage(warn_.what()); }
}

void Editor::readVideo(const QString &filename)
{
    if (filename.isEmpty()) { return; }
    int maxFrame = common.getVideoMaxFrames(filename);
    if (maxFrame==0) { return; }

    videoDialog *dialog = new videoDialog(this,
                                          maxFrame,
                                          filename);
    int ret = dialog->exec();
    if (ret == QDialog::Accepted) {
        if (dialog->getFrames().isNull()) { // open one frame as image
            readVideo(filename,
                      dialog->getFrame());
        } else { // open muliple frames as images
            for (int i=dialog->getFrames().width();i<dialog->getFrames().height()+1;++i) {
                readVideo(filename, i);
            }
            mdi->tileSubWindows(); // tile on multiple images
        }
    }
    QTimer::singleShot(100,
                       dialog,
                       SLOT(deleteLater()));
}

void Editor::readVideo(const QString &filename, int frame)
{
    if (filename.isEmpty() || frame<0) { return; }
    try {
        Magick::Image image = Common::getVideoFrame(filename, frame);
        Magick::Blob blob;
        image.write(&blob);
        if (blob.length()>0) { readImage(blob, filename); }
    }
    catch(Magick::Error &error_ ) { emit errorMessage(error_.what()); }
    catch(Magick::Warning &warn_ ) { emit warningMessage(warn_.what()); }
}

Magick::Image Editor::getVideoFrameAsImage(const QString &filename)
{
    Magick::Image result;
    if (filename.isEmpty()) { return result; }
    int maxFrame = common.getVideoMaxFrames(filename);
    if (maxFrame==0) { return result; }
    videoDialog *dialog = new videoDialog(this,
                                          maxFrame,
                                          filename);
    int ret = dialog->exec();
    if (ret == QDialog::Accepted) {
        result = Common::getVideoFrame(filename,
                                       dialog->getFrame());
    }
    QTimer::singleShot(100,
                       dialog,
                       SLOT(deleteLater()));
    return result;
}

Magick::Image Editor::getVideoFrameAsImage(const QString &filename,
                                           int frame)
{
    Magick::Image result;
    if (filename.isEmpty()) { return result; }
    int maxFrame = common.getVideoMaxFrames(filename);
    if (maxFrame==0) { return result; }
    if (frame>maxFrame) { frame = maxFrame; }
    if (frame<0) { frame = 0; }
    result = Common::getVideoFrame(filename,
                                   frame);
    return result;
}
#endif

void Editor::saveProjectDialog()
{
    if (!getCurrentView()) { return; }
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save Project"),
                                                    QDir::homePath(),
                                                    tr("Project Files (*.miff)"));
    if (!filename.endsWith(".miff",
                           Qt::CaseInsensitive)) { filename.append(".miff"); }
    saveProject(filename);
}

void Editor::saveImageDialog()
{
    if (!getCurrentView()) { return; }
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save Image"),
                                                    QString("%1/%2")
                                                    .arg(QDir::homePath())
                                                    .arg(getCurrentView()->getCanvasProject().label),
                                                    tr("Image files (%1)")
                                                    .arg(common.supportedWriteFormats()));
    if (filename.isEmpty()) { return; }
    QFileInfo fileInfo(filename);
    if (fileInfo.suffix().isEmpty()) {
        QMessageBox::warning(this,
                             tr("No image suffix"),
                             tr("Missing image suffix, try again."));
        return;
    }
    writeImage(filename);
}

void Editor::saveLayerDialog()
{
    if (!getCurrentView()) { return; }
    LayerTreeItem *layerItem = dynamic_cast<LayerTreeItem*>(layersTree->currentItem());
    if (!layerItem) {
        QMessageBox::warning(this,
                             tr("No layer selected"),
                             tr("No layer selected"));
        return;
    }
    QString label = getCurrentView()->getCanvasProject().label;
    if (!getCurrentView()->getCanvasProject().layers[layerItem->getLayerID()].label.isEmpty()) {
        label = getCurrentView()->getCanvasProject().layers[layerItem->getLayerID()].label;
    }
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save Image"),
                                                    QString("%1/%2")
                                                    .arg(QDir::homePath())
                                                    .arg(label),
                                                    tr("Image files (%1)")
                                                    .arg(common.supportedWriteFormats()));
    if (filename.isEmpty()) { return; }
    QFileInfo fileInfo(filename);
    if (fileInfo.suffix().isEmpty()) {
        QMessageBox::warning(this,
                             tr("No image suffix"),
                             tr("Missing image suffix, try again."));
        return;
    }
    writeLayer(filename, layerItem->getLayerID());
}

void Editor::loadImageDialog()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open Media"),
                                                    QDir::homePath(),
                                                    tr("Media Files (%1)")
                                                    .arg(common.supportedReadFormats()));
    if (filename.isEmpty()) { return; }

    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(filename);
#ifdef WITH_FFMPEG
    if (type.name().startsWith(QString("audio"))) {
        readAudio(filename);
    } else if(type.name().startsWith(QString("video"))) {
        readVideo(filename);
    } else {
        loadImage(filename);
    }
#else
    if (type.name().startsWith(QString("audio")) ||
        type.name().startsWith(QString("video"))) { return; }
    loadImage(filename);
#endif
}

void Editor::newImageDialog()
{
    NewMediaDialog *dialog = new NewMediaDialog(this);
    int res =  dialog->exec();
    if (res == QDialog::Accepted) { newTab(dialog->getImage()); }
    QTimer::singleShot(100,
                       dialog,
                       SLOT(deleteLater()));
}

void Editor::newLayerDialog()
{
    if (!getCurrentView()) { return; }
    NewMediaDialog *dialog = new NewMediaDialog(this,
                                                tr("New Layer"),
                                                Common::newLayerDialogType,
                                                getCurrentView()->getCanvas().colorSpace(),
                                                getCurrentView()->getCanvasProject().profile,
                                                getCurrentView()->getCanvasSize());
    int res =  dialog->exec();
    if (res == QDialog::Accepted) {
        getCurrentView()->addLayer(dialog->getImage());
    }

    QTimer::singleShot(100,
                       dialog,
                       SLOT(deleteLater()));
}

/*void Editor::handleNewImage(Magick::Image image)
{
    if (image.columns()>0 &&
        image.rows()>0) { newTab(image); }
}*/





void Editor::connectView(View *view)
{
    if (!view) { return; }
    qDebug() << "connect new view";
    connect(view, SIGNAL(selectedLayer(int)), this, SLOT(handleLayerSelected(int)));
    connect(view, SIGNAL(errorMessage(QString)), this, SLOT(handleError(QString)));
    connect(view, SIGNAL(statusMessage(QString)), this, SLOT(handleStatus(QString)));
    connect(view, SIGNAL(warningMessage(QString)), this, SLOT(handleStatus(QString)));
    connect(view, SIGNAL(viewClosed()), this, SLOT(handleViewClosed()));
    connect(view, SIGNAL(updatedLayers()), this, SLOT(handleLayersUpdated()));
    connect(view, SIGNAL(switchMoveTool()), this, SLOT(handleSwitchMoveTool()));
    connect(view, SIGNAL(updatedBrushStroke(int)), this, SLOT(handleUpdateBrushSize(int)));
    connect(view, SIGNAL(openImages(QList<QUrl>)), this, SLOT(handleOpenImages(QList<QUrl>)));
    connect(view, SIGNAL(openLayers(QList<QUrl>)), this, SLOT(handleOpenLayers(QList<QUrl>)));
    connect(layersTree, SIGNAL(moveLayerEvent(QKeyEvent*)), view, SLOT(moveLayerEvent(QKeyEvent*)));
}

void Editor::setViewTool(View *view)
{
    if (!view) { return; }
    qDebug() << "set view tool";
    if (viewMoveAct->isChecked()) {
        view->setInteractiveMode(View::IteractiveMoveMode);
    } /*else if (viewDragAct->isChecked()) {
        view->setInteractiveMode(View::InteractiveDragMode);
    }*/ else if (viewDrawAct->isChecked()) {
        view->setInteractiveMode(View::InteractiveDrawMode);
    }
}

// TODO
/*void Editor::newTabFromLayer(View *parentView,
                             int layerID)
{
    //qDebug() << "new tab from layer" << layerID;
    return;


    if (!parentView) { return; }
    Common::Layer layer  = parentView->getLayer(layerID);
    QMdiSubWindow *tab = new QMdiSubWindow(mdi);
    View *view = new View(this, layerID);

    connect(view, SIGNAL(selectedLayer(int)), this, SLOT(handleLayerSelected(int)));
    connect(view, SIGNAL(errorMessage(QString)), this, SLOT(handleError(QString)));
    connect(view, SIGNAL(statusMessage(QString)), this, SLOT(handleStatus(QString)));
    connect(view, SIGNAL(warningMessage(QString)), this, SLOT(handleStatus(QString)));
    connect(view, SIGNAL(viewClosed()), this, SLOT(handleViewClosed()));
    connect(view, SIGNAL(updatedLayers()), this, SLOT(handleLayersUpdated()));
    //connect(view, SIGNAL(updatedCanvas(View::Canvas,int)), parentView, SLOT(setLayerFromCanvas(View::Canvas,int)));
    connect(view, SIGNAL(switchMoveTool()), this, SLOT(handleSwitchMoveTool()));
    connect(view, SIGNAL(updateBrushSize(bool)), this, SLOT(handleUpdateBrushSize(bool)));

    tab->setWidget(view);
    tab->setWindowTitle(QString::fromStdString(layer.image.fileName()).split("/").takeLast());
    tab->setAttribute(Qt::WA_DeleteOnClose);

    //tab->showNormal();
    tab->showMaximized();
    //mdi->tileSubWindows();

    view->setCanvasSpecsFromImage(layer.image);
    view->addLayer(layer.image);
    view->setFit(true);
    if (viewMoveAct->isChecked()) {
        view->setInteractiveMode(View::IteractiveMoveMode);
    } else if (viewDragAct->isChecked()) {
        view->setInteractiveMode(View::InteractiveDragMode);
    } else if (viewDrawAct->isChecked()) {
        view->setInteractiveMode(View::InteractiveDrawMode);
    }
    updateTabTitle(view);
    //parseImageInfo(view->getCanvas());
}*/


void Editor::handleViewClosed()
{
    qDebug() << "view closed";
    layersTree->clear();
    layersTree->handleTabActivated(mdi->currentSubWindow());
}


/*void Editor::handleSetDragMode(bool triggered)
{
    qDebug() << "set drag mode" << triggered;
    if (!triggered) {
        viewDragAct->setChecked(true);
        return;
    }
    handleSwitchMoveTool(View::InteractiveDragMode);
}*/








void Editor::handleOpenImages(const QList<QUrl> &urls)
{
    qDebug() << "open images" << urls;
    if (urls.size()==0) { return; }
    for (int i=0;i<urls.size();++i) {
        QString filename = urls.at(i).toLocalFile();
        QMimeDatabase db;
        QMimeType type = db.mimeTypeForFile(filename);
#ifdef WITH_FFMPEG
        if (type.name().startsWith(QString("audio"))) { // try to get "coverart" from audio
            readAudio(filename);
        } else if (type.name().startsWith(QString("video"))) { // get frame from video
            readVideo(filename);
        } else { // "regular" image
            if (common.isValidCanvas(filename)) { loadProject(filename); }
            else { readImage(filename); }
        }
#else
        if (type.name().startsWith(QString("audio")) ||
            type.name().startsWith(QString("video"))) { continue; }
        if (common.isValidCanvas(filename)) { loadProject(filename); }
        else { readImage(filename); }
#endif
    }
    if (urls.size()>1) {
        mdi->tileSubWindows();
    }
}







void Editor::handleOpenLayers(const QList<QUrl> &urls)
{
    View *view = qobject_cast<View*>(sender());
    if (!view) { return; }

    qDebug() << "open layers" << urls;

    for (int i=0;i<urls.size();++i) {
        QString filename = urls.at(i).toLocalFile();

        QMimeDatabase db;
        QMimeType type = db.mimeTypeForFile(urls.at(i).toString());
        Magick::Image image;

        try {
#ifdef WITH_FFMPEG
            if (type.name().startsWith(QString("audio"))) { // try to get "coverart" from audio
                QByteArray coverart = common.getEmbeddedCoverArt(filename);
                if (coverart.size()==0) { continue; }
                image.read(Magick::Blob(coverart.data(),
                                        static_cast<size_t>(coverart.size())));
            } else if (type.name().startsWith(QString("video"))) { // get frame from video
                image = getVideoFrameAsImage(filename);
            } else { // "regular" image
                if (Common::isValidCanvas(filename)) { continue; }
                image.read(filename.toStdString());
            }
#else
            if (type.name().startsWith(QString("audio")) ||
                type.name().startsWith(QString("video"))) { continue; }
            if (Common::isValidCanvas(filename)) {
                // skip projects
                continue;
            }
            image.read(filename.toStdString());
#endif

            if (image.columns()<=0 ||
                image.rows()<=0) { continue; } // not an (readable) image, skip
            image.magick("MIFF");
            image.fileName(filename.toStdString());
            if (image.label().empty()) {
                QFileInfo fileInfo(filename);
                image.label(fileInfo.baseName().toStdString());
            }

            addLayerToView(image, view);
        }
        catch(Magick::Error &error_ ) { emit errorMessage(error_.what()); }
        catch(Magick::Warning &warn_ ) { emit warningMessage(warn_.what()); }
    }
    // workaround issues with dialogs
    update();
    view->scene()->update();
}





