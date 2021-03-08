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

#include "CyanLayerWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QIcon>
#include <QPixmap>

#include "CyanImageFormat.h"
#include "CyanView.h"

CyanLayerWidget::CyanLayerWidget(QWidget *parent,
                                 bool native) :
    QWidget(parent)
  , layerTree(nullptr)
  , layerCompositeBox(nullptr)
  //, layerOpacitySlider(nullptr)
  , layerOpacitySpin(nullptr)
  , _textSupport(false)
  , actionBar(nullptr)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QWidget *compositeWidget = new QWidget(this);
    QHBoxLayout *compositeLayout = new QHBoxLayout(compositeWidget);

    //QWidget *opacityWidget = new QWidget(this);
    //QHBoxLayout *opacityLayout = new QHBoxLayout(opacityWidget);

    //QLabel *compositeLabel = new QLabel(this);
    //QLabel *compositeIconLabel = new QLabel(this);
    layerCompositeBox = new QComboBox(this);
    layerCompositeBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);

    //QLabel *opacityLabel = new QLabel(this);
    //QLabel *opacityIconLabel = new QLabel(this);
    //layerOpacitySlider = new QSlider(this);
    layerOpacitySpin = new QDoubleSpinBox(this);

    layerTree = new LayerTree(this, native);

    layerCompositeBox->setSizePolicy(QSizePolicy::Expanding,
                                     QSizePolicy::Fixed);

    /*compositeLabel->setText(tr("Mode"));
    compositeLabel->setSizePolicy(QSizePolicy::Minimum,
                                  QSizePolicy::Minimum);*/
    /*compositeIconLabel->setPixmap(QIcon::fromTheme("images")
                                  .pixmap(16, 16));
    compositeIconLabel->setMinimumSize(QSize(16, 16));
    compositeIconLabel->setMaximumSize(QSize(16, 16));
    compositeIconLabel->setToolTip(tr("Layer composite mode"));
*/
    /*opacityLabel->setText(tr("Opacity"));
    opacityLabel->setSizePolicy(QSizePolicy::Minimum,
                                QSizePolicy::Minimum);*/
  /*  opacityIconLabel->setPixmap(QIcon::fromTheme("layer")
                                .pixmap(16, 16));
    opacityIconLabel->setMinimumSize(QSize(16, 16));
    opacityIconLabel->setMaximumSize(QSize(16, 16));
    opacityIconLabel->setToolTip(tr("Layer opacity"));
*/
    //layerOpacitySlider->setOrientation(Qt::Horizontal);
    //layerOpacitySlider->setRange(0, 100);
    layerOpacitySpin->setRange(0, 100);

   // compositeLayout->addWidget(compositeIconLabel);
    //compositeLayout->addWidget(compositeLabel);
    compositeLayout->addWidget(layerCompositeBox);
    compositeLayout->setContentsMargins(0, 0, 0, 0);
    //compositeLayout->setSpacing(0);

    //opacityLayout->addWidget(opacityIconLabel);
    //opacityLayout->addWidget(opacityLabel);
    //opacityLayout->addWidget(layerOpacitySlider);
    compositeLayout->addWidget(layerOpacitySpin);
    //opacityLayout->setContentsMargins(0, 0, 0, 0);
    //opacityLayout->setSpacing(0);

    mainLayout->addWidget(compositeWidget);
    //mainLayout->addWidget(opacityWidget);
    mainLayout->addWidget(layerTree);

    actionBar = new QToolBar(this);
    actionBar->setIconSize(QSize(16, 16));
    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    actionBar->addActions(layerTree->getLeftActions());
    actionBar->addWidget(spacer);
    actionBar->addActions(layerTree->getRightActions());
    mainLayout->addWidget(actionBar);

    mainLayout->setContentsMargins(0, 0, 0, 0);
    //mainLayout->setSpacing(0);

    populateCompositeBox();

    connect(layerTree,
            SIGNAL(itemActivated(QTreeWidgetItem*,int)),
            this,
            SLOT(handleLayerActivated(QTreeWidgetItem*,int)));
    connect(layerTree,
            SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this,
            SLOT(handleLayerActivated(QTreeWidgetItem*,int)));
    connect(layerTree,
            SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this,
            SLOT(handleLayerActivated(QTreeWidgetItem*,QTreeWidgetItem*)));

    /*connect(layerOpacitySlider,
            SIGNAL(sliderReleased()),
            this,
            SLOT(handleLayerOpacitySlider()));
    connect(layerOpacitySlider,
            SIGNAL(sliderMoved(int)),
            this,
            SLOT(handleLayerOpacitySlider()));*/
    connect(layerOpacitySpin,
            SIGNAL(valueChanged(double)),
            this,
            SLOT(handleLayerOpacitySpin(double)));

    connect(layerCompositeBox,
            SIGNAL(currentIndexChanged(QString)),
            this,
            SLOT(handleLayerCompositeBox()));

    connect(layerTree, SIGNAL(selectedLayer(int)), this, SLOT(handleTreeSelectedLayer(int)));

    connect(layerTree, SIGNAL(layerVisibilityChanged(int,bool)), this, SLOT(handleTreeLayerVisibility(int,bool)));
    connect(layerTree, SIGNAL(layerLabelChanged(int,QString)), this, SLOT(handleTreeLayerLabel(int,QString)));
    connect(layerTree, SIGNAL(layerLockChanged(int,bool)), this, SLOT(handleTreeLayerLock(int,bool)));
    connect(layerTree, SIGNAL(moveLayerEvent(QKeyEvent*)), this, SLOT(handleTreeMoveLayer(QKeyEvent*)));

    connect(layerTree, SIGNAL(actDuplicate()), this, SLOT(handleDuplicateLayer()));
    connect(layerTree, SIGNAL(actMoveDown()), this, SLOT(handleMoveLayerDown()));
    connect(layerTree, SIGNAL(actMoveUp()), this, SLOT(handleMoveLayerUp()));
    connect(layerTree, SIGNAL(actNewImage()), this, SLOT(handleNewImageLayer()));
    connect(layerTree, SIGNAL(actRemove()), this, SLOT(handleRemoveLayer()));
    connect(layerTree, SIGNAL(actNewText()), this, SLOT(handleNewTextLayer()));

    layerOpacitySpin->setValue(100.0);
}

