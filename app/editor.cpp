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

#include "editor.h"

#include <QFileDialog>
#include <QDir>
#include <QImage>
#include <QPixmap>
#include <QPluginLoader>
#include <QApplication>
#include <QToolButton>
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
#include <QtConcurrent/QtConcurrentRun>

#include "CyanNewMediaDialog.h"
#include "convertdialog.h"
#include "CyanColorConvert.h"

#ifdef WITH_FFMPEG
#include "CyanFFmpegOpenDialog.h"
#include "CyanFFmpeg.h"
#endif

#ifdef Q_OS_MAC
#define CYAN_FONT_SIZE 10
#else
#define CYAN_FONT_SIZE 8
#endif

Editor::Editor(QWidget *parent)
    : QMainWindow(parent)
    , pluginHandler(nullptr)
    , mdi(nullptr)
    , mainToolBar(nullptr)
    , mainMenu(nullptr)
    , mainStatusBar(nullptr)
    , newImageAct(nullptr)
    , openImageAct(nullptr)
    , saveImageAct(nullptr)
    , saveImageAsAct(nullptr)
    //, saveProjectAct(nullptr)
    //, saveProjectAsAct(nullptr)
    , quitAct(nullptr)
    , viewMoveAct(nullptr)
    , viewDrawAct(nullptr)
    , viewZoom100Act(nullptr)
    , viewZoomFitAct(nullptr)
    , aboutCyanAct(nullptr)
    , aboutImageMagickAct(nullptr)
    , aboutLcmsAct(nullptr)
    , aboutQtAct(nullptr)
#ifdef WITH_FFMPEG
    , aboutFFmpegAct(nullptr)
#endif
    , convertRGBAct(nullptr)
    , convertCMYKAct(nullptr)
    , convertGRAYAct(nullptr)
    , convertAssignAct(nullptr)
    , convertExtractAct(nullptr)
    , addGuideVAct(nullptr)
    , addGuideHAct(nullptr)
    , showGuidesAct(nullptr)
    , magickMemoryResourcesGroup(nullptr)
    , viewModeGroup(nullptr)
    , profileRGBGroup(nullptr)
    , profileCMYKGroup(nullptr)
    , profileGRAYGroup(nullptr)
    , fileMenu(nullptr)
    , optMenu(nullptr)
    , helpMenu(nullptr)
    //, saveMenu(nullptr)
    , colorMenu(nullptr)
    , colorProfileRGBMenu(nullptr)
    , colorProfileCMYKMenu(nullptr)
    , colorProfileGRAYMenu(nullptr)
    , colorIntentMenu(nullptr)
    , viewMenu(nullptr)
    , guideMenu(nullptr)
    , memoryMenu(nullptr)
    , editMenu(nullptr)
    , windowsMenu(nullptr)
    , newButton(nullptr)
    , openButton(nullptr)
    , saveButton(nullptr)
    , interactButton(nullptr)
    , layersWidget(nullptr)
    , colorPicker(nullptr)
    , currentZoomStatusIcon(nullptr)
    , currentZoomStatusLabel(nullptr)
    , mainSplitter(nullptr)
    , rightSplitter(nullptr)
    , leftSplitter(nullptr)
    , bottomSplitter(nullptr)
    , middleSplitter(nullptr)
    , topSplitter(nullptr)
    , hasTextSupport(false)
{
    // set window title
    setWindowTitle(qApp->applicationName());
    setAttribute(Qt::WA_QuitOnClose);

    // register Magick Image
    qRegisterMetaType<Magick::Image>("Magick::Image");

    // setup UI and load settings
    setupUI();
    pluginHandler = new CyanPlugins(this);
    initPlugins(pluginHandler->scan());
    loadSettings();
}

// save settings on quit
Editor::~Editor()
{
    saveSettings();
    pluginHandler->removePlugins();
}

// get current active canvas
View *Editor::getCurrentCanvas()
{
    QMdiSubWindow *tab = mdi->currentSubWindow();
    if (!tab) { return nullptr; }
    View *view = qobject_cast<View*>(tab->widget());
    if (!view) { return nullptr; }
    return view;
}

