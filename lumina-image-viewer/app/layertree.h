#ifndef LAYERTREE_H
#define LAYERTREE_H

#include <QTreeWidget>
#include <QMdiSubWindow>
#include "view.h"

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
