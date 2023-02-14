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

#include "convertdialog.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QDebug>
#include <QTimer>
#include <QMapIterator>
#include <QGroupBox>

using namespace Cyan;

ConvertDialog::ConvertDialog(QWidget *parent,
                             Engine::ColorSettings cs,
                             const QString &inFilename,
                             const QString &outFilename,
                             const QString &title) :
    QDialog (parent)
  , _working(false)
  , _cs(cs)
  , _inFilename(inFilename)
  , _outFilename(outFilename)
  , _boxCategory(nullptr)
  , _boxDestination(nullptr)
  , _boxIntent(nullptr)
  , _checkBlackPoint(nullptr)
  , _buttonSave(nullptr)
  , _buttonCancel(nullptr)
  , _progress(nullptr)
{
    setWindowTitle(title);

    qDebug() << _inFilename;
    qDebug() << _outFilename;
    qDebug() << _cs.applyDisplayProfile;
    qDebug() << _cs.applyPrintProfile;
    qDebug() << "bp" << _cs.blackpoint;
    qDebug() << "cs" << _cs.colorspace;
    qDebug() << "intent" << _cs.intent;
    qDebug() << "rgb" << _cs.profiles.rgb;
    qDebug() << "cmyk" << _cs.profiles.cmyk;
    qDebug() << "gray" << _cs.profiles.gray;
    qDebug() << "display" << _cs.profiles.display;
    qDebug() << "print" << _cs.profiles.print;

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QWidget *footerWidget = new QWidget(this);
    footerWidget->setContentsMargins(0, 0, 0, 0);
    footerWidget->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Fixed);
    QHBoxLayout *footerLayout = new QHBoxLayout(footerWidget);

    _progress = new QProgressBar(this);
    _progress->setRange(0, 100);
    _progress->setValue(100);
    _progress->setFormat( tr("Ready") );

    _buttonSave = new QPushButton(QIcon::fromTheme(CYAN_ICON_SAVE_IMAGE),
                                  tr("Save"),
                                  this);
    _buttonCancel = new QPushButton(QIcon::fromTheme(CYAN_ICON_STOP),
                                    tr("Cancel"),
                                    this);

    QGroupBox *destWidget = new QGroupBox(tr("Output Profile"), this);
    QHBoxLayout *destLayout = new QHBoxLayout(destWidget);

    _boxCategory = new QComboBox(this);
    _boxCategory->setMinimumWidth(75);
    _boxCategory->addItem( QIcon::fromTheme(CYAN_ICON_DISPLAY), tr("RGB") );
    _boxCategory->addItem( QIcon::fromTheme(CYAN_ICON_PRINTER_COLOR), tr("CMYK") );
    _boxCategory->addItem( QIcon::fromTheme(CYAN_ICON_CONVERT_TO_GRAY), tr("GRAY") );

    _boxDestination = new QComboBox(this);
    _boxDestination->setMinimumWidth(250);

    _boxCategory->setSizePolicy(QSizePolicy::Fixed,
                                QSizePolicy::Fixed);
    _boxDestination->setSizePolicy(QSizePolicy::Expanding,
                                  QSizePolicy::Fixed);

    QGroupBox *renderWidget = new QGroupBox(tr("Rendering Intent"), this);
    QHBoxLayout *renderLayout = new QHBoxLayout(renderWidget);

    _boxIntent = new QComboBox(this);
    _boxIntent->setMinimumWidth(100);

    _checkBlackPoint = new QCheckBox(tr("Black Point Compensation"), this);
    _checkBlackPoint->setCheckable(true);
    _checkBlackPoint->setChecked(_cs.blackpoint);

    destLayout->addWidget(_boxCategory);
    destLayout->addWidget(_boxDestination);

    renderLayout->addWidget(_boxIntent);
    renderLayout->addWidget(_checkBlackPoint);

    footerLayout->addWidget(_progress);
    footerLayout->addWidget(_buttonSave);
    footerLayout->addWidget(_buttonCancel);

    mainLayout->addWidget(destWidget);
    mainLayout->addWidget(renderWidget);
    mainLayout->addStretch();
    mainLayout->addWidget(footerWidget);

    connect( _boxCategory,
             SIGNAL( currentIndexChanged(int) ),
             this,
             SLOT( handleCategoryChanged(int) ) );

    QTimer::singleShot( 0,
                        this,
                        SLOT( init() ) );
}