CyanLayerWidget::~CyanLayerWidget()
{

}

void CyanLayerWidget::setCanvasID(const QString &id)
{
    layerTree->setCanvasID(id);
}

const QString CyanLayerWidget::getCanvasID()
{
    return layerTree->getCanvasID();
}

CyanLayerTreeItem *CyanLayerWidget::getCurrentLayer()
{
    return dynamic_cast<CyanLayerTreeItem*>(layerTree->currentItem());
}

void CyanLayerWidget::setCurrentLayer(const int &id)
{
    for (int i=0;i<layerTree->topLevelItemCount();++i) {
        CyanLayerTreeItem *item = dynamic_cast<CyanLayerTreeItem*>(layerTree->topLevelItem(i));
        if (!item) { continue; }
        if (id == item->getLayerID()) {
            layerTree->setCurrentItem(item);
            handleLayerActivated(item, 0);
            break;
        }
    }
}

void CyanLayerWidget::handleTabActivated(QMdiSubWindow *tab,
                                         bool force)
{
    layerTree->handleTabActivated(tab,
                                  force);
}

void CyanLayerWidget::clearTree()
{
    layerTree->clear();
}

void CyanLayerWidget::setTextSupport(bool enabled)
{
    _textSupport = enabled;
    layerTree->setTextSupport(enabled);
}

void CyanLayerWidget::moveCurrentLayerUp(bool triggered)
{
    Q_UNUSED(triggered)
    CyanLayerTreeItem *item = getCurrentLayer();
    if (!item) { return; }
    int id = item->getLayerID();
    moveLayerUp(id);
}

void CyanLayerWidget::moveCurrentLayerDown(bool triggered)
{
    Q_UNUSED(triggered)
    CyanLayerTreeItem *item = getCurrentLayer();
    if (!item) { return; }
    int id = item->getLayerID();
    moveLayerDown(id);
}

void CyanLayerWidget::duplicateCurrentLayer(bool triggered)
{
    Q_UNUSED(triggered)
    CyanLayerTreeItem *item = getCurrentLayer();
    if (!item) { return; }
    int id = item->getLayerID();
    duplicateLayer(id);
}

void CyanLayerWidget::removeCurrentLayer(bool triggered)
{
    Q_UNUSED(triggered)
    CyanLayerTreeItem *item = getCurrentLayer();
    if (!item) { return; }
    int id = item->getLayerID();
    removeLayer(id);
}

void CyanLayerWidget::createNewLayer(bool triggered)
{
    Q_UNUSED(triggered);
    newLayer(false);
}

