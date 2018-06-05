/*
#
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Cyan is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.
#
# Cyan is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Cyan.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>
#
*/

#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>
#include <Magick++.h>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QThread>
#include "common.h"

#define PREVIEW_WIDTH 640
#define PREVIEW_HEIGHT 480

class ImageHandler : public QObject
{
    Q_OBJECT

public:
    explicit ImageHandler(QObject *parent = 0);
    ~ImageHandler();

signals:
    void filteredImage(Magick::Image image);
    void errorMessage(QString message);
    void warningMessage(QString message);

public slots:
    void requestFilterImage(Magick::Image, filterOptions options);
    void filterImage(Magick::Image image, filterOptions options);

private:
    QThread t;
};

class Dialog : public QDialog
{
    Q_OBJECT

public:
   explicit Dialog(QWidget *parent = NULL, const Magick::Image &image = Magick::Image(), filterOptions effectOptions = filterOptions());
    filterOptions options;
    Magick::Image filteredImage;

signals:
    void filterDone();
    void disableOptions(bool disable);

private:
    Magick::Image preview;
    QLabel *previewLabel;
    QPushButton *applyButton;
    QPushButton *cancelButton;
    QProgressBar *progress;
    ImageHandler *backend;

public slots:
    void showProgress();
private slots:
    void handleOption(int value);
    void handleOption(double value);
    void handleOption(QString option, QVariant value);
    void handlePreview(bool effect = true);
    void adjustPreview();
    void applyAction();
    void cancelAction();
    void handleBackendDone(Magick::Image result);
};

#endif // FILTERDIALOG_H
