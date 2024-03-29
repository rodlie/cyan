/*
#
# Cyan Graphics - https://cyan.graphics - https://github.com/rodlie/cyan
#
# Copyright (c) Ole-André Rodlie <ole.andre.rodlie@gmail.com>. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
#
*/

#include "cyan.h"

#include <QDebug>
#include <QApplication>
#include <QStyleFactory>
#include <QFileDialog>
#include <QIcon>
#include <QShortcut>
#include <QKeySequence>
#include <QtConcurrentRun>
#include <QHeaderView>
#include <QSettings>
#include <QTimer>
#include <QMessageBox>
#include <QLabel>

#include <vector>

#include "aboutdialog.h"
#include "convertdialog.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
#define QT_SKIP_EMPTY QString::SkipEmptyParts
#else
#define QT_SKIP_EMPTY Qt::SkipEmptyParts
#endif

using namespace Cyan;

Cyan::Window::Window(QWidget *parent)
    : QMainWindow(parent)
    , _mdi(nullptr)
    , _splitter(nullptr)
    , _splitterMiddle(nullptr)
    , _splitterLeft(nullptr)
    , _splitterRight(nullptr)
    , _toolbar(nullptr)
    , _statusbar(nullptr)
    , _menubar(nullptr)
    , _menuFile(nullptr)
    , _menuView(nullptr)
    , _menuHelp(nullptr)
    , _menuColor(nullptr)
    , _menuColorRGB(nullptr)
    , _menuColorCMYK(nullptr)
    , _menuColorGRAY(nullptr)
    , _menuColorIntent(nullptr)
    , _menuColorDisplay(nullptr)
    , _menuColorPrint(nullptr)
    , _menuZoom(nullptr)
    , _menuColorBlackPoint(nullptr)
    , _menuColorRGBGroup(nullptr)
    , _menuColorCMYKGroup(nullptr)
    , _menuColorGRAYGroup(nullptr)
    , _menuColorDisplayGroup(nullptr)
    , _menuColorPrintGroup(nullptr)
    , _menuColorIntentGroup(nullptr)
    , _menuColorButton(nullptr)
    , _menuColorDisplayButton(nullptr)
    , _menuColorPrintButton(nullptr)
    , _menuZoomButton(nullptr)
    , _menuWindows(nullptr)
    , _actionOpenImage(nullptr)
    , _actionSaveImage(nullptr)
    , _actionAbout(nullptr)
    , _tabs(nullptr)
    , _tabImageDetails(nullptr)
    , _tabPrintDetails(nullptr)
    , _proxy(nullptr)
{
    setupUi();
    setupMenus();
    setupActions();

    qRegisterMetaType<Engine::Image>("Engine::Image");
    connect( this,
             SIGNAL( openImageReady(Engine::Image) ),
             this,
             SLOT( handleOpenImageReady(Engine::Image) ) );
    connect( this,
             SIGNAL( applyDisplayProfileReady(Engine::Image) ),
             this,
             SLOT( handleUpdateImageReady(Engine::Image) ) );
    connect( this,
             SIGNAL( clearDisplayProfileReady(Engine::Image) ),
             this,
             SLOT( handleUpdateImageReady(Engine::Image) ) );
    connect( this,
             SIGNAL( applyPrintProfileReady(Engine::Image) ),
             this,
             SLOT( handleUpdateImageReady(Engine::Image) ) );
    connect( this,
             SIGNAL( clearPrintProfileReady(Engine::Image) ),
             this,
             SLOT( handleUpdateImageReady(Engine::Image) ) );
    connect( this,
             SIGNAL( showStatusMessage(QString,int) ),
             _statusbar,
             SLOT( showMessage(QString,int) ) );

    QTimer::singleShot( 0,
                        this,
                        SLOT( loadSettings() ) );
}

Cyan::Window::~Window()
{
    saveSettings();
}