void Editor::initPlugins(QList<QPluginLoader *> plugins)
{
    // TODO get order from settings!
    for (int i=0;i<plugins.size();++i) {
        QObject *plugin = plugins.at(i)->instance();
        if (!plugin) { continue; }
        CyanWidgetPlugin *widgetPlugin = qobject_cast<CyanWidgetPlugin*>(plugin);
        if (widgetPlugin) {
            initWidgetPlugin(widgetPlugin);
            connect(colorPicker,
                    SIGNAL(colorChanged(QColor)),
                    plugin,
                    SLOT(setCurrentColor(QColor)));
            connect(plugin,
                    SIGNAL(currentColorChanged(QColor)),
                    colorPicker,
                    SLOT(setCurrentColor(QColor)));
            if (widgetPlugin->isText()) {
                hasTextSupport = true;
                connect(this,
                        SIGNAL(currentTextChanged(QString,int,int,bool)),
                        plugin,
                        SLOT(setCurrentText(QString,int,int,bool)));
                connect(plugin,
                        SIGNAL(currentTextChanged(QString,int,int,bool)),
                        this,
                        SLOT(handleCurrentLayerTextChanged(QString,int,int,bool)));
            }
        }
    }
    layersWidget->setTextSupport(hasTextSupport);
    emit currentTextChanged(QString(""), 0, 0, false);
}

void Editor::initWidgetPlugin(CyanWidgetPlugin *plugin)
{
    if (!plugin) { return; }
    qDebug() << "==> Loaded new Widget Plugin:" << plugin->uuid() << plugin->version() << plugin->title() << plugin->desc();
    int pos = plugin->position();
    // TODO add settings override!
    switch (pos) {
    case CyanWidgetPlugin::CyanWidgetPluginBottomPosition:
        qDebug() << "add plugin to bottom splitter";
        bottomSplitter->addWidget(plugin->getWidget(this));
        break;
    case CyanWidgetPlugin::CyanWidgetPluginTopPosition:
        qDebug() << "add plugin to top splitter";
        topSplitter->addWidget(plugin->getWidget(this));
        break;
    case CyanWidgetPlugin::CyanWidgetPluginRightPosition:
        qDebug() << "add plugin to right splitter";
        rightSplitter->addWidget(plugin->getWidget(this));
        break;
    default:
        qDebug() << "add plugin to left splitter";
        leftSplitter->addWidget(plugin->getWidget(this));
        break;
    }
    plugin->setCurrentColor(colorPicker->currentColor());
}

// save global settings
void Editor::saveSettings()
{
    emit statusMessage(tr("Saving settings ..."));
    QSettings settings;

    settings.beginGroup("engine");
    settings.setValue("disk_limit",
                      CyanCommon::getDiskResource());
    settings.setValue("memory_limit",
                      CyanCommon::getMemoryResource());
    settings.endGroup();

    settings.beginGroup("gui");
    settings.setValue("editor_state",
                      saveState());
    settings.setValue("editor_geometry",
                      saveGeometry());
    settings.setValue("editor_maximized",
                      isMaximized());

#ifndef Q_OS_MAC
    settings.setValue("mdi", mdi->viewMode());
#endif

    settings.setValue("mainSplitter_state", mainSplitter->saveState());
    settings.setValue("mainSplitter_geometry", mainSplitter->saveGeometry());
    settings.setValue("leftSplitter_state", leftSplitter->saveState());
    settings.setValue("leftSplitter_geometry", leftSplitter->saveGeometry());
    settings.setValue("rightSplitter_state", rightSplitter->saveState());
    settings.setValue("rightSplitter_geometry", rightSplitter->saveGeometry());
    settings.setValue("bottomSplitter_state", bottomSplitter->saveState());
    settings.setValue("bottomSplitter_geometry", bottomSplitter->saveGeometry());
    settings.setValue("middleSplitter_state", middleSplitter->saveState());
    settings.setValue("middleSplitter_geometry", middleSplitter->saveGeometry());
    settings.setValue("topSplitter_state", topSplitter->saveState());
    settings.setValue("topSplitter_geometry", topSplitter->saveGeometry());
    settings.endGroup();

    settings.beginGroup(QString("color"));
    settings.setValue(QString("rgb_profile"),
                      selectedDefaultColorProfile(profileRGBGroup));
    settings.setValue(QString("cmyk_profile"),
                      selectedDefaultColorProfile(profileCMYKGroup));
    settings.setValue(QString("gray_profile"),
                      selectedDefaultColorProfile(profileGRAYGroup));
    settings.setValue(QString("blackpoint"),
                      blackPointAct->isChecked());
    settings.endGroup();

    // view
    settings.beginGroup("view");
    settings.setValue("guides", showGuidesAct->isChecked());
    settings.endGroup();
}

void Editor::saveSettingsLastOpenDir(const QString &dir)
{
    if (dir.isEmpty()) { return; }
    QSettings settings;
    settings.beginGroup("files");
    settings.setValue("lastOpenDir", dir);
    settings.endGroup();
    settings.sync();
}

