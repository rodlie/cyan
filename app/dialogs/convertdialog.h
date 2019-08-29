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

#ifndef CONVERTDIALOG_H
#define CONVERTDIALOG_H

#include <QObject>
#include <QDialog>
#include <QPushButton>
#include <QComboBox>

#include <Magick++.h>

class ConvertDialog : public QDialog
{
    Q_OBJECT

public:

    ConvertDialog(QWidget *parent = nullptr,
                  const QString &title = QObject::tr("Assign Color Profile"),
                  const QString &filename = QString(),
                  Magick::ColorspaceType colorspace = Magick::sRGBColorspace);

public slots:

    const QString getProfile();

private:
    QString _filename;
    Magick::ColorspaceType _colorspace;
    QPushButton *_ok;
    QPushButton *_cancel;
    QComboBox *_box;

private slots:

    void populateProfiles();
    void handleOk();
    void handleCancel();
};

#endif // CONVERTDIALOG_H