void CyanLayerWidget::populateCompositeBox()
{
    layerCompositeBox->clear();

    /*
    gimp modes in cyan:
    normal - ok
    dissolve - ok
    lighten only - ok
    screen - ok
    dodge (color) - ok
    addition (plus) - ok
    darken only - ok
    multiply - ok
    burn (color) - ok
    overlay - ok
    soft light - ok
    hard light - ok
    difference - ok
    substract (minus src) - ok
    grain * - https://www.imagemagick.org/discourse-server/viewtopic.php?t=25085
    divide (src) - ok
    hue - ok
    saturation - ok
    color - ??? https://docs.gimp.org/en/gimp-concepts-layer-modes.html
    value - ??? https://docs.gimp.org/en/gimp-concepts-layer-modes.html
    */

    QIcon icon = QIcon::fromTheme(QString("images"));

    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::OverCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::OverCompositeOp].second);

    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::DissolveCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::DissolveCompositeOp].second);

    layerCompositeBox->insertSeparator(layerCompositeBox->count());

    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::PlusCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::PlusCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::MultiplyCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::MultiplyCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::OverlayCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::OverlayCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::ScreenCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::ScreenCompositeOp].second);

    layerCompositeBox->insertSeparator(layerCompositeBox->count());

    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::ColorDodgeCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::ColorDodgeCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::LinearDodgeCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::LinearDodgeCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::ColorBurnCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::ColorBurnCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::LinearBurnCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::LinearBurnCompositeOp].second);

    layerCompositeBox->insertSeparator(layerCompositeBox->count());

    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::LightenCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::LightenCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::DarkenCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::DarkenCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::LightenIntensityCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::LightenIntensityCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::DarkenIntensityCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::DarkenIntensityCompositeOp].second);

    layerCompositeBox->insertSeparator(layerCompositeBox->count());

    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::SoftLightCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::SoftLightCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::HardLightCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::HardLightCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::VividLightCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::VividLightCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::PegtopLightCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::PegtopLightCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::PinLightCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::PinLightCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::LinearLightCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::LinearLightCompositeOp].second);

    layerCompositeBox->insertSeparator(layerCompositeBox->count());

    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::DifferenceCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::DifferenceCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::MinusSrcCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::MinusSrcCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::DivideSrcCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::DivideSrcCompositeOp].second);

    layerCompositeBox->insertSeparator(layerCompositeBox->count());

    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::HueCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::HueCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::SaturateCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::SaturateCompositeOp].second);

    layerCompositeBox->insertSeparator(layerCompositeBox->count());
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::CopyCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::CopyCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::CopyRedCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::CopyRedCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::CopyGreenCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::CopyGreenCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::CopyBlueCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::CopyBlueCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::CopyAlphaCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::CopyAlphaCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::CopyCyanCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::CopyCyanCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::CopyMagentaCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::CopyMagentaCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::CopyYellowCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::CopyYellowCompositeOp].second);
    layerCompositeBox->addItem(icon,
                               CyanImageFormat::compositeModes()[Magick::CopyBlackCompositeOp].first,
                               CyanImageFormat::compositeModes()[Magick::CopyBlackCompositeOp].second);

    // add the "rest"
    layerCompositeBox->insertSeparator(layerCompositeBox->count());

    QMapIterator<Magick::CompositeOperator, QPair<QString, int> > i(CyanImageFormat::compositeModes());
    while (i.hasNext()) {
        i.next();
        // TODO move to ID
        if (layerCompositeBox->findText(i.value().first,
                                  Qt::MatchExactly |
                                  Qt::MatchCaseSensitive)==-1)
        {
            layerCompositeBox->addItem(icon, i.value().first, i.value().second);
        }
    }

    layerCompositeBox->setCurrentIndex(layerCompositeBox->findText(QString("Normal (Over)")));
}

void CyanLayerWidget::handleLayerActivated(QTreeWidgetItem *item,
                                           int col)
{
    Q_UNUSED(col)
    handleLayerActivated(item, item);
}

void CyanLayerWidget::handleLayerActivated(QTreeWidgetItem *item,
                                           QTreeWidgetItem *old)
{
    if (item==old) { return; } // ignore if same
    CyanLayerTreeItem *layer = dynamic_cast<CyanLayerTreeItem*>(item);
    if (!layer) { return; }

    layerCompositeBox->setCurrentIndex(layerCompositeBox->findText(CyanImageFormat::compositeModes()[layer->getComposite()].first));
    layerOpacitySpin->setValue(layer->getOpacity()*100);

    qDebug() << "layer selected" << layer->getLayerID() << layerCompositeBox->currentText() << layerOpacitySpin->value();

    emit selectedLayer(layer->getLayerID());
}

