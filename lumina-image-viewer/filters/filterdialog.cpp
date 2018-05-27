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
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QMapIterator>

Dialog::Dialog(QWidget *parent, const Magick::Image &image, filterOptions effectOptions)
    : QDialog(parent)
    , options(effectOptions)
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

    QMapIterator<QString, QVariant> i(options.option);
    while (i.hasNext()) {
        i.next();
        qDebug() << i.key() << i.value();
        if (options.type[i.key()] == filterTypeDoubleSpinBox) {
            QDoubleSpinBox *optionbox = new QDoubleSpinBox(this);
            optionbox->setObjectName(i.key());
            optionbox->setToolTip(optionbox->objectName());
            optionbox->setMinimum(options.min[i.key()].toDouble());
            optionbox->setMaximum(options.max[i.key()].toDouble());
            optionbox->setValue(i.value().toDouble());
            connect(optionbox, SIGNAL(valueChanged(double)), this, SLOT(handleOption(double)));
            containerLayout->addWidget(optionbox);
        } else if (options.type[i.key()] == filterTypeIntSpinBox) {
            QSpinBox *optionbox = new QSpinBox(this);
            optionbox->setObjectName(i.key());
            optionbox->setToolTip(optionbox->objectName());
            optionbox->setMinimum(options.min[i.key()].toDouble());
            optionbox->setMaximum(options.max[i.key()].toDouble());
            optionbox->setValue(i.value().toDouble());
            connect(optionbox, SIGNAL(valueChanged(int)), this, SLOT(handleOption(int)));
            containerLayout->addWidget(optionbox);
        }
    }

    previewLabel = new QLabel(this);
    previewLabel->setMinimumSize(QSize(PREVIEW_WIDTH, PREVIEW_HEIGHT));
    previewLabel->setMaximumSize(previewLabel->minimumSize());
    handlePreview(/*false*/);

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
    containerLayout->addWidget(applyButton);
    containerLayout->addWidget(cancelButton);
}

void Dialog::handleOption(int value)
{
    QSpinBox *optionbox = qobject_cast<QSpinBox*>(sender());
    if (!optionbox) { return;}
    handleOption(optionbox->objectName(), value);
}

void Dialog::handleOption(double value)
{
    QDoubleSpinBox *optionbox = qobject_cast<QDoubleSpinBox*>(sender());
    if (!optionbox) { return;}
    handleOption(optionbox->objectName(), value);
}

void Dialog::handleOption(QString option, QVariant value)
{
    qDebug() << "handle option" << option << value;
    if (option.isEmpty()) { return; }
    options.option[option] = value;
    handlePreview();
}

void Dialog::handlePreview(bool effect)
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
            if (options.effect == filterEffectSwirl) {
                tmp.swirl(options.option[SWIRL_DEGREES].toDouble());
            }
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
    close();
}