void Editor::saveSettingsLastSaveDir(const QString &dir)
{
    if (dir.isEmpty()) { return; }
    QSettings settings;
    settings.beginGroup("files");
    settings.setValue("lastSaveDir", dir);
    settings.endGroup();
    settings.sync();
}

// load global settings
void Editor::loadSettings()
{
    emit statusMessage(tr("Loading settings ..."));
    QSettings settings;

    settings.beginGroup("engine");
    CyanCommon::setDiskResource(settings
                            .value("disk_limit", 0).toInt());
    int maxMem = settings.value("memory_limit", 4).toInt();
    CyanCommon::setMemoryResource(maxMem);
    settings.endGroup();

    QList<QAction*> memActions = magickMemoryResourcesGroup->actions();
    bool foundAct = false;
    for (int i=0;i<memActions.size();++i) {
        QAction *act = memActions.at(i);
        if (!act) { continue; }
        if (act->data().toInt()==maxMem) {
            act->setChecked(true);
            foundAct = true;
            break;
        }
    }
    if (!foundAct) { memoryMenu->setDisabled(true); }

    settings.beginGroup("gui");
    if (settings.value("editor_state").isValid()) {
        restoreState(settings.value("editor_state").toByteArray());
    }
    if (settings.value("editor_geometry").isValid()) {
        restoreGeometry(settings.value("editor_geometry").toByteArray());
    }

#ifdef Q_OS_MAC
    mdi->setViewMode(QMdiArea::TabbedView);
#else
    mdi->setViewMode(static_cast<QMdiArea::ViewMode>(settings
                                                     .value("mdi",
                                                            QMdiArea::SubWindowView)
                                                     .toInt()));
    QList<QAction*> viewActions = viewModeGroup->actions();
    for (int i=0;i<viewActions.size();++i) {
        QAction *act = viewActions.at(i);
        if (!act) { continue; }
        if (static_cast<QMdiArea::ViewMode>(act->data().toInt()) == mdi->viewMode()) {
            act->setChecked(true);
            break;
        }
    }
    if (mdi->viewMode() == QMdiArea::TabbedView) { // workaround qtbug
        QTabBar *mdiBar = mdi->findChild<QTabBar*>();
        if (mdiBar) {
            mdiBar->setDrawBase(false);
        }
    }
#endif

    if (settings.value("mainSplitter_state").isValid()) {
        mainSplitter->restoreState(settings.value("mainSplitter_state").toByteArray());
    }
    if (settings.value("mainSplitter_geometry").isValid()) {
        mainSplitter->restoreGeometry(settings.value("mainSplitter_geometry").toByteArray());
    }
    if (settings.value("leftSplitter_state").isValid()) {
        leftSplitter->restoreState(settings.value("leftSplitter_state").toByteArray());
    }
    if (settings.value("leftSplitter_geometry").isValid()) {
        leftSplitter->restoreGeometry(settings.value("leftSplitter_geometry").toByteArray());
    }
    if (settings.value("rightSplitter_state").isValid()) {
        rightSplitter->restoreState(settings.value("rightSplitter_state").toByteArray());
    }
    if (settings.value("rightSplitter_geometry").isValid()) {
        rightSplitter->restoreGeometry(settings.value("rightSplitter_geometry").toByteArray());
    }
    if (settings.value("bottomSplitter_state").isValid()) {
        bottomSplitter->restoreState(settings.value("bottomSplitter_state").toByteArray());
    }
    if (settings.value("bottomSplitter_geometry").isValid()) {
        bottomSplitter->restoreGeometry(settings.value("bottomSplitter_geometry").toByteArray());
    }
    if (settings.value("middleSplitter_state").isValid()) {
        middleSplitter->restoreState(settings.value("middleSplitter_state").toByteArray());
    }
    if (settings.value("middleSplitter_geometry").isValid()) {
        middleSplitter->restoreGeometry(settings.value("middleSplitter_geometry").toByteArray());
    }
    if (settings.value("topSplitter_state").isValid()) {
        topSplitter->restoreState(settings.value("topSplitter_state").toByteArray());
    }
    if (settings.value("topSplitter_geometry").isValid()) {
        topSplitter->restoreGeometry(settings.value("topSplitter_geometry").toByteArray());
    }

    if (settings.value("editor_maximized").toBool()) { showMaximized(); }
    settings.endGroup();

    emit statusMessage(QString("%2: %1 GB")
                       .arg(CyanCommon::getDiskResource())
                       .arg(tr("Engine disk cache limit")));
    emit statusMessage(QString("%2: %1 GB")
                       .arg(CyanCommon::getMemoryResource())
                       .arg(tr("Engine memory limit")));

    // setup color profiles
    qDebug() << "setup color profiles";
    setDefaultColorProfiles();

    // setup color intent
    qDebug() << "setup color intent";
    loadDefaultColorIntent();

    // add blackpoint
    settings.beginGroup(QString("color"));
    blackPointAct->setChecked(settings.value(QString("blackpoint"),
                                             true)
                              .toBool());
    settings.endGroup();

    // view
    settings.beginGroup("view");
    showGuidesAct->setChecked(settings.value("guides", true).toBool());
    settings.endGroup();

    // check if we have the required color profiles needed to do anything
    QTimer::singleShot(100, this, SLOT(hasColorProfiles()));
}