void
Window::openImage(bool showDialog,
                  const QString &filename)
{
    QString dialogFilename;
    if (showDialog) {
        dialogFilename = QFileDialog::getOpenFileName( this,
                                                       tr("Open image"),
                                                       getRecentLoadDirectory(),
                                                       Engine::supportedReadFormats().join(" ") );
    }
    QString filePath = showDialog ? dialogFilename : filename;
    bool isOpen = isFileOpen(filePath);
    if ( !isOpen && Engine::isValidImage(filePath) ) {
        updateRecentLoadDirectory(filePath);
        emit showStatusMessage(tr("Reading image %1 ...").arg(filePath), 0);
        auto cs = getColorSettings();
        cs.proxy = _proxy->currentData().toInt();

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QFuture f = QtConcurrent::run(
                                      &Window::readImage,
                                      this,
#else
        QtConcurrent::run(
                                      this,
                                      &Window::readImage,
#endif
                                      filePath,
                                      cs);
    } else if (isOpen) {
        MdiSubWindow *tab = getTab(filename);
        if (tab) { _mdi->setActiveSubWindow(tab); }
    }
}

void
Window::saveImage()
{
    QString filename = QFileDialog::getSaveFileName( this,
                                                     tr("Save image"),
                                                     getRecentSaveDirectory(),
                                                     Engine::supportedWriteFormats().join(" ") );
    if ( filename.isEmpty() ) { return; }
    openConvertDialog(filename);
}

void
Window::readImage(const QString &filename,
                  const Engine::ColorSettings &cs)
{
    auto image = Engine::readImage(filename,
                                   cs.profiles.rgb,
                                   cs.profiles.cmyk,
                                   cs.profiles.gray,
                                   cs.intent,
                                   cs.blackpoint,
                                   true,
                                   cs.proxy);
    emit openImageReady(image);
}

void
Window::applyDisplayProfile(const QString &filename,
                            const Engine::ColorProfiles &profiles)
{
    qDebug() << "applyDisplayProfile" << filename << profiles.source << profiles.display << profiles.print;
    if ( profiles.display.isEmpty() && profiles.print.isEmpty() ) {
        Engine::Image image;
        image.filename = filename;
        image.success = false;
        image.errors = tr("Missing display and print profile.");
        emit applyDisplayProfileReady(image);
        return;
    }

    QByteArray src = Engine::fileToByteArray(filename);
    QByteArray fallbackProfile = Engine::fileToByteArray(profiles.source);
    QByteArray displayProfile = Engine::fileToByteArray(profiles.display);

    QString printDetails;
    if ( !profiles.print.isEmpty() ) {
        auto print = Engine::convertImage(src,
                                          fallbackProfile,
                                          Engine::fileToByteArray(profiles.print),
                                          profiles.intent,
                                          profiles.blackpoint,
                                          false,
                                          false,
                                          true,
                                          true,
                                          true,
                                          profiles.display.isEmpty() ? true : false,
                                          true,
                                          Engine::ImageOptions(),
                                          profiles.proxy);
        fallbackProfile = Engine::fileToByteArray(profiles.print);
        src = print.buffer;
        printDetails = print.information;
        if ( profiles.display.isEmpty() ) {
            print.printInformation = print.information;
            print.filename = filename;
            emit applyDisplayProfileReady(print);
        }
    } else {
        fallbackProfile = Engine::fileToByteArray(profiles.source);
    }

    int proxy = profiles.proxy;
    if ( !profiles.print.isEmpty() && profiles.proxy != 100) { proxy = 100; }

    if ( !profiles.display.isEmpty() ) {
        auto image = Engine::convertImage(src,
                                          fallbackProfile,
                                          displayProfile,
                                          profiles.intent,
                                          profiles.blackpoint,
                                          false,
                                          false,
                                          true,
                                          true,
                                          true,
                                          true,
                                          false,
                                          Engine::ImageOptions(),
                                          proxy);
        image.filename = filename;
        image.printInformation = printDetails;
        emit applyDisplayProfileReady(image);
    }
}

void
Window::updateDisplayProfile()
{
    MdiSubWindow *tab = qobject_cast<MdiSubWindow*>( _mdi->currentSubWindow() );
    if (!tab) { return; }
    updateDisplayProfile( tab->getFilename(),
                          tab->getColorSpace() );
}

void
Window::updateDisplayProfile(const QString &filename,
                             const Engine::ColorSpace &colorspace)
{
    setColorActionsEnabled(false);
    emit showStatusMessage(tr("Applying display profile ..."), 0);
    auto cs = getColorSettings();
    Engine::ColorProfiles args = cs.profiles;
    args.proxy = cs.proxy; //_proxy->currentData().toInt();

    switch(colorspace) {
    case Engine::ColorSpaceRGB:
        args.source = args.rgb;
        break;
    case Engine::ColorSpaceCMYK:
        args.source = args.cmyk;
        break;
    case Engine::ColorSpaceGRAY:
        args.source = args.gray;
        break;
    default:;
    }

    args.intent = cs.intent;
    args.blackpoint = cs.blackpoint;

    if ( !canApplyDisplayProfile() ) { args.display.clear(); }
    if ( !canApplyPrintProfile() ) { args.print.clear(); }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QFuture f = QtConcurrent::run(
                                  &Window::applyDisplayProfile,
                                  this,
#else
    QtConcurrent::run(
                                  this,
                                  &Window::applyDisplayProfile,
#endif
                                  filename,
                                  args);
}

void
Window::clearDisplayProfile(const QString &filename,
                            const Engine::ColorSettings &cs)
{
    auto image = Engine::readImage(filename,
                                   cs.profiles.rgb,
                                   cs.profiles.cmyk,
                                   cs.profiles.gray,
                                   cs.intent,
                                   cs.blackpoint,
                                   false,
                                   cs.proxy);
    emit clearDisplayProfileReady(image);
}

void
Window::resetDisplayProfile()
{
    MdiSubWindow *tab = qobject_cast<MdiSubWindow*>( _mdi->currentSubWindow() );
    if (!tab) { return; }
    resetDisplayProfile( tab->getFilename() );
}

void
Window::resetDisplayProfile(const QString &filename)
{
    setColorActionsEnabled(false);
    emit showStatusMessage(tr("Clear display profile ..."), 0);
    auto cs = getColorSettings();
    //cs.proxy = _proxy->currentData().toInt();

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QFuture f = QtConcurrent::run(
                                  &Window::clearDisplayProfile,
                                  this,
#else
    QtConcurrent::run(
                                  this,
                                  &Window::clearDisplayProfile,
#endif
                                  filename,
                                  cs);
}

void
Window::setupUi()
{
 #ifdef Q_OS_DARWIN
    setWindowTitle("Cyan");
#endif

    // mdi
    _mdi = new Mdi(this);
    _mdi->setViewMode(QMdiArea::TabbedView);
    _mdi->setTabsClosable(true);
    _mdi->setTabsMovable(true);

    connect( _mdi,
             SIGNAL( dropped(QList<QUrl>) ),
             this,
             SLOT( handleDropped(QList<QUrl>) ) );
    connect( _mdi,
             SIGNAL( subWindowActivated(QMdiSubWindow*) ),
             this,
             SLOT( handleWindowActivated(QMdiSubWindow*) ) );

    // tabs
    _tabs = new QTabWidget(this);
    _tabs->setTabPosition(QTabWidget::East);

    // tab details
    _tabImageDetails = new QTreeWidget(this);
    _tabImageDetails->setColumnCount(2);
    _tabImageDetails->setHeaderHidden(true);
    _tabImageDetails->header()->setStretchLastSection(true);
    _tabImageDetails->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    _tabPrintDetails = new QTreeWidget(this);
    _tabPrintDetails->setColumnCount(2);
    _tabPrintDetails->setHeaderHidden(true);
    _tabPrintDetails->header()->setStretchLastSection(true);
    _tabPrintDetails->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    _tabPrintDetails->setVisible(false);

    _tabs->addTab( _tabImageDetails,
                   QIcon::fromTheme(CYAN_ICON_IMAGE),
                   tr("Image Details") );

    // splitters
    _splitter = new QSplitter(this);
    _splitterMiddle = new QSplitter(this);
    _splitterLeft = new QSplitter(this);
    _splitterRight = new QSplitter(this);
    _splitter->setOrientation(Qt::Horizontal);
    _splitterMiddle->setOrientation(Qt::Vertical);
    _splitterLeft->setOrientation(Qt::Vertical);
    _splitterRight->setOrientation(Qt::Vertical);
    _splitter->addWidget(_splitterLeft);
    _splitter->addWidget(_splitterMiddle);
    _splitter->addWidget(_splitterRight);
    _splitterMiddle->addWidget(_mdi);
    _splitterRight->addWidget(_tabs);

    setCentralWidget(_splitter);

    // toolbar
    _toolbar = new QToolBar(tr("Tools"), this);
    _toolbar->setObjectName("ToolsBar");
    _toolbar->setMovable(false);
    //_toolbar->setIconSize( QSize(32, 32) );
    addToolBar(Qt::TopToolBarArea, _toolbar);

    // proxy
    _proxy = new QComboBox(this);
    _proxy->setToolTip(tr("Adjust proxy"));
    _proxy->addItem(tr("100%"), 100);
    _proxy->addItem(tr("75%"), 75);
    _proxy->addItem(tr("50%"), 50);
    _proxy->addItem(tr("25%"), 25);

    // statusbar
    _statusbar = new QStatusBar(this);
    setStatusBar(_statusbar);

    // menu bar
    _menubar = new QMenuBar(this);
    setMenuBar(_menubar);

    // setup theme and icons
    setupTheme(false, false);
}

void
Window::setupMenus()
{
    _menuFile = new QMenu(tr("File"), this);
    _menuView = new QMenu(tr("View"), this);
    _menuHelp = new QMenu(tr("Help"), this);

    _menuColor = new QMenu(tr("Colors"), this);
    _menuColorRGB = new QMenu(tr("Default RGB profile"), this);
    _menuColorCMYK = new QMenu(tr("Default CMYK profile"), this);
    _menuColorGRAY = new QMenu(tr("Default GRAY profile"), this);
    _menuColorIntent = new QMenu(tr("Rendering Intent"), this);
    _menuColorDisplay = new QMenu(tr("Display profile"), this);
    _menuColorPrint = new QMenu(tr("Print profile"), this);
    _menuZoom = new QMenu(tr("Zoom"), this);

    _menuColorRGB->setIcon( QIcon::fromTheme(CYAN_ICON_COLOR_WHEEL) );
    _menuColorCMYK->setIcon( QIcon::fromTheme(CYAN_ICON_COLOR_WHEEL) );
    _menuColorGRAY->setIcon( QIcon::fromTheme(CYAN_ICON_COLOR_WHEEL) );
    _menuColorIntent->setIcon( QIcon::fromTheme(CYAN_ICON_COLOR_MAN) );
    _menuColorDisplay->setIcon( QIcon::fromTheme(CYAN_ICON_DISPLAY) );
    _menuColorPrint->setIcon( QIcon::fromTheme(CYAN_ICON_PRINTER_COLOR) );
    _menuZoom->setIcon( QIcon::fromTheme(CYAN_ICON_ZOOM) );
    _menuColor->setIcon( QIcon::fromTheme(CYAN_ICON_COLOR_WHEEL) );

    _menuColor->addMenu(_menuColorRGB);
    _menuColor->addMenu(_menuColorCMYK);
    _menuColor->addMenu(_menuColorGRAY);
    _menuColor->addMenu(_menuColorDisplay);
    _menuColor->addMenu(_menuColorPrint);
    _menuColor->addMenu(_menuColorIntent);

    _menuColorButton = new QToolButton(this);
    _menuColorButton->setPopupMode(QToolButton::InstantPopup);
    _menuColorButton->setIcon( QIcon::fromTheme(CYAN_ICON_COLOR_WHEEL) );
    _menuColorButton->setText( tr("Colors") );
    _menuColorButton->setMenu(_menuColor);

    _menuColorDisplayButton = new QToolButton(this);
    _menuColorDisplayButton->setPopupMode(QToolButton::MenuButtonPopup);
    _menuColorDisplayButton->setCheckable(true);
    _menuColorDisplayButton->setIcon( QIcon::fromTheme(CYAN_ICON_DISPLAY) );
    _menuColorDisplayButton->setText( tr("Display") );
    _menuColorDisplayButton->setToolTip( tr("Apply display color profile") );
    _menuColorDisplayButton->setStyleSheet( QString("QToolButton:checked { background-color: rgb(95.4, 105.7, 121.1); }") );
    _menuColorDisplayButton->setMenu(_menuColorDisplay);
    connect( _menuColorDisplayButton,
             SIGNAL( toggled(bool) ),
             SLOT( handleColorDisplayButtonTriggered(bool) ) );

    _menuColorPrintButton = new QToolButton(this);
    _menuColorPrintButton->setPopupMode(QToolButton::MenuButtonPopup);
    _menuColorPrintButton->setCheckable(true);
    _menuColorPrintButton->setIcon( QIcon::fromTheme(CYAN_ICON_PRINTER_COLOR) );
    _menuColorPrintButton->setText( tr("Printer") );
    _menuColorPrintButton->setToolTip( tr("Apply printer color profile") );
    _menuColorPrintButton->setStyleSheet( QString("QToolButton:checked { background-color: rgb(95.4, 105.7, 121.1); }") );
    _menuColorPrintButton->setMenu(_menuColorPrint);
    connect( _menuColorPrintButton,
             SIGNAL( toggled(bool) ),
             SLOT( handleColorPrintButtonTriggered(bool) ) );

    /*_menuZoomButton = new QToolButton(this);
    _menuZoomButton->setPopupMode(QToolButton::InstantPopup);
    _menuZoomButton->setCheckable(false);
    _menuZoomButton->setIcon( QIcon::fromTheme(CYAN_ICON_ZOOM) );
    _menuZoomButton->setText( tr("Zoom") );
    _menuZoomButton->setMenu(_menuZoom);*/

    _menuView->addMenu(_menuColor);
    _menuView->addMenu(_menuZoom);

    _menuWindows = new QtWindowListMenu(this);
    _menuWindows->attachToMdiArea(_mdi);

    _menubar->addMenu(_menuFile);
    _menubar->addMenu(_menuView);
    _menubar->addMenu(_menuWindows);
    _menubar->addMenu(_menuHelp);
}

void
Window::setupActions()
{
    // open image
    _actionOpenImage = new QAction(QIcon::fromTheme(CYAN_ICON_OPEN_IMAGE),
                                   tr("Open image"),
                                   this);
    _actionOpenImage->setShortcut( QKeySequence( tr("Ctrl+O") ) );
    connect( _actionOpenImage,
             SIGNAL( triggered() ),
             this,
             SLOT( handleActionOpenImage() ) );
    _menuFile->addAction(_actionOpenImage);

    // save image
    _actionSaveImage = new QAction(QIcon::fromTheme(CYAN_ICON_SAVE_IMAGE),
                                   tr("Save image"),
                                   this);
    _actionSaveImage->setShortcut( QKeySequence( tr("Ctrl+S") ) );
    _actionSaveImage->setDisabled(true);
    connect( _actionSaveImage,
             SIGNAL( triggered() ),
             this,
             SLOT( saveImage() ) );
    _menuFile->addAction(_actionSaveImage);

    // about
    _actionAbout = new QAction(QIcon::fromTheme(CYAN_ICON),
                               tr("About"),
                               this);
    connect( _actionAbout,
             SIGNAL( triggered() ),
             this,
             SLOT( openAboutDialog() ) );
    _menuHelp->addAction(_actionAbout);

    // toolbar spacer
    QWidget *toolbarSpacer = new QWidget(this);
    toolbarSpacer->setSizePolicy(QSizePolicy::Expanding,
                                 QSizePolicy::Expanding);

    // toolbar
    _toolbar->addAction(_actionOpenImage);
    _toolbar->addAction(_actionSaveImage);
    _toolbar->addWidget(_menuColorButton);
    _toolbar->addWidget(toolbarSpacer);
    _toolbar->addWidget(_menuColorDisplayButton);
    _toolbar->addWidget(_menuColorPrintButton);
    //_toolbar->addWidget(_menuZoomButton);

    // proxy
    connect( _proxy,
             SIGNAL( currentIndexChanged(int) ),
             this,
             SLOT( handleProxyChanged(int) ) );

    QLabel *proxyLabel = new QLabel(tr("Proxy"), this);
    proxyLabel->setToolTip( tr("Adjust proxy. Lower values will increase performance.") );
    _statusbar->addPermanentWidget(proxyLabel);
    _statusbar->addPermanentWidget(_proxy);

    // default profiles
    _menuColorRGBGroup = new QActionGroup(this);
    _menuColorCMYKGroup = new QActionGroup(this);
    _menuColorGRAYGroup = new QActionGroup(this);
    _menuColorDisplayGroup = new QActionGroup(this);
    _menuColorPrintGroup = new QActionGroup(this);
    _menuColorIntentGroup = new QActionGroup(this);

    _menuColorRGB->addActions( _menuColorRGBGroup->actions() );
    _menuColorCMYK->addActions( _menuColorCMYKGroup->actions() );
    _menuColorGRAY->addActions( _menuColorGRAYGroup->actions() );
    _menuColorDisplay->addActions( _menuColorDisplayGroup->actions() );
    _menuColorPrint->addActions( _menuColorPrintGroup->actions() );

    // default blackpoint
    _menuColorBlackPoint = new QAction(tr("Black point compensation"), this);
    _menuColorBlackPoint->setCheckable(true);
    _menuColor->addAction(_menuColorBlackPoint);
    connect( _menuColorBlackPoint,
             SIGNAL( triggered() ),
             this,
             SLOT( handleColorBlackPointTriggered() ) );

    // populate default profiles
    populateColorProfileMenu(_menuColorRGB,
                             _menuColorRGBGroup,
                             Engine::ColorSpaceRGB);
    populateColorProfileMenu(_menuColorCMYK,
                             _menuColorCMYKGroup,
                             Engine::ColorSpaceCMYK);
    populateColorProfileMenu(_menuColorGRAY,
                             _menuColorGRAYGroup,
                             Engine::ColorSpaceGRAY);
    populateColorProfileMenu(_menuColorDisplay,
                             _menuColorDisplayGroup,
                             Engine::ColorSpaceRGB,
                             true);
    populateColorProfileMenu(_menuColorPrint,
                             _menuColorPrintGroup,
                             Engine::ColorSpaceCMYK,
                             false,
                             true);

    populateColorIntentMenu();
}

void
Window::setupTheme(bool native,
                   bool nativeIcons)
{
    if (native) {
        if ( QIcon::themeName().isEmpty() ) { QIcon::setThemeName("hicolor"); }
        return;
    }

    // set qt style (force fusion)
    qApp->setStyle( QStyleFactory::create("fusion") );

    // set icon theme (force hicolor)
    if ( !nativeIcons || QIcon::themeName().isEmpty() ) { QIcon::setThemeName("hicolor"); }

    // set (dark) colors
    QPalette palette;
    palette.setColor( QPalette::Window, QColor(53, 53, 53) );
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor( QPalette::Base, QColor(15, 15, 15) );
    palette.setColor( QPalette::AlternateBase, QColor(53, 53, 53) );
    palette.setColor(QPalette::Link, Qt::white);
    palette.setColor(QPalette::LinkVisited, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor( QPalette::Button, QColor(53, 53, 53) );
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor( QPalette::Highlight, QColor(33, 122, 196) );
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    qApp->setPalette(palette);

    _mdi->setBackground( QBrush( QColor(20, 20, 20) ) );

    setStyleSheet( QString("QMenu::separator { background-color: rgb(53, 53, 53); height: 1px; }"
                           "QToolBar { border-color: none; }"
                           "QToolButton::menu-indicator { image: none; }") );

    setWindowIcon( QIcon::fromTheme(CYAN_ICON) );
}

void
Window::handleDropped(const QList<QUrl> &urls)
{
    for (auto &url : urls) {
        QFileInfo info( url.toLocalFile() );
        if ( info.isDir() ) {
            QDir dir( info.absoluteFilePath() );
            QStringList images = dir.entryList(Engine::supportedReadFormats(),
                                               QDir::Files);
            QList<QUrl> newUrls;
            for (auto &image : images) { newUrls.append( QUrl::fromLocalFile( QString("%1/%2")
                                                                              .arg(info.absoluteFilePath(),
                                                                                   image) ) ); }
            handleDropped(newUrls);
            continue;
        }
        openImage( false, url.toLocalFile() );
    }
}

void
Window::handleActionOpenImage()
{
    openImage();
}

void
Window::populateColorProfileMenu(QMenu *menu,
                                 QActionGroup *group,
                                 Engine::ColorSpace colorspace,
                                 bool isDisplay,
                                 bool isPrint)
{
    if (!menu || !group) { return; }
    QString fallback;
    switch(colorspace) {
    case Engine::ColorSpaceRGB:
        fallback = CYAN_PROFILE_FALLBACK_RGB;
        break;
    case Engine::ColorSpaceCMYK:
        fallback = CYAN_PROFILE_FALLBACK_CMYK;
        break;
    case Engine::ColorSpaceGRAY:
        fallback = CYAN_PROFILE_FALLBACK_GRAY;
        break;
    default:;
    }
    QMapIterator<QString, QString> i( Engine::getProfiles(colorspace,
                                                          false,
                                                          fallback,
                                                          true) );
    while ( i.hasNext() ) {
        i.next();
        QAction *action = new QAction(menu);
        action->setText( i.key() );
        action->setData( i.value() );
        action->setCheckable(true);
        group->addAction(action);
        if (isDisplay) {
            connect( action,
                     SIGNAL( triggered() ),
                     this,
                     SLOT( handleColorProfileDisplayTriggered() ) );
        } else if (isPrint) {
            connect( action,
                     SIGNAL( triggered() ),
                     this,
                     SLOT( handleColorProfilePrintTriggered() ) );
        } else {
            connect( action,
                     SIGNAL( triggered() ),
                     this,
                     SLOT( handleColorProfileTriggered() ) );
        }
    }
    menu->clear();
    menu->addActions( group->actions() );
}

void
Window::populateColorIntentMenu()
{
    std::vector<Engine::RenderingIntent> intents;
    intents.push_back(Engine::UndefinedRenderingIntent);
    intents.push_back(Engine::SaturationRenderingIntent);
    intents.push_back(Engine::PerceptualRenderingIntent);
    intents.push_back(Engine::AbsoluteRenderingIntent);
    intents.push_back(Engine::RelativeRenderingIntent);

    for (auto &intent : intents) {
        QAction *action = new QAction(_menuColorIntent);
        switch (intent) {
        case Engine::SaturationRenderingIntent:
            action->setText( tr("Saturation") );
            break;
        case Engine::PerceptualRenderingIntent:
            action->setText( tr("Perceptual") );
            break;
        case Engine::AbsoluteRenderingIntent:
            action->setText( tr("Absolute") );
            break;
        case Engine::RelativeRenderingIntent:
            action->setText( tr("Relative") );
            break;
        default:
            action->setText( tr("Undefined") );
        }
        action->setData(intent);
        action->setCheckable(true);
        connect( action,
                 SIGNAL( triggered() ),
                 this,
                 SLOT( handleColorIntentTriggered() ) );
        _menuColorIntentGroup->addAction(action);
    }
    _menuColorIntent->addActions( _menuColorIntentGroup->actions() );
}

void
Window::handleColorProfileTriggered()
{
    saveColorSettings(true);
}

void
Window::handleColorProfileDisplayTriggered()
{
    saveColorSettings(true);
    if ( canApplyDisplayProfile() ) { updateDisplayProfile(); }
}

void
Window::handleColorProfilePrintTriggered()
{
    saveColorSettings(true);
    if ( canApplyPrintProfile() ) { updateDisplayProfile(); }
}

void
Window::handleColorIntentTriggered()
{
    if ( canApplyDisplayProfile() || canApplyPrintProfile() ) { updateDisplayProfile(); }
    saveColorSettings(true);
}

void
Window::handleColorBlackPointTriggered()
{
    if ( canApplyDisplayProfile() || canApplyPrintProfile() ) { updateDisplayProfile(); }
    saveColorSettings(true);
}

void
Window::handleColorDisplayButtonTriggered(bool checked)
{
    if ( checked && !canApplyDisplayProfile() ) {
        QMessageBox::warning( this,
                              tr("No display profile"),
                              tr("Select a display profile from the color settings.") );
        _menuColorDisplayButton->setChecked(false);
        return;
    }
    saveColorSettings(true);
    if ( checked && canApplyDisplayProfile() ) { updateDisplayProfile(); }
    else if (!checked) {
        if ( canApplyPrintProfile() ) { updateDisplayProfile(); }
        else { resetDisplayProfile(); }
    }
}

void
Window::handleColorPrintButtonTriggered(bool checked)
{
    if ( checked && !canApplyPrintProfile() ) {
        QMessageBox::warning( this,
                              tr("No print profile"),
                              tr("Select a print profile from the color settings.") );
        _menuColorPrintButton->setChecked(false);
        return;
    }
    saveColorSettings(true);
    if ( checked && canApplyPrintProfile() ) { updateDisplayProfile(); }
    else if (!checked) {
        if ( canApplyDisplayProfile() ) { updateDisplayProfile(); }
        else { resetDisplayProfile(); }
    }
    setPrintDetailsVisibility(checked);
}

bool
Window::isFileOpen(const QString &filename)
{
    for (int i = 0; i < _mdi->subWindowList().size(); ++i) {
        MdiSubWindow *tab = qobject_cast<MdiSubWindow*>( _mdi->subWindowList().at(i) );
        if (tab && tab->getFilename() == filename) { return true; }
    }
    return false;
}

MdiSubWindow *
Window::getTab(const QString &filename)
{
    for (int i = 0; i < _mdi->subWindowList().size(); ++i) {
        MdiSubWindow *tab = qobject_cast<MdiSubWindow*>( _mdi->subWindowList().at(i) );
        if (tab && tab->getFilename() == filename) { return tab; }
    }
    return nullptr;
}

MdiSubWindow *
Window::getCurrentTab()
{
    return qobject_cast<MdiSubWindow*>( _mdi->currentSubWindow() );
}

void
Window::handleOpenImageReady(const Engine::Image &image)
{
    if ( isFileOpen(image.filename) ) { return; }

    if (!image.success || image.buffer.length() <= 0 || image.width <= 0 || image.height <= 0) {
        emit showStatusMessage(tr("Failed"), 500);
        QFileInfo info(image.filename);
        QMessageBox::warning( this,
                              tr("Failed"),
                              tr("Failed to read image %1.\n\n%2").arg(info.baseName(),
                                                                       image.errors) );
        return;
    }

    auto cs = getColorSettings();
    cs.colorspace = image.colorspace;
    //cs.proxy = _proxy->currentData().toInt();

    bool isLastTabMaximized = true;
    if ( getTab(_lastTab) && !getTab(_lastTab)->isMaximized() ) { isLastTabMaximized = false; }

    MdiSubWindow *tab = new MdiSubWindow(_mdi,
                                         image.filename,
                                         cs);
    tab->setAttribute(Qt::WA_DeleteOnClose);
    tab->setWindowIcon( QIcon::fromTheme(CYAN_ICON_SUBWINDOW) );
    tab->getView()->setImage(image);

    connect( tab->getView(),
             SIGNAL( dropped(QList<QUrl>) ),
             this,
             SLOT( handleDropped(QList<QUrl>) ) );
    connect( tab,
             SIGNAL( closed(QString) ),
             this,
             SLOT( handleClosedWindow(QString) ) );
    if (isLastTabMaximized) { tab->showMaximized(); }
    else {
        tab->setMinimumSize( QSize(320, 256) );
        tab->showNormal();
    }

    emit showStatusMessage(tr("Done"), 500);

    if ( canApplyDisplayProfile() || canApplyPrintProfile() ) { updateDisplayProfile(image.filename,
                                                                                     image.colorspace); }
}

void
Window::handleUpdateImageReady(const Engine::Image &image)
{
    setColorActionsEnabled(true);
    if ( !isFileOpen(image.filename) ) { return; }

    if (!image.success || image.buffer.length() <= 0 || image.width <= 0 || image.height <= 0) {
        emit showStatusMessage(tr("Failed"), 500);
        QFileInfo info(image.filename);
        QMessageBox::warning( this,
                              tr("Failed"),
                              tr("Failed to process %1.\n\n%2").arg(info.baseName(),
                                                                    image.errors) );
        return;
    }

    emit showStatusMessage(tr("Done"), 500);

    MdiSubWindow *tab = getTab(image.filename);
    if (!tab) { return; }
    tab->getView()->setImage(image, false, false);
    tab->getView()->setPrintDetails(image.printInformation);

    if ( tab == qobject_cast<MdiSubWindow*>( _mdi->currentSubWindow() ) ) {
        qDebug() << "update print details";
        setDetails(_tabPrintDetails, image.printInformation);
    }
    if ( colorSettingsDiffer(tab->getColorSettings(), false, true) ) {
        tab->setColorSettings( getColorSettings() );
    }
}

void
Window::handleWindowActivated(QMdiSubWindow *window)
{
    MdiSubWindow *tab = qobject_cast<MdiSubWindow*>(window);
    if (!tab) {
        _actionSaveImage->setEnabled(false);
        return;
    }
    _actionSaveImage->setEnabled(true);
    qDebug() << "handle window activated" << tab->getFilename() << _lastTab;
    if (tab->getFilename() == _lastTab) { return; }
    _lastTab = tab->getFilename();

    setDetails( _tabImageDetails, tab->getView()->getSourceDetails() );
    setDetails( _tabPrintDetails, tab->getView()->getPrintDetails() );

    if ( colorSettingsDiffer(tab->getColorSettings(), false, true, true) ) {
        if ( canApplyDisplayProfile() || canApplyPrintProfile() ) {
            updateDisplayProfile( tab->getFilename(),
                                  tab->getColorSpace() );
        } else if ( tab->getColorSettings().applyDisplayProfile ||
                    tab->getColorSettings().applyPrintProfile) {
            clearDisplayProfile( tab->getFilename(), getColorSettings() );
        }
    }
}

// TODO: cleanup
void
Window::setDetails(QTreeWidget *tree,
                   const QString &info)
{
    if (!tree) { return; }
    tree->clear();
    if ( info.isEmpty() ) { return; }
    QStringList list = info.split("\n", QT_SKIP_EMPTY);
    QVector<QTreeWidgetItem*> level1items;
    QVector<QTreeWidgetItem*> level2items;
    QString level1 = "  ";
    QString level2 = "    ";
    QString level3 = "      ";
    bool foundHistogramTag = false;
    for (int i = 0; i < list.size(); ++i) {
        QString item = list.at(i);
        if ( item.startsWith(level1) ) {
            QString section1 = item.section(":", 0, 0).trimmed();
            QString section2 = item.section(":", 1).trimmed();
            if ( item.startsWith("  Pixels per second:") ||
                 item.startsWith("  User time:") ||
                 item.startsWith("  Elapsed time:") ||
                 item.startsWith("  Version: Image") ||
                 item.startsWith("  Format: ") ||
                 item.startsWith("  Class: ") ||
                 item.startsWith("  Base filename:") ||
                 item.startsWith("  Mime type:") ||
                 item.startsWith("  Tainted") ||
                 item.startsWith("  Filesize") ||
                 item.contains("Filename") )
            {
                continue;
            }
            if (item == "  Histogram:") { foundHistogramTag = true; }
            if ( foundHistogramTag && ( item.startsWith("  Rendering intent:") ||
                                        item.startsWith("  Gamma:") ) )
            {
                foundHistogramTag = false;
            }
            if (foundHistogramTag) { continue; }
            QTreeWidgetItem *levelItem = new QTreeWidgetItem();
            levelItem->setText(0, section1);
            levelItem->setText(1, section2);
            if ( item.startsWith(level3) ) {
                int parentID = level2items.size() - 1;
                if (parentID < 0) { parentID = 0; }
                QTreeWidgetItem *parentItem = level2items.at(parentID);
                if (parentItem) { parentItem->addChild(levelItem); }
            }
            else if ( item.startsWith(level2) ) {
                int parentID = level1items.size() - 1;
                if (parentID < 0) { parentID = 0; }
                QTreeWidgetItem *parentItem = level1items.at(parentID);
                if (parentItem) {
                    parentItem->addChild(levelItem);
                    level2items << levelItem;
                }
            }
            else if ( item.startsWith(level1) ) { level1items << levelItem; }
            else { delete levelItem; }
            continue;
        }
    }
    tree->addTopLevelItems( level1items.toList() );
    level2items.clear();
    tree->expandAll();
}

void
Window::handleClosedWindow(const QString &filename)
{
    if (_lastTab == filename) {
        _tabImageDetails->clear();
        _tabPrintDetails->clear();
    }
}

void
Window::setDefaultColorProfile(const Engine::ColorSpace &colorspace,
                               const QString &filename,
                               bool isDisplay,
                               bool isPrint)
{
    QSettings settings;
    settings.beginGroup("color_settings");
    QString key;
    switch(colorspace) {
    case Engine::ColorSpaceRGB:
        key = "profile_rgb";
        break;
    case Engine::ColorSpaceCMYK:
        key = "profile_cmyk";
        break;
    case Engine::ColorSpaceGRAY:
        key = "profile_gray";
        break;
    default:;
    }
    if (isDisplay) { key = "profile_display"; }
    else if (isPrint) { key = "profile_print"; }
    qDebug() << "set default color profile" << key << filename;
    settings.setValue(key, filename);
    settings.endGroup();
}

void
Window::loadColorSettings()
{
    auto cs = getColorSettings();

    for (int i = 0; i < _proxy->count() ; ++i) {
        if (_proxy->itemData(i).toInt() == cs.proxy) {
            _proxy->setCurrentIndex(i);
            break;
        }
    }

    _menuColorBlackPoint->setChecked(cs.blackpoint);

    for ( auto &profile : _menuColorRGB->actions() ) {
        if (profile->data().toString() == cs.profiles.rgb) {
            profile->setChecked(true);
            break;
        }
    }
    for ( auto &profile : _menuColorDisplay->actions() ) {
        if (profile->data().toString() == cs.profiles.display) {
            profile->setChecked(true);
            break;
        }
    }
    for ( auto &profile : _menuColorCMYK->actions() ) {
        if (profile->data().toString() == cs.profiles.cmyk) {
            profile->setChecked(true);
            break;
        }
    }
    for ( auto &profile : _menuColorPrint->actions() ) {
        if (profile->data().toString() == cs.profiles.print) {
            profile->setChecked(true);
            break;
        }
    }
    for ( auto &profile : _menuColorGRAY->actions() ) {
        if (profile->data().toString() == cs.profiles.gray) {
            profile->setChecked(true);
            break;
        }
    }
    for ( auto &intent : _menuColorIntent->actions() ) {
        if (intent->data().toInt() == cs.intent) {
            intent->setChecked(true);
            break;
        }
    }
    if (cs.applyDisplayProfile) {
        _menuColorDisplayButton->setChecked(true);
        _menuColorDisplayButton->setChecked( canApplyDisplayProfile() );
    }
    if (cs.applyPrintProfile) {
        _menuColorPrintButton->setChecked(true);
        _menuColorPrintButton->setChecked( canApplyPrintProfile() );
    }
}

void
Window::saveColorSettings(bool forceSync)
{
    QSettings settings;
    settings.beginGroup("color_settings");
    if ( _menuColorRGBGroup->checkedAction() ) {
        settings.setValue( "profile_rgb", _menuColorRGBGroup->checkedAction()->data().toString() );
    }
    if ( _menuColorDisplayGroup->checkedAction() ) {
        settings.setValue( "profile_display", _menuColorDisplayGroup->checkedAction()->data().toString() );
    }
    if ( _menuColorCMYKGroup->checkedAction() ) {
        settings.setValue( "profile_cmyk", _menuColorCMYKGroup->checkedAction()->data().toString() );
    }
    if ( _menuColorPrintGroup->checkedAction() ) {
        settings.setValue( "profile_print", _menuColorPrintGroup->checkedAction()->data().toString() );
    }
    if ( _menuColorGRAYGroup->checkedAction() ) {
        settings.setValue( "profile_gray", _menuColorGRAYGroup->checkedAction()->data().toString() );
    }
    if ( _menuColorIntentGroup->checkedAction() ) {
        settings.setValue( "intent", _menuColorIntentGroup->checkedAction()->data().toInt() );
    }
    settings.setValue( "black_point", _menuColorBlackPoint->isChecked() );
    settings.setValue( "apply_display_profile", _menuColorDisplayButton->isChecked() );
    settings.setValue( "apply_print_profile", _menuColorPrintButton->isChecked() );
    settings.setValue( "proxy", _proxy->currentData().toInt() );
    settings.endGroup();
    if (forceSync) { settings.sync(); }
}

const Engine::ColorSettings
Window::getColorSettings()
{
    Engine::ColorSettings cs;

    QSettings settings;
    settings.beginGroup("color_settings");
    cs.profiles.rgb = settings.value("profile_rgb", CYAN_PROFILE_FALLBACK_RGB).toString();
    cs.profiles.cmyk = settings.value("profile_cmyk", CYAN_PROFILE_FALLBACK_CMYK).toString();
    cs.profiles.gray = settings.value("profile_gray", CYAN_PROFILE_FALLBACK_GRAY).toString();
    cs.profiles.display = settings.value("profile_display", CYAN_PROFILE_FALLBACK_RGB).toString();
    cs.profiles.print = settings.value("profile_print", CYAN_PROFILE_FALLBACK_CMYK).toString();
    cs.intent = static_cast<Engine::RenderingIntent>( settings.value("intent",
                                                                     Engine::RenderingIntent::PerceptualRenderingIntent).toInt() );
    cs.blackpoint = settings.value("black_point", true).toBool();
    cs.applyDisplayProfile = settings.value("apply_display_profile", false).toBool();
    cs.applyPrintProfile = settings.value("apply_print_profile", false).toBool();
    cs.proxy = settings.value("proxy", 100).toInt();
    settings.endGroup();

    return cs;
}

void
Window::loadUISettings()
{
    QSettings settings;
    settings.beginGroup("ui_settings");
    restoreState( settings.value("window_state").toByteArray() );
    restoreGeometry( settings.value("window_geometry").toByteArray() );
    _splitter->restoreState( settings.value("splitter_state").toByteArray() );
    _splitterLeft->restoreState( settings.value("splitter_left_state").toByteArray() );
    _splitterRight->restoreState( settings.value("splitter_right_state").toByteArray() );
    _splitterMiddle->restoreState( settings.value("splitter_middle_state").toByteArray() );
    bool maximized = settings.value("window_maximized", false).toBool();
    settings.endGroup();
    if (maximized) { showMaximized(); }
}

void
Window::saveUISettings()
{
    QSettings settings;
    settings.beginGroup("ui_settings");
    settings.setValue( "window_state", saveState() );
    settings.setValue( "window_geometry", saveGeometry() );
    settings.setValue( "window_maximized", isMaximized() );
    settings.setValue( "splitter_state", _splitter->saveState() );
    settings.setValue( "splitter_left_state", _splitterLeft->saveState() );
    settings.setValue( "splitter_right_state", _splitterRight->saveState() );
    settings.setValue( "splitter_middle_state", _splitterMiddle->saveState() );
    settings.endGroup();
}

void
Window::loadSettings()
{
    loadUISettings();
    loadColorSettings();
}

void
Window::saveSettings()
{
    saveUISettings();
    // color settings are saved when changed
}

bool
Window::canApplyDisplayProfile()
{
    QAction *action = _menuColorDisplayGroup->checkedAction();
    if (!action) { return false; }
    QString filename = action->data().toString();
    if ( !filename.isEmpty() && _menuColorDisplayButton->isChecked() ) { return true; }
    return false;
}

bool
Window::canApplyPrintProfile()
{
    QAction *action = _menuColorPrintGroup->checkedAction();
    if (!action) {
        setPrintDetailsVisibility(false);
        return false;
    }
    QString filename = action->data().toString();
    bool canApply = false;
    if ( !filename.isEmpty() && _menuColorPrintButton->isChecked() ) { canApply = true; }
    setPrintDetailsVisibility(canApply);
    return canApply;
}

void
Window::setPrintDetailsVisibility(bool visible)
{
    _tabPrintDetails->setVisible(visible);
    int index = _tabs->indexOf(_tabPrintDetails);
    qDebug() << "setPrintDetailsVisibility" << visible << index;
    if (visible && index == -1) {
        _tabs->addTab( _tabPrintDetails,
                       QIcon::fromTheme(CYAN_ICON_PRINTER_COLOR),
                       tr("Print Details") );
    }
    else if (!visible && index >= 0) { _tabs->removeTab(index); }
}

bool
Window::colorSettingsDiffer(const Engine::ColorSettings &cs,
                            bool checkColorspace,
                            bool checkDisplay,
                            bool checkPrint)
{
    auto config = getColorSettings();
    if (config.proxy != cs.proxy) {
        qDebug() << "====> proxy" << config.proxy << cs.proxy;
        return true;
    }
    if (config.blackpoint != cs.blackpoint) {
        qDebug() << "====> blackpoint" << config.blackpoint << cs.blackpoint;
        return true;
    }
    if (config.intent != cs.intent) {
        qDebug() << "====> intent" << config.intent << cs.intent;
        return true;
    }
    if (config.profiles.rgb != cs.profiles.rgb) {
        qDebug() << "====> rgb" << config.profiles.rgb << cs.profiles.rgb;
        return true;
    }
    if (config.profiles.cmyk != cs.profiles.cmyk) {
        qDebug() << "====> cmyk" << config.profiles.cmyk << cs.profiles.cmyk;
        return true;
    }
    if (config.profiles.gray != cs.profiles.gray) {
        qDebug() << "====> gray" << config.profiles.gray << cs.profiles.gray;
        return true;
    }
    if (checkColorspace) {
        if (config.colorspace != cs.colorspace) {
            qDebug() << "====> colorspace" << config.colorspace << cs.colorspace;
            return true;
        }
    }
    if (checkDisplay) {
        if (config.applyDisplayProfile != cs.applyDisplayProfile) {
            qDebug() << "====> apply display" << config.applyDisplayProfile << cs.applyDisplayProfile;
            return true;
        }
        if (config.profiles.display != cs.profiles.display) {
            qDebug() << "====> display" << config.profiles.display << cs.profiles.display;
            return true;
        }
    }
    if (checkPrint) {
        if (config.applyPrintProfile != cs.applyPrintProfile) {
            qDebug() << "====> apply print" << config.applyPrintProfile << cs.applyPrintProfile;
            return true;
        }
        if (config.profiles.print != cs.profiles.print) {
            qDebug() << "====> print" << config.profiles.print << cs.profiles.print;
            return true;
        }
    }
    return false;
}

void
Window::setColorActionsEnabled(bool enabled)
{
    _menuColor->setEnabled(enabled);
    _menuColorButton->setEnabled(enabled);
    _menuColorDisplayButton->setEnabled(enabled);
    _menuColorPrintButton->setEnabled(enabled);
}

void
Window::openAboutDialog()
{
    AboutDialog dialog;
    dialog.exec();
}

void
Window::openConvertDialog(const QString &filename)
{
    MdiSubWindow* tab = getCurrentTab();
    if (!tab) { return; }

    ConvertDialog dialog( this,
                          tab->getColorSettings(),
                          tab->getFilename(),
                          filename,
                          tr("Save to file") );
    dialog.exec();
    updateRecentSaveDirectory(filename);
}

void
Window::handleProxyChanged(int index)
{
    Q_UNUSED(index)
    saveColorSettings();
    updateDisplayProfile();
}

void
Window::updateRecentLoadDirectory(const QString &path)
{
   updateConfigPath(QString::fromUtf8("files"),
                    QString::fromUtf8("lastLoadDir"),
                    path);
}

const QString
Window::getRecentLoadDirectory()
{
    return getConfig( QString::fromUtf8("files"),
                      QString::fromUtf8("lastLoadDir"),
                      QDir::homePath() );
}

void
Window::updateRecentSaveDirectory(const QString &path)
{
    updateConfigPath(QString::fromUtf8("files"),
                     QString::fromUtf8("lastSaveDir"),
                     path);
}

const QString
Window::getRecentSaveDirectory()
{
    return getConfig( QString::fromUtf8("files"),
                      QString::fromUtf8("lastSaveDir"),
                      QDir::homePath() );
}

void
Window::updateConfigPath(const QString &grp,
                         const QString &key,
                         const QString &val)
{
    QFileInfo info(val);
    if ( info.absoluteDir().absolutePath() == getConfig( grp,
                                                         key,
                                                         QDir::homePath() ) ) { return; }
    QSettings settings;
    settings.beginGroup(grp);
    settings.setValue( key,
                       info.absoluteDir().absolutePath() );
    settings.endGroup();
}

const QString
Window::getConfig(const QString &grp,
                  const QString &key,
                  const QString &def)
{
    QString value;
    QSettings settings;
    settings.beginGroup(grp);
    value = settings.value(key, def).toString();
    settings.endGroup();
    return value;
}
