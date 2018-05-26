/*
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "filterdialog.h"
#include <QVBoxLayout>
#include <QIcon>
#include <QDebug>
#include <QLabel>
#include <QPixmap>

Dialog::Dialog(QWidget *parent, const Magick::Image &image, int *option1)
    : QDialog(parent)
    , option1Box(0)
    , option1Value(option1)
    , defaultValue(*option1)
    , preview(image)
    , previewLabel(0)
    , applyButton(0)
    , cancelButton(0)
{
    setAttribute(Qt::WA_DeleteOnClose, true);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    QWidget *containerWidget = new QWidget(this);
    containerWidget->setContentsMargins(0,0,0,0);

    QHBoxLayout *containerLayout = new QHBoxLayout(containerWidget);
    containerLayout->setMargin(0);
    containerLayout->setContentsMargins(0,0,0,0);
    containerLayout->setSpacing(0);

    option1Box = new QSpinBox(this);
    option1Box->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    connect(option1Box, SIGNAL(valueChanged(int)), this, SLOT(handleOption1Box(int)));

    previewLabel = new QLabel(this);
    previewLabel->setMinimumSize(QSize(PREVIEW_WIDTH, PREVIEW_HEIGHT));
    previewLabel->setMaximumSize(previewLabel->minimumSize());
    handlePreview(0, false);

    applyButton = new QPushButton(this);
    applyButton->setText(tr("Apply"));
    applyButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(applyButton, SIGNAL(released()), this, SLOT(close()));

    cancelButton = new QPushButton(this);
    cancelButton->setText(tr("Cancel"));
    cancelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(cancelButton, SIGNAL(released()), this, SLOT(cancelAction()));

    layout->addWidget(previewLabel);
    layout->addWidget(containerWidget);
    containerLayout->addWidget(option1Box);
    containerLayout->addWidget(applyButton);
    containerLayout->addWidget(cancelButton);
}

void Dialog::handleOption1Box(int value)
{
    *option1Value = value;
    handlePreview(value);
}

void Dialog::handlePreview(int value, bool effect)
{
    try {
        if ((int)preview.columns() > PREVIEW_WIDTH) {
            qDebug() << "prep preview";
            preview.scale(Magick::Geometry(QString("%1x%2").arg(PREVIEW_WIDTH).arg(PREVIEW_HEIGHT).toStdString()));
            preview.strip();
            if (preview.depth()>8) { preview.depth(8); }
            adjustPreview();
        }
        Magick::Blob blob;
        Magick::Image tmp = preview;
        if (effect) {
            tmp.swirl(value);
        }
        tmp.magick("BMP");
        tmp.write(&blob);
        previewLabel->setPixmap(QPixmap::fromImage(QImage::fromData(QByteArray((char*)blob.data(), blob.length()))));
    }
    catch(Magick::Error &error_ ) {
        qDebug() << error_.what();
    }
    catch(Magick::Warning &warn_ ) {
        qDebug() << warn_.what();
    }
}

void Dialog::adjustPreview()
{
    previewLabel->setMinimumSize(QSize(preview.columns(), preview.rows()));
    previewLabel->setMaximumSize(previewLabel->minimumSize());
}

void Dialog::cancelAction()
{
    *option1Value = defaultValue;
    close();
}