void ConvertDialog::init()
{
    populateColorIntentMenu();
    if (_cs.applyPrintProfile) { _boxCategory->setCurrentIndex(1); }
    populateProfiles();
}

void
ConvertDialog::populateColorIntentMenu()
{
    std::vector<Engine::RenderingIntent> intents;
    intents.push_back(Engine::UndefinedRenderingIntent);
    intents.push_back(Engine::SaturationRenderingIntent);
    intents.push_back(Engine::PerceptualRenderingIntent);
    intents.push_back(Engine::AbsoluteRenderingIntent);
    intents.push_back(Engine::RelativeRenderingIntent);

    _boxIntent->clear();
    for (auto &intent : intents) {
        switch (intent) {
        case Engine::SaturationRenderingIntent:
            _boxIntent->addItem(QIcon::fromTheme(CYAN_ICON_COLORS),
                                tr("Saturation"),
                                Engine::SaturationRenderingIntent);
            break;
        case Engine::PerceptualRenderingIntent:
            _boxIntent->addItem(QIcon::fromTheme(CYAN_ICON_COLORS),
                                tr("Perceptual"),
                                Engine::PerceptualRenderingIntent);
            break;
        case Engine::AbsoluteRenderingIntent:
            _boxIntent->addItem(QIcon::fromTheme(CYAN_ICON_COLORS),
                                tr("Absolute"),
                                Engine::AbsoluteRenderingIntent);
            break;
        case Engine::RelativeRenderingIntent:
            _boxIntent->addItem(QIcon::fromTheme(CYAN_ICON_COLORS),
                                tr("Relative"),
                                Engine::RelativeRenderingIntent);
            break;
        default:
            _boxIntent->addItem( QIcon::fromTheme(CYAN_ICON_COLORS), tr("Undefined") );
        }
    }
    for (int i = 0; i < _boxIntent->count() ; i++) {
        if ( _boxIntent->itemData(i).toInt() == static_cast<Engine::RenderingIntent>(_cs.intent) ) {
            _boxIntent->setCurrentIndex(i);
            break;
        }
    }
}

void
ConvertDialog::populateProfiles()
{
    int index  = _boxCategory->currentIndex();
    QString fallback;
    QString selected;
    if (index < 0) { return; }
    Engine::ColorSpace space;
    switch(index) {
    case 1:
        space = Engine::ColorSpaceCMYK;
        fallback = CYAN_PROFILE_FALLBACK_CMYK;
        selected = ( _cs.applyPrintProfile && !_cs.profiles.print.isEmpty() ) ? _cs.profiles.print : _cs.profiles.cmyk;
        break;
    case 2:
        space = Engine::ColorSpaceGRAY;
        fallback = CYAN_PROFILE_FALLBACK_GRAY;
        selected = _cs.profiles.gray;
        break;
    default:
        space = Engine::ColorSpaceRGB;
        fallback = CYAN_PROFILE_FALLBACK_RGB;
        selected = ( _cs.applyDisplayProfile && !_cs.profiles.display.isEmpty() ) ? _cs.profiles.display : _cs.profiles.rgb;
    }

    _boxDestination->clear();

    QMapIterator<QString, QString> i( Engine::getProfiles(space,
                                                          false,
                                                          fallback,
                                                          true) );
    while ( i.hasNext() ) {
        i.next();
        _boxDestination->addItem( QIcon::fromTheme(CYAN_ICON_COLOR_WHEEL),
                                  i.key(),
                                  i.value() );
    }

    if ( !selected.isEmpty() ) {
        for (int i = 0; i < _boxDestination->count(); ++i) {
            if (_boxDestination->itemData(i).toString() == selected) {
                _boxDestination->setCurrentIndex(i);
                break;
            }
        }
    }
}

void
ConvertDialog::handleCategoryChanged(int index)
{
    Q_UNUSED(index)
    populateProfiles();
}

void
ConvertDialog::closeEvent(QCloseEvent *e)
{
    if (_working) {
        qDebug() << "I'm busy ...";
        e->ignore();
    } else { e->accept(); }
}
