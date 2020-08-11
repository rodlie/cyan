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

#include "CyanColorConvertDialog.h"

#include <QIcon>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

#include "CyanCommon.h"

ConvertDialog::ConvertDialog(QWidget *parent,
                             const QString &title,
                             const QString &filename,
                             Magick::ColorspaceType colorspace) :
    QDialog (parent)
  , _filename(filename)
  , _colorspace(colorspace)
  , _ok(nullptr)
  , _cancel(nullptr)
  , _box(nullptr)
  , _label(nullptr)
  , _icon(nullptr)
{
    setWindowTitle(title);
    setWindowIcon(QIcon::fromTheme("colors"));


    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    _ok = new QPushButton(this);
    _cancel = new QPushButton(this);

    _ok->setText(tr("Convert"));
    _ok->setIcon(QIcon::fromTheme("colors"));
    _cancel->setText(tr("Cancel"));
    _cancel->setIcon(QIcon::fromTheme("process-stop"));

    QWidget *buttonWidget = new QWidget(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);

    buttonLayout->addWidget(_ok);
    buttonLayout->addWidget(_cancel);

    _box = new QComboBox(this);
    _box->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);

    QWidget *labelWidget = new QWidget(this);
    QHBoxLayout *labelLayout = new QHBoxLayout(labelWidget);

    _label = new QLabel(this);
    _icon = new QLabel(this);

    _icon->setPixmap(QIcon::fromTheme("colors").pixmap(QSize(32, 32)));
    _label->setText(tr("Convert selected image to %1 color space?")
                    .arg(CyanCommon::colorspaceToString(colorspace)));
    _label->setWordWrap(true);
    labelLayout->addWidget(_icon);
    labelLayout->addWidget(_label);

    mainLayout->addWidget(labelWidget);
    mainLayout->addWidget(_box);
    mainLayout->addWidget(buttonWidget);
    mainLayout->addStretch();

    populateProfiles();

    connect(_ok,
            SIGNAL(released()),
            this,
            SLOT(handleOk()));
    connect(_cancel,
            SIGNAL(released()),
            this,
            SLOT(handleCancel()));
}

const QString ConvertDialog::getProfile()
{
    return _box->currentData().toString();
}

void ConvertDialog::populateProfiles()
{
    _box->clear();
    int index = -1;
    int c = 0;
    QIcon colorWheelIcon = QIcon::fromTheme("color-wheel");
    QMapIterator<QString, QString> i(CyanCommon::getColorProfiles(_colorspace));
    while (i.hasNext()) {
        i.next();
        _box->addItem(colorWheelIcon,
                      i.key(),
                      i.value());
        if (i.value() == _filename) { index = c; }
        c++;
    }
    _box->setCurrentIndex(index);
}

void ConvertDialog::handleOk()
{
    QDialog::accept();
}

void ConvertDialog::handleCancel()
{
    QDialog::reject();
}
