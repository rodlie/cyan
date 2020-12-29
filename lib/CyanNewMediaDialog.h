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
#include <QProgressBar>

#include <Magick++.h>

#include "CyanCommon.h"
#include "qtcolorpicker.h"

#include "CyanGlobal.h"

class CYANSHARED_EXPORT NewMediaDialog : public QDialog
{
    Q_OBJECT

public:
    enum CyanDrawTextureType
    {
        CyanDrawTextureTransparent,
        CyanDrawTextureSolid,
        CyanDrawTextureGradient,
        CyanDrawTextureGradientColor,
        CyanDrawTextureRadial,
        CyanDrawTextureRadialColor,
        CyanDrawTexturePlasma,
        CyanDrawTexturePlasmaFractal,
        CyanDrawTexturePlasmaColor,
        CyanDrawTextureGaussianNoise,
        CyanDrawTextureImpulseNoise,
        CyanDrawTextureLaplacianNoise,
        CyanDrawTextureLoopsGaussian,
        CyanDrawTextureLoopsImpulse,
        CyanDrawTextureLoopsLaplacian
    };
    NewMediaDialog(QWidget *parent = nullptr,
                   QString title = QObject::tr("New Image"),
                   CyanCommon::newDialogType dialogType = CyanCommon::newImageDialogType,
                   Magick::ColorspaceType colorspace = Magick::UndefinedColorspace,
                   Magick::Blob profile = Magick::Blob(),
                   QSize size = QSize(0,0),
                   int depth = 8);
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
    Magick::Blob _forcedProfile;
    QtColorPicker *_colorPickerFrom;
    QtColorPicker *_colorPickerTo;
    QComboBox *_drawOption;
    QComboBox *_bitBox;
    QProgressBar *_progbar;

signals:
    void createImageFinished();

private slots:
    void handleOk();
    void handleCancel();
    void createImage(QSize geo = QSize(1024, 1024),
                     Magick::ColorspaceType colorspace = Magick::sRGBColorspace,
                     size_t depth = 8,
                     QString label = tr("Untitled"),
                     int option = 0);
    void populateProfiles(Magick::ColorspaceType colorspace);
    void handleColorspaceChanged(int index = -1);
    Magick::Blob selectedProfile();
    void handleCreatedImage();
};

#endif // NEWMEDIADIALOG_H
