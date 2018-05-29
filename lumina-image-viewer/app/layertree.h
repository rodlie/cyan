#ifndef LAYERTREE_H
#define LAYERTREE_H

#include <QTreeWidget>

class LayerTree : public QTreeWidget
{
    Q_OBJECT

public:
    explicit LayerTree(QWidget *parent = NULL);
    ~LayerTree();
};

#endif // LAYERTREE_H
