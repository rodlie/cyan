/*
#
# Cyan - https://github.com/rodlie/cyan
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

#include <vector>

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
    , _menuColorBlackPoint(nullptr)
    , _menuColorRGBGroup(nullptr)
    , _menuColorCMYKGroup(nullptr)
    , _menuColorGRAYGroup(nullptr)
    , _menuColorDisplayGroup(nullptr)
    , _menuColorButton(nullptr)
    , _menuColorDisplayButton(nullptr)
    , _menuWindows(nullptr)
    , _actionOpenImage(nullptr)
    , _tabs(nullptr)
    , _tabDetails(nullptr)
{
    setupUi();
    setupMenus();
    setupActions();

    qRegisterMetaType<Engine::Image>("Engine::Image");
    connect( this,
             SIGNAL( openImageReady(Engine::Image) ),
             this,
             SLOT( handleOpenImageReady(Engine::Image) ) );
}

Cyan::Window::~Window()
{

}

void
Window::openImage(bool showDialog,
                  const QString &filename)
{
    QString dialogFilename;
    if (showDialog) {
        dialogFilename = QFileDialog::getOpenFileName( this,
                                                       tr("Open image"),
                                                       QDir::homePath(),
                                                       Engine::supportedReadFormats().join(" ") );
    }
    QString filePath = showDialog ? dialogFilename : filename;
    bool isOpen = isFileOpen(filePath);
    if ( !isOpen && Engine::isValidImage(filePath) ) {
        QtConcurrent::run(this, &Window::readImage, filePath);
    } else if (isOpen) {
        MdiSubWindow *tab = getTab(filename);
        if (tab) { _mdi->setActiveSubWindow(tab); }
    }
}

void
Window::readImage(const QString &filename)
{
    // TODO: get settings
    Engine::Image image = Engine::readImage(filename,
                                            "",
                                            "",
                                            "",
                                            Engine::PerceptualRenderingIntent,
                                            true,
                                            true);
    emit openImageReady(image);
}

void
Window::setupUi()
{
    // mdi
    _mdi = new Mdi(this);
#ifdef Q_OS_DARWIN
    _mdi->setViewMode(QMdiArea::TabbedView);
    _mdi->setTabsClosable(true);
    _mdi->setTabsMovable(true);
#endif
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
    _tabDetails = new QTreeWidget(this);
    _tabDetails->setColumnCount(2);
    _tabDetails->setHeaderHidden(true);
    _tabDetails->header()->setStretchLastSection(true);
    _tabDetails->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    _tabs->addTab( _tabDetails, tr("Image Details") ); //TODO: icon

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
    _toolbar->setMovable(false);
    //_toolbar->setIconSize( QSize(32, 32) );
    addToolBar(Qt::TopToolBarArea, _toolbar);

    // statusbar
    _statusbar = new QStatusBar(this);
    setStatusBar(_statusbar);

    // menu bar
    _menubar = new QMenuBar(this);
    setMenuBar(_menubar);

    // setup theme and icons
    setupTheme(false, true); // TODO: change to false when we have new icons
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

    _menuColorRGB->setIcon( QIcon::fromTheme(CYAN_ICON_COLOR_WHEEL) );
    _menuColorCMYK->setIcon( QIcon::fromTheme(CYAN_ICON_COLOR_WHEEL) );
    _menuColorGRAY->setIcon( QIcon::fromTheme(CYAN_ICON_COLOR_WHEEL) );
    //_menuColorIntent->setIcon( QIcon::fromTheme(CYAN_ICON_COLOR_WHEEL) );
    _menuColorDisplay->setIcon( QIcon::fromTheme(CYAN_ICON_COLOR_WHEEL) );
    _menuColor->setIcon( QIcon::fromTheme(CYAN_ICON_COLOR_WHEEL) );

    _menuColor->addMenu(_menuColorRGB);
    _menuColor->addMenu(_menuColorCMYK);
    _menuColor->addMenu(_menuColorGRAY);
    _menuColor->addMenu(_menuColorDisplay);
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
    _menuColorDisplayButton->setMenu(_menuColorDisplay);

    _menuView->addMenu(_menuColor);

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

    // toolbar
    _toolbar->addAction(_actionOpenImage);
    _toolbar->addSeparator();
    _toolbar->addWidget(_menuColorButton);
    _toolbar->addSeparator();
    _toolbar->addWidget(_menuColorDisplayButton);

    // default profiles
    _menuColorRGBGroup = new QActionGroup(this);
    _menuColorCMYKGroup = new QActionGroup(this);
    _menuColorGRAYGroup = new QActionGroup(this);
    _menuColorDisplayGroup = new QActionGroup(this);

    _menuColorRGB->addActions( _menuColorRGBGroup->actions() );
    _menuColorCMYK->addActions( _menuColorCMYKGroup->actions() );
    _menuColorGRAY->addActions( _menuColorGRAYGroup->actions() );
    _menuColorDisplay->addActions( _menuColorDisplayGroup->actions() );

    // default blackpoint
    _menuColorBlackPoint = new QAction(tr("Black point compensation"), this);
    _menuColorBlackPoint->setCheckable(true);
    _menuColor->addAction(_menuColorBlackPoint);

    // populate default profiles
    populateColorProfileMenu(_menuColorRGB,
                             _menuColorRGBGroup,
                             Engine::colorSpaceRGB);
    populateColorProfileMenu(_menuColorCMYK,
                             _menuColorCMYKGroup,
                             Engine::colorSpaceCMYK);
    populateColorProfileMenu(_menuColorGRAY,
                             _menuColorGRAYGroup,
                             Engine::colorSpaceGRAY);
    populateColorProfileMenu(_menuColorDisplay,
                             _menuColorDisplayGroup,
                             Engine::colorSpaceRGB);

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
    palette.setColor( QPalette::Highlight, QColor(196, 110, 33) );
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    qApp->setPalette(palette);

    _mdi->setBackground( QBrush( QColor(20, 20, 20) ) );

    setStyleSheet( QString("QMenu::separator { background-color: rgb(53, 53, 53); height: 1px; }"
                           "QToolBar { border-color: none; }"
                           "QToolButton::menu-indicator { image: none; }") );
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
                                 Engine::colorSpace colorspace)
{
    if (!menu || !group) { return; }
    QMapIterator<QString, QString> i( Engine::getProfiles(colorspace) );
    while ( i.hasNext() ) {
        i.next();
        QAction *action = new QAction(menu);
        action->setText( i.key() );
        action->setData( i.value() );
        action->setCheckable(true);
        group->addAction(action);
        connect( action,
                 SIGNAL( triggered() ),
                 this,
                 SLOT( handleColorProfileTriggered() ) );
    }
    menu->clear();
    menu->addActions( group->actions() );
}

void
Window::handleColorProfileTriggered()
{
    QAction *action = qobject_cast<QAction*>( sender() );
    if (!action) { return; }
    action->setChecked(true);
    // TODO: save settings!
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
                 SLOT( setDefaultColorIntent() ) );
        _menuColorIntent->addAction(action);
    }
}

void
Window::setDefaultColorIntent()
{
    QAction *action = qobject_cast<QAction*>( sender() );
    if (!action) { return; }
    for ( auto &act : _menuColorIntent->actions() ) { act->setChecked(false); }
    action->setChecked(true);
    // TODO: save settings!
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

void
Window::handleOpenImageReady(const Engine::Image &image)
{
    if (isFileOpen(image.filename) ||
        !image.success ||
        image.buffer.length() < 1) { return; }
    MdiSubWindow *tab = new MdiSubWindow(_mdi,
                                         image.filename);
    tab->setAttribute(Qt::WA_DeleteOnClose);
    tab->setWindowIcon( QIcon::fromTheme(CYAN_ICON_SUBWINDOW) );
    tab->getView()->setImage(image);
    connect( tab->getView(),
             SIGNAL( dropped(QList<QUrl>) ),
             this,
             SLOT( handleDropped(QList<QUrl>) ) );
    tab->showMaximized();
}

void
Window::handleWindowActivated(QMdiSubWindow *window)
{
    MdiSubWindow *tab = qobject_cast<MdiSubWindow*>(window);
    if (!tab) { return; }
    qDebug() << "handle window activated" << tab->getFilename();
    if (tab->getFilename() == _lastTab) { return; }
    _lastTab = tab->getFilename();
    setImageSourceDetails( tab->getView()->getSourceDetails() );
}

void Window::setImageSourceDetails(const QString &info)
{
    _tabDetails->clear();
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
    _tabDetails->addTopLevelItems( level1items.toList() );
    level2items.clear();
    _tabDetails->expandAll();
}
