#ifndef LAYERTREE_H
#define LAYERTREE_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMdiSubWindow>
#include "view.h"

class LayerTreeItem : public QTreeWidget, public QTreeWidgetItem
{
    Q_OBJECT

public:
    explicit LayerTreeItem(QTreeWidget *parent = NULL);
    ~LayerTreeItem();
    Magick::CompositeOperator _composite;
    int _id;
    QString _name;

public slots:
    Magick::CompositeOperator getComposite();
    void setComposite(Magick::CompositeOperator composite);
    int getLayerID();
    void setLayerID(int id);
    QString getLayerName();
    void setLayerName(QString name);
};

class LayerTree : public QTreeWidget
{
    Q_OBJECT

public:
    explicit LayerTree(QWidget *parent = NULL);
    ~LayerTree();

private slots:
    void handleTabActivated(QMdiSubWindow *tab);
    void populateTree(View *image);
};

#endif // LAYERTREE_H
