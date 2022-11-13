/*
#
# Cyan - https://github.com/rodlie/cyan
#
# Copyright (c) 2020-2022 Ole-André Rodlie. All rights reserved.
# Copyright (c) 2018-2019 Ole-André Rodlie, FxArena. All rights reserved.
# Copyright (c) 2016-2017 Ole-André Rodlie, INRIA. All rights reserved.
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

#include <vector>

using namespace Cyan;

Cyan::Window::Window(QWidget *parent)
    : QMainWindow(parent)
    , _mdi(nullptr)
    , _splitter(nullptr)
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
    , _menuColorBlackPoint(nullptr)
    , _menuColorRGBGroup(nullptr)
    , _menuColorCMYKGroup(nullptr)
    , _menuColorGRAYGroup(nullptr)
    , _menuColorButton(nullptr)
    , _menuWindows(nullptr)
    , _actionOpenImage(nullptr)
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
                                                       Engine::supportedFormats().join(" ") );
    }
    QString filePath = showDialog ? dialogFilename : filename;
    if ( !isFileOpen(filePath) && Engine::isValidImage(filePath) ) {
        QtConcurrent::run(this, &Window::readImage, filePath);
    }
}

void
Window::readImage(const QString &filename)
{
    qDebug() << "readImage" << filename;
    Engine::Image image = Engine::readImage(filename);
    emit openImageReady(image);
}

void
Window::setupUi()
{
    // mdi
    _mdi = new Mdi(this);
    connect( _mdi,
             SIGNAL( dropped(QList<QUrl>) ),
             this,
             SLOT( handleDropped(QList<QUrl>) ) );

    // splitters
    _splitter = new QSplitter(this);
    _splitterLeft = new QSplitter(this);
    _splitterRight = new QSplitter(this);
    _splitter->setOrientation(Qt::Horizontal);
    _splitterLeft->setOrientation(Qt::Vertical);
    _splitterRight->setOrientation(Qt::Vertical);
    _splitter->addWidget(_splitterLeft);
    _splitter->addWidget(_splitterRight);
    _splitterLeft->addWidget(_mdi);

    setCentralWidget(_splitter);

    // tool bar
    _toolbar = new QToolBar(tr("Tools"), this);
    _toolbar->setMovable(false);
    _toolbar->setIconSize( QSize(32, 32) );
    addToolBar(Qt::LeftToolBarArea, _toolbar);

    // status bar
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

    _menuColor->setIcon( QIcon::fromTheme("applications-graphics") );
    _menuColor->addMenu(_menuColorRGB);
    _menuColor->addMenu(_menuColorCMYK);
    _menuColor->addMenu(_menuColorGRAY);
    _menuColor->addMenu(_menuColorIntent);

    _menuColorButton = new QToolButton(this);
    _menuColorButton->setPopupMode(QToolButton::InstantPopup);
    _menuColorButton->setIcon( QIcon::fromTheme("applications-graphics") );
    _menuColorButton->setText( tr("Colors") );
    _menuColorButton->setMenu(_menuColor);

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
    _actionOpenImage = new QAction(QIcon::fromTheme("document-open"),
                                   tr("Open image"),
                                   this);
    _actionOpenImage->setShortcut( QKeySequence( tr("Ctrl+O") ) );
    connect( _actionOpenImage,
             SIGNAL( triggered() ),
             this,
             SLOT( handleActionOpenImage() ) );
    _menuFile->addAction(_actionOpenImage);

    // tool bar
    _toolbar->addAction(_actionOpenImage);
    _toolbar->addWidget(_menuColorButton);

    // default profiles
    _menuColorRGBGroup = new QActionGroup(this);
    _menuColorCMYKGroup = new QActionGroup(this);
    _menuColorGRAYGroup = new QActionGroup(this);

    _menuColorRGB->addActions( _menuColorRGBGroup->actions() );
    _menuColorCMYK->addActions( _menuColorCMYKGroup->actions() );
    _menuColorGRAY->addActions( _menuColorGRAYGroup->actions() );

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
    palette.setColor( QPalette::Window, QColor(53,53,53) );
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor( QPalette::Base, QColor(15,15,15) );
    palette.setColor( QPalette::AlternateBase, QColor(53,53,53) );
    palette.setColor(QPalette::Link, Qt::white);
    palette.setColor(QPalette::LinkVisited, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor( QPalette::Button, QColor(53,53,53) );
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor( QPalette::Highlight, QColor(196,110,33) );
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    qApp->setPalette(palette);

    _mdi->setBackground( QBrush( QColor(20, 20, 20) ) );

    setStyleSheet(QString("QMenu::separator { background-color: rgb(53, 53, 53); height: 1px; }"
                          "QToolBar { border-color: none; }"
                          "QToolButton::menu-indicator { image: none; }"));
}

void
Window::handleDropped(const QList<QUrl> &urls)
{
    for (auto &url : urls) { openImage( false, url.toLocalFile() ); }
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
        //action->setIcon(QIcon::fromTheme("color-wheel"));
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
        //action->setIcon(); // TODO
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

    /*QSettings settings;
    settings.beginGroup(QString("color"));
    settings.setValue(QString("intent"),
                      action->data().toInt());
    settings.endGroup();
    settings.sync();*/
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

void Window::handleOpenImageReady(const Engine::Image &image)
{
    qDebug() << "handle open image ready" << image.filename;
    if ( isFileOpen(image.filename) ) { return; }
    MdiSubWindow *tab = new MdiSubWindow(_mdi, image.filename);
    tab->setAttribute(Qt::WA_DeleteOnClose);
    tab->getView()->setImage(image.buffer, image.width, image.height);
    tab->showMaximized();
}
