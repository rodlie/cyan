#include "cyanlayerwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QIcon>
#include <QPixmap>

#include "common.h"
#include "view.h"

CyanLayerWidget::CyanLayerWidget(QWidget *parent) :
    QWidget(parent)
  , layerTree(nullptr)
  , layerCompositeBox(nullptr)
  , layerOpacitySlider(nullptr)
  , layerOpacitySpin(nullptr)
  , layerNewButton(nullptr)
  , layerRemoveButton(nullptr)
  , layerMoveUpButton(nullptr)
  , layerMoveDownButton(nullptr)
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

    QWidget *layerButtonsWidget = new QWidget(this);
    QHBoxLayout *layerButtonsLayout = new QHBoxLayout(layerButtonsWidget);

    layerCompositeBox->setSizePolicy(QSizePolicy::Expanding,
                                     QSizePolicy::Fixed);

    compositeLabel->setText(tr("Composite"));
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

    layerNewButton = new QPushButton(this);
    layerRemoveButton = new QPushButton(this);
    layerMoveUpButton = new QPushButton(this);
    layerMoveDownButton = new QPushButton(this);

    layerNewButton->setToolTip(tr("New layer"));
    layerRemoveButton->setToolTip(tr("Remove layer"));
    layerMoveUpButton->setToolTip(tr("Move layer up"));
    layerMoveDownButton->setToolTip(tr("Move layer down"));

    layerNewButton->setIcon(QIcon::fromTheme("document-new"));
    layerRemoveButton->setIcon(QIcon::fromTheme("edit-delete"));
    layerMoveUpButton->setIcon(QIcon::fromTheme("go-up"));
    layerMoveDownButton->setIcon(QIcon::fromTheme("go-down"));

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

    layerButtonsLayout->addWidget(layerNewButton);
    layerButtonsLayout->addWidget(layerRemoveButton);
    layerButtonsLayout->addWidget(layerMoveUpButton);
    layerButtonsLayout->addWidget(layerMoveDownButton);
    layerButtonsLayout->setContentsMargins(0, 0, 0, 0);
    //layerButtonsLayout->setSpacing(0);

    mainLayout->addWidget(compositeWidget);
    mainLayout->addWidget(opacityWidget);
    mainLayout->addWidget(layerTree);
    mainLayout->addWidget(layerButtonsWidget);

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


    connect(layerNewButton, SIGNAL(released()), this, SLOT(handleNewButtonReleased()));
    connect(layerRemoveButton, SIGNAL(released()), this, SLOT(handleRemoveButtonReleased()));
    connect(layerMoveUpButton, SIGNAL(released()), this, SLOT(handleUpButtonReleased()));
    connect(layerMoveDownButton, SIGNAL(released()), this, SLOT(handleDownButtonReleased()));

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

    layerCompositeBox->addItem(Common::compositeModes()[Magick::OverCompositeOp]);
    layerCompositeBox->addItem(Common::compositeModes()[Magick::DissolveCompositeOp]);

    layerCompositeBox->insertSeparator(layerCompositeBox->count());

    layerCompositeBox->addItem(Common::compositeModes()[Magick::PlusCompositeOp]);
    layerCompositeBox->addItem(Common::compositeModes()[Magick::MultiplyCompositeOp]);
    layerCompositeBox->addItem(Common::compositeModes()[Magick::OverlayCompositeOp]);
    layerCompositeBox->addItem(Common::compositeModes()[Magick::ScreenCompositeOp]);

    layerCompositeBox->insertSeparator(layerCompositeBox->count());

    layerCompositeBox->addItem(Common::compositeModes()[Magick::ColorDodgeCompositeOp]);
    layerCompositeBox->addItem(Common::compositeModes()[Magick::LinearDodgeCompositeOp]);
    layerCompositeBox->addItem(Common::compositeModes()[Magick::ColorBurnCompositeOp]);
    layerCompositeBox->addItem(Common::compositeModes()[Magick::LinearBurnCompositeOp]);

    layerCompositeBox->insertSeparator(layerCompositeBox->count());

    layerCompositeBox->addItem(Common::compositeModes()[Magick::LightenCompositeOp]);
    layerCompositeBox->addItem(Common::compositeModes()[Magick::DarkenCompositeOp]);
    layerCompositeBox->addItem(Common::compositeModes()[Magick::LightenIntensityCompositeOp]);
    layerCompositeBox->addItem(Common::compositeModes()[Magick::DarkenIntensityCompositeOp]);

    layerCompositeBox->insertSeparator(layerCompositeBox->count());

    layerCompositeBox->addItem(Common::compositeModes()[Magick::SoftLightCompositeOp]);
    layerCompositeBox->addItem(Common::compositeModes()[Magick::HardLightCompositeOp]);
    layerCompositeBox->addItem(Common::compositeModes()[Magick::VividLightCompositeOp]);
    layerCompositeBox->addItem(Common::compositeModes()[Magick::PegtopLightCompositeOp]);
    layerCompositeBox->addItem(Common::compositeModes()[Magick::PinLightCompositeOp]);
    layerCompositeBox->addItem(Common::compositeModes()[Magick::LinearLightCompositeOp]);

    layerCompositeBox->insertSeparator(layerCompositeBox->count());

    layerCompositeBox->addItem(Common::compositeModes()[Magick::DifferenceCompositeOp]);
    layerCompositeBox->addItem(Common::compositeModes()[Magick::MinusSrcCompositeOp]);
    layerCompositeBox->addItem(Common::compositeModes()[Magick::DivideSrcCompositeOp]);

    layerCompositeBox->insertSeparator(layerCompositeBox->count());

    layerCompositeBox->addItem(Common::compositeModes()[Magick::HueCompositeOp]);
    layerCompositeBox->addItem(Common::compositeModes()[Magick::SaturateCompositeOp]);

    // add the "rest"
    layerCompositeBox->insertSeparator(layerCompositeBox->count());

    QMapIterator<Magick::CompositeOperator, QString> i(Common::compositeModes());
    while (i.hasNext()) {
        i.next();
        if (layerCompositeBox->findText(i.value(),
                                  Qt::MatchExactly |
                                  Qt::MatchCaseSensitive)==-1)
        {
            layerCompositeBox->addItem(i.value());
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
    Q_UNUSED(old)
    CyanLayerTreeItem *layer = dynamic_cast<CyanLayerTreeItem*>(item);
    if (!layer) { return; }

    layerCompositeBox->setCurrentIndex(layerCompositeBox->findText(Common::compositeModes()[layer->getComposite()]));
    layerOpacitySpin->setValue(layer->getOpacity()*100);

    qDebug() << "layer selected" << layer->getLayerID() << layerCompositeBox->currentText() << layerOpacitySpin->value();
}

void CyanLayerWidget::handleLayerCompositeBox()
{
    if (layerCompositeBox->currentText().isEmpty()) { return; }
    CyanLayerTreeItem *layer = dynamic_cast<CyanLayerTreeItem*>(layerTree->currentItem());
    if (!layer) { return; }

    qDebug() << "layer comp changed to" << layerCompositeBox->currentText();

    QMapIterator<Magick::CompositeOperator, QString> i(Common::compositeModes());
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

void CyanLayerWidget::handleTreeMoveLayer(QKeyEvent *e)
{
    qDebug() << "forward move layer";
    emit moveLayerEvent(e);
}

void CyanLayerWidget::handleNewButtonReleased()
{
    qDebug() << "forward new layer";
    emit newLayer();
}

void CyanLayerWidget::handleRemoveButtonReleased()
{
    CyanLayerTreeItem *layer = dynamic_cast<CyanLayerTreeItem*>(layerTree->currentItem());
    if (!layer) { return; }

    qDebug() << "forward remove layer";
    emit removeLayer(layer->getLayerID());
}

void CyanLayerWidget::handleUpButtonReleased()
{
    CyanLayerTreeItem *layer = dynamic_cast<CyanLayerTreeItem*>(layerTree->currentItem());
    if (!layer) { return; }

    qDebug() << "forward move layer up";
    emit moveLayerUp(layer->getLayerID());
}

void CyanLayerWidget::handleDownButtonReleased()
{
    CyanLayerTreeItem *layer = dynamic_cast<CyanLayerTreeItem*>(layerTree->currentItem());
    if (!layer) { return; }

    qDebug() << "forward move layer down";
    emit moveLayerDown(layer->getLayerID());
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
