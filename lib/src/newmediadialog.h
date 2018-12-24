/*
# Copyright Ole-André Rodlie.
#
# ole.andre.rodlie@gmail.com
#
# This software is governed by the CeCILL license under French law and
# abiding by the rules of distribution of free software. You can use,
# modify and / or redistribute the software under the terms of the CeCILL
# license as circulated by CEA, CNRS and INRIA at the following URL
# "https://www.cecill.info".
#
# As a counterpart to the access to the source code and rights to
# modify and redistribute granted by the license, users are provided only
# with a limited warranty and the software's author, the holder of the
# economic rights and the subsequent licensors have only limited
# liability.
#
# In this respect, the user's attention is drawn to the associated risks
# with loading, using, modifying and / or developing or reproducing the
# software by the user in light of its specific status of free software,
# that can mean that it is complicated to manipulate, and that also
# so that it is for developers and experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to test and test the software's suitability
# Requirements in the conditions of their systems
# data to be ensured and, more generally, to use and operate
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL license and that you accept its terms.
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

#include <Magick++.h>

#include "common.h"

class NewMediaDialog : public QDialog
{
    Q_OBJECT

public:

    NewMediaDialog(QWidget *parent = nullptr,
                   QString title = QObject::tr("New Image"),
                   Common::newDialogType dialogType = Common::newImageDialogType,
                   Magick::ColorspaceType colorspace = Magick::UndefinedColorspace,
                   QSize size = QSize(0,0));
    ~NewMediaDialog();

public slots:

    Magick::Image getImage();

private:

    Common::newDialogType _type;
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
