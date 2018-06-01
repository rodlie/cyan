/*
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
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