const QString Editor::loadSettingsLastOpenDir()
{
    QString result;
    QSettings settings;
    settings.beginGroup("files");
    result = settings.value("lastOpenDir", QDir::homePath()).toString();
    settings.endGroup();
    return result;
}

const QString Editor::loadSettingsLastSaveDir()
{
    QString result;
    QSettings settings;
    settings.beginGroup("files");
    result = settings.value("lastSaveDir", QDir::homePath()).toString();
    settings.endGroup();
    return result;
}

// load image project (*.MIFF)
void Editor::loadProject(const QString &filename)
{
    if (filename.isEmpty()) { return; }
    QFileInfo fileInfo(filename);
    if (fileInfo.suffix().toLower() != "miff") { return; }
    if (CyanImageFormat::isValidCanvas(filename)) {
        qDebug() << "is valid project, reading ...";
        CyanImageFormat::CyanCanvas canvas = CyanImageFormat::readCanvas(filename);
        canvas.filename = filename;
        newTab(canvas);
    } else {
        QMessageBox::warning(this,
                             tr("Invalid project"),
                             tr("This file is not a valid Cyan project file"));
    }
}

// save image project (*.MIFF)
void Editor::writeProject(const QString &filename, bool setFilename)
{
    qDebug() << "save project" << filename;
    if (filename.isEmpty() || !getCurrentCanvas()) { return; }
    if (CyanImageFormat::writeCanvas(getCurrentCanvas()->getCanvasProject(),
                                     filename,
                                     MagickCore::ZipCompression)) {
        // TODO, verify project!
        if (setFilename) {
            getCurrentCanvas()->setFilename(filename);
            qDebug() << "was filename set in canvas?" << getCurrentCanvas()->getFilename();
        }
        getCurrentCanvas()->setDirty(false);
        handleCanvasStatus();
    } else {
        QMessageBox::warning(this,
                             tr("Save error"),
                             tr("Failed to save the project!"));
    }
}

/*void Editor::saveImage(const QString &filename)
{
    qDebug() << "save image" << filename;
}*/

// load unknown image
void Editor::loadImage(const QString &filename)
{
    if (filename.isEmpty() || !QFile::exists(filename)) { return; }
    if (CyanImageFormat::isValidCanvas(filename)) { // cyan project
        emit statusMessage(QString("%2 %1").arg(filename).arg(tr("Loading canvas")));
        loadProject(filename);
        emit statusMessage(tr("Done"));
    } else { // regular image
        // TODO
        qDebug() << "HAS LAYERS?" << CyanImageFormat::hasLayers(filename);
        emit statusMessage(QString("%2 %1").arg(filename).arg(tr("Loading image")));
        readImage(filename);
        emit statusMessage(tr("Done"));
    }
}

