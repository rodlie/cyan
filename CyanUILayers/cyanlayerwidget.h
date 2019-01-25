#ifndef CYANLAYERWIDGET_H
#define CYANLAYERWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QMdiSubWindow>

#include <Magick++.h>

#include "cyanlayertreeitem.h"
#include "layertree.h"

class CyanLayerWidget : public QWidget
{
    Q_OBJECT

public:

    explicit CyanLayerWidget(QWidget *parent = nullptr);
    ~CyanLayerWidget();

signals:

    void layerCompositeChanged(const Magick::CompositeOperator &composite,
                               const int &id);
    void layerOpacityChanged(const double &value,
                             const int &id);
    void selectedLayer(int id);
    void layerVisibilityChanged(int id, bool visible);
    void layerLabelChanged(int id, QString label);
    void moveLayerEvent(QKeyEvent *e);
    void newLayer();
    void removeLayer(int id);
    void moveLayerUp(int id);
    void moveLayerDown(int id);

public slots:

    void setCanvasID(const QString &id);
    const QString getCanvasID();

    CyanLayerTreeItem* getCurrentLayer();
    void setCurrentLayer(const int &id);

    void handleTabActivated(QMdiSubWindow *tab,
                            bool force = false);
    void clearTree();

private:

    LayerTree *layerTree;
    QComboBox *layerCompositeBox;
    QSlider *layerOpacitySlider;
    QDoubleSpinBox *layerOpacitySpin;
    QPushButton *layerNewButton;
    QPushButton *layerRemoveButton;
    QPushButton *layerMoveUpButton;
    QPushButton *layerMoveDownButton;

private slots:

    void populateCompositeBox();

    void handleLayerActivated(QTreeWidgetItem *item,
                              int col);
    void handleLayerActivated(QTreeWidgetItem *item,
                              QTreeWidgetItem *old);

    void handleLayerOpacitySpin(double value);
    void handleLayerOpacitySlider();
    void handleLayerCompositeBox();

    void handleTreeSelectedLayer(int id);
    void handleTreeLayerVisibility(int id,
                                   bool visible);
    void handleTreeLayerLabel(int id,
                              QString label);
    void handleTreeMoveLayer(QKeyEvent *e);

    void handleNewButtonReleased();
    void handleRemoveButtonReleased();
    void handleUpButtonReleased();
    void handleDownButtonReleased();
};

#endif // CYANLAYERWIDGET_H
