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

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , layoutBox(0)
    , variantBox(0)
    , modelBox(0)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setAttribute(Qt::WA_QuitOnClose, false);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layoutBox = new QComboBox(this);
    variantBox = new QComboBox(this);
    modelBox = new QComboBox(this);
    layout->addWidget(layoutBox);
    layout->addWidget(variantBox);
    layout->addWidget(modelBox);
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
    box->addItem(QObject::tr("None (system default)"));
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