// read "regular" new image
void Editor::readImage(Magick::Blob blob,
                       const QString &filename)
{
    qDebug() << "read image" << filename;
    Magick::Image image;
    image.quiet(false);

    try {
        if (blob.length()>0) {
            image.read(blob);
        } else if (!filename.isEmpty()) {
            image.read(filename.toStdString());
        } else { return; }
    }
    catch(Magick::Error &error_ ) {
        emit errorMessage(error_.what());
        return;
    }
    catch(Magick::Warning &warn_ ) { emit warningMessage(warn_.what()); }

    try {
        // DCM
        if (image.format() == "Digital Imaging and Communications in Medicine image") {
            image.defineValue("dcm", "display-range", "reset");
            image.autoLevel();
        }
        image.magick("MIFF"); // force internal format
        image.fileName(filename.toStdString());
        if (image.label().empty()) { // add label, use filename as fallback
            QFileInfo fileInfo(filename);
            image.label(fileInfo.baseName().toStdString());
        }

        // check for color profile, if none add
        if (image.iccColorProfile().length()==0) {
            QString defPro;
            switch(image.colorSpace()) {
            case Magick::CMYKColorspace:
                defPro = selectedDefaultColorProfile(profileCMYKGroup);
                break;
            case Magick::GRAYColorspace:
                defPro = selectedDefaultColorProfile(profileGRAYGroup);
                break;
            default:
                defPro = selectedDefaultColorProfile(profileRGBGroup);
            }
            if (defPro.isEmpty()) {
                QMessageBox::warning(this,
                                     tr("Missing default profile"),
                                     tr("Missing default profile for the selected color space!"));
                return;
            }
            ConvertDialog *dialog = new ConvertDialog(this,
                                                      tr("Assign color profile"),
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
                    newTab(image); // add new image to new tab
                }
            }
            QTimer::singleShot(100,
                               dialog,
                               SLOT(deleteLater()));
        } else {
            if (image.columns()>0 &&
                image.rows()>0)
            {
                newTab(image); // add new image to new tab
            }
        }
    }
    catch(Magick::Error &error_ ) { emit errorMessage(error_.what()); }
    catch(Magick::Warning &warn_ ) { emit warningMessage(warn_.what()); }
}

// read image "wrapper"
void Editor::readImage(const QString &filename)
{
    if (filename.isEmpty()) { return; }
    readImage(Magick::Blob(), filename);
}

// write "regular" image to file
void Editor::writeImage(const QString &filename, bool setFilename)
{
    qDebug() << "write image" << filename;
    if (filename.isEmpty() || !getCurrentCanvas()) { return; }

    Magick::Image image = CyanImageFormat::renderCanvasToImage(getCurrentCanvas()->getCanvasProject());
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
        //getCurrentCanvas()->getCanvasProject().filename = filename;
        if (setFilename) {
            getCurrentCanvas()->setFilename(filename);
        }
    }
    catch(Magick::Error &error_ ) { emit errorMessage(error_.what()); }
    catch(Magick::Warning &warn_ ) { emit warningMessage(warn_.what()); }
}

