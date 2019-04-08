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

#include "cyan_layerwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QIcon>
#include <QPixmap>

#include "cyan_view.h"

CyanLayerWidget::CyanLayerWidget(QWidget *parent) :
    QWidget(parent)
  , layerTree(nullptr)
  , layerCompositeBox(nullptr)
  , layerOpacitySlider(nullptr)
  , layerOpacitySpin(nullptr)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QWidget *compositeWidget = new QWidget(this);
    QHBoxLayout *compositeLayout = new QHBoxLayout(compositeWidget);

    QWidget *opacityWidget = new QWidget(this);
    QHBoxLayout *opacityLayout = new QHBoxLayout(opacityWidget);

    QLabel *compositeLabel = new QLabel(this);
    QLabel *compositeIconLabel = new QLabel(this);
    layerCompositeBox = new QComboBox(this);

    QLabel *opacityLabel = new QLabel(this);
    QLabel *opacityIconLabel = new QLabel(this);
    layerOpacitySlider = new QSlider(this);
    layerOpacitySpin = new QDoubleSpinBox(this);

    layerTree = new LayerTree(this);

    layerCompositeBox->setSizePolicy(QSizePolicy::Expanding,
                                     QSizePolicy::Fixed);

    compositeLabel->setText(tr("Mode"));
    compositeLabel->setSizePolicy(QSizePolicy::Minimum,
                                  QSizePolicy::Minimum);
    compositeIconLabel->setPixmap(QIcon::fromTheme("images_flickr")
                                  .pixmap(16, 16));
    compositeIconLabel->setMinimumSize(QSize(16, 16));
    compositeIconLabel->setMaximumSize(QSize(16, 16));

    opacityLabel->setText(tr("Opacity"));
    opacityLabel->setSizePolicy(QSizePolicy::Minimum,
                                QSizePolicy::Minimum);
    opacityIconLabel->setPixmap(QIcon::fromTheme("layer")
                                .pixmap(16, 16));
    opacityIconLabel->setMinimumSize(QSize(16, 16));
    opacityIconLabel->setMaximumSize(QSize(16, 16));

    layerOpacitySlider->setOrientation(Qt::Horizontal);
    layerOpacitySlider->setRange(0, 100);
    layerOpacitySpin->setRange(0, 100);

    compositeLayout->addWidget(compositeIconLabel);
    compositeLayout->addWidget(compositeLabel);
    compositeLayout->addWidget(layerCompositeBox);
    compositeLayout->setContentsMargins(0, 0, 0, 0);
    //compositeLayout->setSpacing(0);

    opacityLayout->addWidget(opacityIconLabel);
    opacityLayout->addWidget(opacityLabel);
    opacityLayout->addWidget(layerOpacitySlider);
    opacityLayout->addWidget(layerOpacitySpin);
    opacityLayout->setContentsMargins(0, 0, 0, 0);
    //opacityLayout->setSpacing(0);

    mainLayout->addWidget(compositeWidget);
    mainLayout->addWidget(opacityWidget);
    mainLayout->addWidget(layerTree);

    mainLayout->setContentsMargins(0, 10, 0, 0);
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

    connect(layerOpacitySlider,
            SIGNAL(sliderReleased()),
            this,
            SLOT(handleLayerOpacitySlider()));
    connect(layerOpacitySlider,
            SIGNAL(sliderMoved(int)),
            this,
            SLOT(handleLayerOpacitySlider()));
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

    QIcon icon = QIcon::fromTheme(QString("images_flickr"));

    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::OverCompositeOp]);
    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::DissolveCompositeOp]);

    layerCompositeBox->insertSeparator(layerCompositeBox->count());

    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::PlusCompositeOp]);
    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::MultiplyCompositeOp]);
    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::OverlayCompositeOp]);
    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::ScreenCompositeOp]);

    layerCompositeBox->insertSeparator(layerCompositeBox->count());

    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::ColorDodgeCompositeOp]);
    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::LinearDodgeCompositeOp]);
    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::ColorBurnCompositeOp]);
    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::LinearBurnCompositeOp]);

    layerCompositeBox->insertSeparator(layerCompositeBox->count());

    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::LightenCompositeOp]);
    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::DarkenCompositeOp]);
    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::LightenIntensityCompositeOp]);
    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::DarkenIntensityCompositeOp]);

    layerCompositeBox->insertSeparator(layerCompositeBox->count());

    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::SoftLightCompositeOp]);
    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::HardLightCompositeOp]);
    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::VividLightCompositeOp]);
    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::PegtopLightCompositeOp]);
    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::PinLightCompositeOp]);
    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::LinearLightCompositeOp]);

    layerCompositeBox->insertSeparator(layerCompositeBox->count());

    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::DifferenceCompositeOp]);
    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::MinusSrcCompositeOp]);
    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::DivideSrcCompositeOp]);

    layerCompositeBox->insertSeparator(layerCompositeBox->count());

    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::HueCompositeOp]);
    layerCompositeBox->addItem(icon, CyanCommon::compositeModes()[Magick::SaturateCompositeOp]);

    // add the "rest"
    layerCompositeBox->insertSeparator(layerCompositeBox->count());

    QMapIterator<Magick::CompositeOperator, QString> i(CyanCommon::compositeModes());
    while (i.hasNext()) {
        i.next();
        if (layerCompositeBox->findText(i.value(),
                                  Qt::MatchExactly |
                                  Qt::MatchCaseSensitive)==-1)
        {
            layerCompositeBox->addItem(icon, i.value());
        }
    }

    layerCompositeBox->setCurrentIndex(layerCompositeBox->findText(QString("Normal")));
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

    layerCompositeBox->setCurrentIndex(layerCompositeBox->findText(CyanCommon::compositeModes()[layer->getComposite()]));
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

    QMapIterator<Magick::CompositeOperator, QString> i(CyanCommon::compositeModes());
    while (i.hasNext()) {
        i.next();
        if (i.value() == layerCompositeBox->currentText()) {
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
    emit newLayer();
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
    layerOpacitySlider->blockSignals(true);
    layerOpacitySlider->setValue(static_cast<int>(value));
    layerOpacitySlider->blockSignals(false);

    CyanLayerTreeItem *layer = dynamic_cast<CyanLayerTreeItem*>(layerTree->currentItem());
    if (!layer) { return; }

    qDebug() << "layer opacity" << layer->getLayerID() << value << value/100;
    layer->setOpacity(value/100);
    emit layerOpacityChanged(value/100,
                             layer->getLayerID());
}

void CyanLayerWidget::handleLayerOpacitySlider()
{
    layerOpacitySpin->setValue(static_cast<int>(layerOpacitySlider->value()));
}
