/*
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <Magick++.h>
#include <QLabel>
#include <QPushButton>

#define PREVIEW_WIDTH 640
#define PREVIEW_HEIGHT 480

class Dialog : public QDialog
{
    Q_OBJECT
public:
   explicit Dialog(QWidget *parent = NULL, const Magick::Image &image = Magick::Image(), int *option1 = 0);
private:
    QSpinBox *option1Box;
    int *option1Value;
    int defaultValue;
    Magick::Image preview;
    QLabel *previewLabel;
    QPushButton *applyButton;
    QPushButton *cancelButton;
private slots:
    void handleOption1Box(int value);
    void handlePreview(int value, bool effect = true);
    void adjustPreview();
    void cancelAction();
};

#endif // FILTERDIALOG_H