// write "regular" image from selected layer to file
void Editor::writeLayer(const QString &filename,
                        int id)
{
    if (filename.isEmpty() || !getCurrentCanvas() || id<0) { return; }

    Magick::Image image = getCurrentCanvas()->getCanvasProject().layers[id].image;
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
    if (filename.isEmpty() || !QFile::exists(filename)) { return; }
    QByteArray coverart = CyanFFmpeg::getEmbeddedCoverArt(filename);
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
    if (filename.isEmpty() || !QFile::exists(filename)) { return; }
    int maxFrame = CyanFFmpeg::getVideoMaxFrames(filename);
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
        Magick::Image image = CyanFFmpeg::getVideoFrame(filename, frame);
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
    int maxFrame = CyanFFmpeg::getVideoMaxFrames(filename);
    if (maxFrame==0) { return result; }
    videoDialog *dialog = new videoDialog(this,
                                          maxFrame,
                                          filename);
    int ret = dialog->exec();
    if (ret == QDialog::Accepted) {
        result = CyanFFmpeg::getVideoFrame(filename,
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
    int maxFrame = CyanFFmpeg::getVideoMaxFrames(filename);
    if (maxFrame==0) { return result; }
    if (frame>maxFrame) { frame = maxFrame; }
    if (frame<0) { frame = 0; }
    result = CyanFFmpeg::getVideoFrame(filename,
                                   frame);
    return result;
}
#endif

// save cyan project filename dialog
/*void Editor::saveProjectDialog()
{
    if (!getCurrentCanvas()) { return; }
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save Project"),
                                                    loadSettingsLastSaveDir(),
                                                    QString("%1 (*.miff)")
                                                    .arg(tr("Project Files")));
    if (!filename.endsWith(".miff",
                           Qt::CaseInsensitive)) { filename.append(".miff"); }

    QFileInfo fileInfo(filename);
    if (fileInfo.absolutePath() != loadSettingsLastSaveDir()) {
        saveSettingsLastSaveDir(fileInfo.absolutePath());
    }

    saveProject(filename);
}*/

// save "regular" image filename dialog
void Editor::saveImageDialog(bool ignoreExisting, bool setFilename)
{
    qDebug() << "saveImageDialog";
    if (!getCurrentCanvas()) { return; }
    QString filename = getCurrentCanvas()->getFilename();
    bool fileExists = QFile::exists(filename);
    if (ignoreExisting) { fileExists = false; }
        qDebug() << "save to existing image?" << filename << fileExists << "ignore override" << ignoreExisting;
    if (!fileExists) {
        qDebug() << "save image dialog";
        filename = QFileDialog::getSaveFileName(this,
                                                tr("Save Image"),
                                                QString("%1/%2")
                                                .arg(loadSettingsLastSaveDir())
                                                .arg(getCurrentCanvas()->getCanvasProject().label),
                                                QString("%2 (%1)")
                                                .arg(CyanImageFormat::supportedWriteFormats())
                                                .arg(tr("Image files")));
    }
    if (filename.isEmpty()) { return; }
    QFileInfo fileInfo(filename);
    if (fileInfo.suffix().isEmpty()) {
        QMessageBox::warning(this,
                             tr("No image suffix"),
                             tr("Missing image suffix, try again."));
        return;
    }

    if (fileInfo.absolutePath() != loadSettingsLastSaveDir()) {
        saveSettingsLastSaveDir(fileInfo.absolutePath());
    }

    if (filename.endsWith(".miff",
                           Qt::CaseInsensitive))
    {
        QtConcurrent::run(this,
                          &Editor::writeProject,
                          filename,
                          setFilename);
    } else {
        QtConcurrent::run(this,
                          &Editor::writeImage,
                          filename,
                          setFilename);
    }
}

void Editor::saveImageAsDialog()
{
    saveImageDialog(true, false);
}

// save selected layer to "regular" image filename dialog
void Editor::saveLayerDialog()
{
    if (!getCurrentCanvas()) { return; }
    CyanLayerTreeItem *layerItem = layersWidget->getCurrentLayer();
    if (!layerItem) {
        QMessageBox::warning(this,
                             tr("No layer selected"),
                             tr("No layer selected"));
        return;
    }
    QString label = getCurrentCanvas()->getCanvasProject().label;
    if (!getCurrentCanvas()->getCanvasProject().layers[layerItem->getLayerID()].label.isEmpty()) {
        label = getCurrentCanvas()->getCanvasProject().layers[layerItem->getLayerID()].label;
    }
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save Image"),
                                                    QString("%1/%2")
                                                    .arg(loadSettingsLastSaveDir())
                                                    .arg(label),
                                                    QString("%2 (%1)")
                                                    .arg(CyanImageFormat::supportedWriteFormats())
                                                    .arg(tr("Image files")));
    if (filename.isEmpty()) { return; }
    QFileInfo fileInfo(filename);
    if (fileInfo.suffix().isEmpty()) {
        QMessageBox::warning(this,
                             tr("No image suffix"),
                             tr("Missing image suffix, try again."));
        return;
    }

    if (fileInfo.absolutePath() != loadSettingsLastSaveDir()) {
        saveSettingsLastSaveDir(fileInfo.absolutePath());
    }

    writeLayer(filename, layerItem->getLayerID());
}

void Editor::loadImageDialog()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open Media"),
                                                    loadSettingsLastOpenDir(),
                                                    QString("%2 (%1)")
                                                    .arg(CyanImageFormat::supportedReadFormats())
                                                    .arg(tr("Media files")));
    if (filename.isEmpty() || !QFile::exists(filename)) { return; }

    QFileInfo fileInfo(filename);
    if (fileInfo.absolutePath() != loadSettingsLastOpenDir()) {
        saveSettingsLastOpenDir(fileInfo.absolutePath());
    }

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

void Editor::newLayerDialog(bool isText)
{
    if (!getCurrentCanvas()) { return; }
    NewMediaDialog *dialog = new NewMediaDialog(this,
                                                tr(isText?"New text layer":"New layer"),
                                                CyanCommon::newLayerDialogType,
                                                getCurrentCanvas()->getCanvas().colorSpace(),
                                                getCurrentCanvas()->getCanvasProject().profile,
                                                getCurrentCanvas()->getCanvasSize());
    int res =  dialog->exec();
    if (res == QDialog::Accepted) {
        getCurrentCanvas()->addLayer(dialog->getImage(), true, false, isText);
    }

    QTimer::singleShot(100,
                       dialog,
                       SLOT(deleteLater()));
}

void Editor::newTextLayerDialog()
{
    newLayerDialog(true /* isText */);
}

void Editor::handleZoom100ActionTriggered()
{
    View *view = qobject_cast<View*>(getCurrentCanvas());
    if (!view) { return; }
    view->resetImageZoom();
    if (view->isFit()) { view->setFit(false); }
    if (viewZoomFitAct->isChecked()) { viewZoomFitAct->setChecked(false); }
}

void Editor::handleZoomFitActionTriggered(bool triggered)
{
    Q_UNUSED(triggered)
    View *view = qobject_cast<View*>(getCurrentCanvas());
    if (!view) { return; }
    view->setFit(viewZoomFitAct->isChecked());
}