void CyanLayerWidget::handleLayerCompositeBox()
{
    if (layerCompositeBox->currentText().isEmpty()) { return; }
    CyanLayerTreeItem *layer = dynamic_cast<CyanLayerTreeItem*>(layerTree->currentItem());
    if (!layer) { return; }

    qDebug() << "layer comp changed to" << layerCompositeBox->currentText();

    QMapIterator<Magick::CompositeOperator, QPair<QString, int> > i(CyanImageFormat::compositeModes());
    while (i.hasNext()) {
        i.next();
        if (i.value().first == layerCompositeBox->currentText()) {
            layer->setComposite(i.key());
            emit layerCompositeChanged(i.key(), layer->getLayerID());
            break;
        }
    }
}

void CyanLayerWidget::handleTreeSelectedLayer(int id)
{
    qDebug() << "forward selected layer from tree" << id;
    emit selectedLayer(id);
}

void CyanLayerWidget::handleTreeLayerVisibility(int id, bool visible)
{
    qDebug() << "forward layer visibility" << id << visible;
    emit layerVisibilityChanged(id, visible);
}

void CyanLayerWidget::handleTreeLayerLabel(int id, QString label)
{
    qDebug() << "forward layer label" << id << label;
    emit layerLabelChanged(id, label);
}

void CyanLayerWidget::handleTreeLayerLock(int id, bool lock)
{
    qDebug() << "forward layer lock" << id << lock;
    emit layerLockChanged(id, lock);
}

void CyanLayerWidget::handleTreeMoveLayer(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_PageUp:
        handleMoveLayerUp();
        break;
    case Qt::Key_PageDown:
        handleMoveLayerDown();
        break;
    default:
        emit moveLayerEvent(e);
    }
}

void CyanLayerWidget::handleNewImageLayer()
{
    qDebug() << "forward new layer";
    emit newLayer(false);
}

void CyanLayerWidget::handleNewTextLayer()
{
    qDebug() << "forward new text layer";
    emit newLayer(true);
}

void CyanLayerWidget::handleRemoveLayer()
{
    CyanLayerTreeItem *layer = dynamic_cast<CyanLayerTreeItem*>(layerTree->currentItem());
    if (!layer) { return; }

    qDebug() << "forward remove layer";
    emit removeLayer(layer->getLayerID());
}

void CyanLayerWidget::handleMoveLayerUp()
{
    CyanLayerTreeItem *layer = dynamic_cast<CyanLayerTreeItem*>(layerTree->currentItem());
    if (!layer) { return; }

    qDebug() << "forward move layer up";
    emit moveLayerUp(layer->getLayerID());
}

void CyanLayerWidget::handleMoveLayerDown()
{
    CyanLayerTreeItem *layer = dynamic_cast<CyanLayerTreeItem*>(layerTree->currentItem());
    if (!layer) { return; }

    qDebug() << "forward move layer down";
    emit moveLayerDown(layer->getLayerID());
}

void CyanLayerWidget::handleDuplicateLayer()
{
    CyanLayerTreeItem *layer = dynamic_cast<CyanLayerTreeItem*>(layerTree->currentItem());
    if (!layer) { return; }

    qDebug() << "forward duplicate layer" << layer->getLayerID();
    emit duplicateLayer(layer->getLayerID());
}

void CyanLayerWidget::handleLayerOpacitySpin(double value)
{
    qDebug() << "handle spin" << value;
    /*layerOpacitySlider->blockSignals(true);
    layerOpacitySlider->setValue(static_cast<int>(value));
    layerOpacitySlider->blockSignals(false);*/

    CyanLayerTreeItem *layer = dynamic_cast<CyanLayerTreeItem*>(layerTree->currentItem());
    if (!layer) { return; }

    qDebug() << "layer opacity" << layer->getLayerID() << value << value/100;
    layer->setOpacity(value/100);
    emit layerOpacityChanged(value/100,
                             layer->getLayerID());
}

void CyanLayerWidget::handleLayerOpacitySlider()
{
    //layerOpacitySpin->setValue(static_cast<int>(layerOpacitySlider->value()));
}
