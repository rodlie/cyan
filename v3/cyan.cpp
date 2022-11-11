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
    , _menuWindows(nullptr)
    , _actionOpenImage(nullptr)
{
    _mdi = new Mdi(this);
    connect( _mdi,
             SIGNAL( dropped(QList<QUrl>) ),
             this,
             SLOT( handleDropped(QList<QUrl>) ) );

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

    _toolbar = new QToolBar(tr("Tools"), this);
    _toolbar->setMovable(false);
    addToolBar(Qt::LeftToolBarArea, _toolbar);

    _statusbar = new QStatusBar(this);
    setStatusBar(_statusbar);
    _statusbar->showMessage( tr("Cyan v3 WIP") );

    _menubar = new QMenuBar(this);
    setMenuBar(_menubar);

    _menuFile = new QMenu(tr("File"), this);
    _menuView = new QMenu(tr("View"), this);
    _menuHelp = new QMenu(tr("Help"), this);

    _menuWindows = new QtWindowListMenu(this);
    _menuWindows->attachToMdiArea(_mdi);

    _menubar->addMenu(_menuFile);
    _menubar->addMenu(_menuView);
    _menubar->addMenu(_menuWindows);
    _menubar->addMenu(_menuHelp);

    setupTheme(false, true);

    _actionOpenImage = new QAction(QIcon::fromTheme("document-open"),
                                   tr("Open image"),
                                   this);
    _actionOpenImage->setShortcut( QKeySequence( tr("Ctrl+O") ) );
    connect( _actionOpenImage,
             SIGNAL( triggered() ),
             this,
             SLOT( handleActionOpenImage() ) );

    _menuFile->addAction(_actionOpenImage);
    _toolbar->addAction(_actionOpenImage);
}

Cyan::Window::~Window()
{

}

void
Cyan::Window::openImage(bool showDialog,
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
    if ( Engine::isValidImage(filePath) ) {
        qDebug() << "TODO: read image and open a new view";
    }
}

void
Cyan::Window::setupTheme(bool native,
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
Cyan::Window::handleDropped(const QList<QUrl> &urls)
{
    for (auto &url : urls) { openImage( false, url.toLocalFile() ); }
}

void Cyan::Window::handleActionOpenImage()
{
    openImage();
}