void Editor::connectView(View *view)
{
    if (!view) { return; }
    qDebug() << "connect new view";
    //connect(view, SIGNAL(selectedLayer(int)), this, SLOT(handleLayerSelected(int)));
    connect(view, SIGNAL(selectedLayer(int)), layersWidget, SLOT(setCurrentLayer(int)));
    connect(view, SIGNAL(errorMessage(QString)), this, SLOT(handleError(QString)));
    connect(view, SIGNAL(statusMessage(QString)), this, SLOT(handleStatus(QString)));
    connect(view, SIGNAL(warningMessage(QString)), this, SLOT(handleStatus(QString)));
    connect(view, SIGNAL(viewClosed()), this, SLOT(handleViewClosed()));
    connect(view, SIGNAL(updatedLayers()), this, SLOT(handleLayersUpdated()));
    connect(view, SIGNAL(switchMoveTool()), this, SLOT(handleSwitchMoveTool()));
    connect(view, SIGNAL(updatedBrushStroke(int)), this, SLOT(handleUpdateBrushSize(int)));
    connect(view, SIGNAL(openImages(QList<QUrl>)), this, SLOT(handleOpenImages(QList<QUrl>)));
    connect(view, SIGNAL(openLayers(QList<QUrl>)), this, SLOT(handleOpenLayers(QList<QUrl>)));
    connect(view, SIGNAL(zoomChanged()), this, SLOT(handleZoomChanged()));
    connect(view, SIGNAL(myFit(bool)), this, SLOT(handleZoomFitChanged(bool)));
    connect(view, SIGNAL(canvasStatusChanged()), this, SLOT(handleCanvasStatus()));

    connect(layersWidget,
            SIGNAL(moveLayerEvent(QKeyEvent*)),
            view,
            SLOT(moveLayerEvent(QKeyEvent*)));
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
    CyanCommon::Layer layer  = parentView->getLayer(layerID);
    QMdiSubWindow *tab = new QMdiSubWindow(mdi);
    View *view = new View(this, layerID);

    connect(view, SIGNAL(selectedLayer(int)), this, SLOT(handleLayerSelected(int)));
    connect(view, SIGNAL(selectedLayer(int)), layersWidget, SLOT(setCurrentLayer(int)));

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
    layersWidget->clearTree();
    layersWidget->handleTabActivated(mdi->currentSubWindow());
}

void Editor::handleZoomChanged()
{
    qDebug() << "zoom changed";
    View *view = qobject_cast<View*>(sender());
    if (!view) { return; }
    if (view != getCurrentCanvas()) { return; }
    setCurrentZoom();
}

void Editor::handleZoomFitChanged(bool fit)
{
    Q_UNUSED(fit)
    View *view = qobject_cast<View*>(sender());
    if (!view) { return; }
    if (view != getCurrentCanvas()) { return; }
    if (viewZoomFitAct->isChecked() == view->isFit()) { return; }
    viewZoomFitAct->setChecked(view->isFit());
    setCurrentZoom();
}

void Editor::setCurrentZoom()
{
    View *view = getCurrentCanvas();
    if (!view) {
        currentZoomStatusLabel->setText("100%");
        return;
    }
    int value = view->getZoomValue()*100;
    currentZoomStatusLabel->setText(QString("%1%").arg(value));
}

void Editor::handleCanvasStatus()
{
    qDebug() << "handleCanvasStatus";
    View *view = getCurrentCanvas();
    if (!view) {
        //saveProjectAct->setEnabled(false);
        setActionsDisabled(true);
        return;
    }
    setActionsDisabled(false);
    setProjectSaveDisabled(!view->isDirty());
    //saveProjectAct->setEnabled(view->isDirty());
}

void Editor::checkTabsOnClose()
{
    if (hasDirtyProjects()) {
        int ret = QMessageBox::question(this,
                                        tr("Unsaved projects"),
                                        tr("You have unsaved project open, are you sure you want to quit?"),
                                        QMessageBox::Yes|QMessageBox::No,
                                        QMessageBox::No);
        if (ret == QMessageBox::No) { return; }
    }
    qApp->quit();
}

bool Editor::hasDirtyProjects()
{
    bool isDirty = false;
    for (int i = 0; i < mdi->subWindowList().size(); ++i) {
        if (isDirty) { break; }
        QMdiSubWindow *tab = mdi->subWindowList().at(i);
        if (!tab) { continue; }
        View *view = qobject_cast<View*>(tab->widget());
        if (!view) { continue; }
        if (view->isDirty()) { isDirty = true; }
    }
    return isDirty;
}

