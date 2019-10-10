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

#ifndef NEWMEDIADIALOG_H
#define NEWMEDIADIALOG_H

#include <QObject>
#include <QDialog>
#include <QSpinBox>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QRadioButton>
#include <QCheckBox>

#include <Magick++.h>

#include "cyan_common.h"

#include "qtcolorpicker.h"

class NewMediaDialog : public QDialog
{
    Q_OBJECT

public:
    NewMediaDialog(QWidget *parent = nullptr,
                   QString title = QObject::tr("New Image"),
                   CyanCommon::newDialogType dialogType = CyanCommon::newImageDialogType,
                   Magick::ColorspaceType colorspace = Magick::UndefinedColorspace,
                   Magick::Blob profile = Magick::Blob(),
                   QSize size = QSize(0,0));
    ~NewMediaDialog();

public slots:
    Magick::Image getImage();

private:
    CyanCommon::newDialogType _type;
    QSpinBox *_width;
    QSpinBox *_height;
    QPushButton *_ok;
    QPushButton *_cancel;
    Magick::Image _image;
    Magick::ColorspaceType _colorspace;
    QLineEdit *_label;
    QComboBox *_select;
    QComboBox *_profile;
    QRadioButton *_depth8;
    QRadioButton *_depth16;
    QRadioButton *_depth32;
    QRadioButton *_depth64;
    Magick::Blob _forcedProfile;
    QtColorPicker *_picker;
    QCheckBox *_solidColor;

private slots:
    void handleOk();
    void handleCancel();
    void createImage(QSize geo = QSize(1024, 1024),
                     Magick::ColorspaceType colorspace = Magick::sRGBColorspace,
                     size_t depth = 8);
    void populateProfiles(Magick::ColorspaceType colorspace);
    void handleColorspaceChanged(int index = -1);
    Magick::Blob selectedProfile();
};

#endif // NEWMEDIADIALOG_H
