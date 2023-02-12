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
  , _boxSource(nullptr)
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

    QWidget *headerWidget = new QWidget(this);
    headerWidget->setContentsMargins(0, 0, 0, 0);
    headerWidget->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Expanding);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *textLabel = new QLabel(this);
    textLabel->setWordWrap(true);
    textLabel->setOpenExternalLinks(true);
    textLabel->setSizePolicy(QSizePolicy::MinimumExpanding,
                             QSizePolicy::MinimumExpanding);

    QWidget *footerWidget = new QWidget(this);
    footerWidget->setContentsMargins(0, 0, 0, 0);
    footerWidget->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Fixed);
    QHBoxLayout *footerLayout = new QHBoxLayout(footerWidget);

    _progress = new QProgressBar(this);
    _progress->setRange(0, 100);
    _progress->setValue(100);
    _progress->setFormat("");

    _buttonSave = new QPushButton(QIcon::fromTheme(CYAN_ICON_SAVE_IMAGE),
                                  tr("Save"),
                                  this);
    _buttonCancel = new QPushButton(QIcon::fromTheme(CYAN_ICON_STOP),
                                    tr("Cancel"),
                                    this);

    QWidget *renderWidget = new QWidget(this);
    renderWidget->setContentsMargins(0, 0, 0, 0);
    renderWidget->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Fixed);
    QHBoxLayout *renderLayout = new QHBoxLayout(renderWidget);

    _boxIntent = new QComboBox(this);
    _checkBlackPoint = new QCheckBox(tr("Black Point"), this);
    _checkBlackPoint->setCheckable(true);
    _checkBlackPoint->setChecked(_cs.blackpoint);

    QLabel *intentLabel = new QLabel(tr("Rendering intent"), this);

    renderLayout->addWidget(intentLabel);
    renderLayout->addWidget(_boxIntent);
    renderLayout->addWidget(_checkBlackPoint);

    headerLayout->addWidget(textLabel);
    headerLayout->addStretch();

    footerLayout->addWidget(_progress);
    footerLayout->addStretch();
    footerLayout->addWidget(_buttonSave);
    footerLayout->addWidget(_buttonCancel);

    mainLayout->addWidget(headerWidget);
    mainLayout->addWidget(renderWidget);
    mainLayout->addWidget(footerWidget);

    populateColorIntentMenu();
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
            _boxIntent->addItem(tr("Saturation"), Engine::SaturationRenderingIntent);
            break;
        case Engine::PerceptualRenderingIntent:
            _boxIntent->addItem(tr("Perceptual"), Engine::PerceptualRenderingIntent);
            break;
        case Engine::AbsoluteRenderingIntent:
            _boxIntent->addItem(tr("Absolute"), Engine::AbsoluteRenderingIntent);
            break;
        case Engine::RelativeRenderingIntent:
            _boxIntent->addItem(tr("Relative"), Engine::RelativeRenderingIntent);
            break;
        default:
            _boxIntent->addItem( tr("Undefined") );
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
ConvertDialog::closeEvent(QCloseEvent *e)
{
    if (_working) {
        qDebug() << "I'm busy ...";
        e->ignore();
    } else { e->accept(); }
}