void Editor::setActionsDisabled(bool disabled)
{
    saveImageAct->setDisabled(disabled);
    saveImageAct->setDisabled(disabled);
    //saveProjectAsAct->setDisabled(disabled);
    interactButton->setDisabled(disabled);
    saveButton->setDisabled(disabled);
    layersWidget->setDisabled(disabled);
    convertCMYKAct->setDisabled(disabled);
    convertGRAYAct->setDisabled(disabled);
    convertRGBAct->setDisabled(disabled);
    convertAssignAct->setDisabled(disabled);
    convertExtractAct->setDisabled(disabled);
}

void Editor::setProjectSaveDisabled(bool disabled)
{
    qDebug() << "setProjectSaveDisabled" << disabled;
    /*if (!saveButton->isEnabled() && !disabled) {
        saveButton->setEnabled(true);
    }
    if (!saveProjectAsAct->isEnabled() && !disabled) {
        saveProjectAsAct->setEnabled(true);
    }*/
    saveButton->setDisabled(disabled);
    saveImageAct->setDisabled(disabled);
    //saveProjectAct->setDisabled(disabled);
}

void Editor::handleAddGuideHAct(bool triggered)
{
    Q_UNUSED(triggered)
    View *view = getCurrentCanvas();
    if (!view) { return; }
    qDebug() << "handle add guide action";
    view->addGuide(true);
}

void Editor::handleAddGuideVAct(bool triggered)
{
    Q_UNUSED(triggered)
    View *view = getCurrentCanvas();
    if (!view) { return; }
    qDebug() << "handle add guide action";
    view->addGuide();
}

void Editor::handleShowGuidesAct(bool triggered)
{
    Q_UNUSED(triggered)
    qDebug() << "show/hide guides!";
    for (int i = 0; i < mdi->subWindowList().size(); ++i) {
        QMdiSubWindow *tab = mdi->subWindowList().at(i);
        if (!tab) { continue; }
        View *view = qobject_cast<View*>(tab->widget());
        if (!view) { continue; }
        view->showGuides(showGuidesAct->isChecked());
    }
}

void Editor::handleMagickMemoryAct(bool triggered)
{
    Q_UNUSED(triggered)
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action) { return; }
    if (action->data().toInt()>=2) {
        CyanCommon::setMemoryResource(action->data().toInt());
    }
}

void Editor::handleViewModeAct(bool triggered)
{
    qDebug() << "handle view mode change";
    Q_UNUSED(triggered)
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action) { return; }
    mdi->setViewMode(static_cast<QMdiArea::ViewMode>(action->data().toInt()));
    if (mdi->viewMode() == QMdiArea::TabbedView) { // workaround qtbug
        QTabBar *mdiBar = mdi->findChild<QTabBar*>();
        if (mdiBar) {
            mdiBar->setDrawBase(false);
        }
    }
}

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
            if (CyanImageFormat::isValidCanvas(filename)) { loadProject(filename); }
            else { readImage(filename); }
        }
#else
        if (type.name().startsWith(QString("audio")) ||
            type.name().startsWith(QString("video"))) { continue; }
        if (CyanImageFormat::isValidCanvas(filename)) { loadProject(filename); }
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
                QByteArray coverart = CyanFFmpeg::getEmbeddedCoverArt(filename);
                if (coverart.size()==0) { continue; }
                image.read(Magick::Blob(coverart.data(),
                                        static_cast<size_t>(coverart.size())));
            } else if (type.name().startsWith(QString("video"))) { // get frame from video
                image = getVideoFrameAsImage(filename);
            } else { // "regular" image
                if (CyanImageFormat::isValidCanvas(filename)) { continue; }
                image.read(filename.toStdString());
            }
#else
            if (type.name().startsWith(QString("audio")) ||
                type.name().startsWith(QString("video"))) { continue; }
            if (CyanImageFormat::isValidCanvas(filename)) {
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

void Editor::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    if (viewZoomFitAct->isChecked()) { setCurrentZoom(); }
}

void Editor::closeEvent(QCloseEvent *e)
{
    if (hasDirtyProjects()) {
        int ret = QMessageBox::question(this,
                                        tr("Unsaved projects"),
                                        tr("You have unsaved project open, are you sure you want to quit?"),
                                        QMessageBox::Yes|QMessageBox::No,
                                        QMessageBox::No);
        if (ret == QMessageBox::No) { e->ignore(); }
        else { e->accept(); }
    } else { e->accept(); }
}

