/*
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "dialog.h"
#include <QVBoxLayout>
#include <QIcon>
#include <QDebug>
#include <QLabel>
#include <QPixmap>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , layoutBox(0)
    , variantBox(0)
    , modelBox(0)
{
    setAttribute(Qt::WA_QuitOnClose, true);
    setWindowTitle(tr("Lumina Keyboard Settings"));

    QHBoxLayout *layout = new QHBoxLayout(this);
    QWidget *containerWidget = new QWidget(this);
    QVBoxLayout *containerLayout = new QVBoxLayout(containerWidget);

    QLabel *keyboardLabel = new QLabel(this);
    QIcon keyboardIcon = QIcon::fromTheme("keyboard");
    keyboardLabel->setPixmap(keyboardIcon.pixmap(QSize(64, 64)));
    keyboardLabel->setMinimumSize(QSize(64, 64));
    keyboardLabel->setMaximumSize(QSize(64, 64));

    layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->addWidget(keyboardLabel);
    layout->addWidget(containerWidget);

    layoutBox = new QComboBox(this);
    variantBox = new QComboBox(this);
    modelBox = new QComboBox(this);

    QWidget *containerWidgetBoxLayout = new QWidget(this);
    QHBoxLayout *containerLayoutBoxLayout = new QHBoxLayout(containerWidgetBoxLayout);
    QLabel *layoutBoxLabel = new QLabel(this);
    containerLayoutBoxLayout->setMargin(0);
    containerLayoutBoxLayout->setSpacing(0);
    layoutBoxLabel->setText(tr("Layout"));
    layoutBoxLabel->setMaximumWidth(50);
    layoutBoxLabel->setMinimumWidth(50);
    containerLayoutBoxLayout->addWidget(layoutBoxLabel);
    containerLayoutBoxLayout->addWidget(layoutBox);

    QWidget *containerWidgetBoxVariant = new QWidget(this);
    QHBoxLayout *containerLayoutBoxVariant = new QHBoxLayout(containerWidgetBoxVariant);
    QLabel *variantBoxLabel = new QLabel(this);
    containerLayoutBoxVariant->setMargin(0);
    containerLayoutBoxVariant->setSpacing(0);
    variantBoxLabel->setText(tr("Variant"));
    variantBoxLabel->setMaximumWidth(50);
    variantBoxLabel->setMinimumWidth(50);
    containerLayoutBoxVariant->addWidget(variantBoxLabel);
    containerLayoutBoxVariant->addWidget(variantBox);

    QWidget *containerWidgetBoxModel = new QWidget(this);
    QHBoxLayout *containerLayoutBoxModel = new QHBoxLayout(containerWidgetBoxModel);
    QLabel *modelBoxLabel = new QLabel(this);
    containerLayoutBoxModel->setMargin(0);
    containerLayoutBoxModel->setSpacing(0);
    modelBoxLabel->setText(tr("Model"));
    modelBoxLabel->setMaximumWidth(50);
    modelBoxLabel->setMinimumWidth(50);
    containerLayoutBoxModel->addWidget(modelBoxLabel);
    containerLayoutBoxModel->addWidget(modelBox);

    containerLayout->addWidget(containerWidgetBoxLayout);
    containerLayout->addWidget(containerWidgetBoxVariant);
    containerLayout->addWidget(containerWidgetBoxModel);
    containerLayout->addStretch();

    populateBoxes();

    connect(layoutBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleLayoutChanged(int)));
    connect(variantBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleVariantChanged(int)));
    connect(modelBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleModelChanged(int)));
}

void Dialog::populateBox(QComboBox *box, xkbType type)
{
    if (box==NULL) { return; }
    QStringList xkb = Common::parseXKB(type);
    box->clear();
    box->addItem(QIcon::fromTheme("keyboard"), QObject::tr("System default"));
    for (int i=0;i<xkb.size();i++) {
        QString itemString = xkb.at(i);
        if (itemString.isEmpty()) { continue; }
        QString itemData = itemString.split(" ").at(0);
        if (itemData.isEmpty()) { continue; }
        box->addItem(QIcon::fromTheme("keyboard"), itemString, itemData);
    }
    setCurrentIndex(box, type);
}

void Dialog::populateBoxes()
{
    populateBox(layoutBox, xkbLayout);
    populateBox(variantBox, xkbVariant);
    populateBox(modelBox, xkbModel);
}

void Dialog::handleLayoutChanged(int index)
{
    if (index == -1) { return; }
    QString value = layoutBox->itemData(index).toString();
    Common::saveKeyboard("layout", value);
    Common::loadKeyboard();
}

void Dialog::handleVariantChanged(int index)
{
    if (index == -1) { return; }
    QString value = variantBox->itemData(index).toString();
    Common::saveKeyboard("variant", value);
    Common::loadKeyboard();
}

void Dialog::handleModelChanged(int index)
{
    if (index == -1) { return; }
    QString value = modelBox->itemData(index).toString();
    Common::saveKeyboard("model", value);
    Common::loadKeyboard();
}

void Dialog::setCurrentIndex(QComboBox *box, xkbType type)
{
    QString kbType;
    switch (type) {
    case xkbLayout:
        kbType = "layout";
        break;
    case xkbVariant:
        kbType = "variant";
        break;
    case xkbModel:
        kbType = "model";
        break;
    default:;
    }
    QString value = Common::defaultKeyboard(kbType);
    for (int i=0;i<box->count();i++) {
        if (box->itemData(i).toString() == value) { box->setCurrentIndex(i); }
    }
}
