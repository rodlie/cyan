#ifndef CYANLAYERTREEITEM_H
#define CYANLAYERTREEITEM_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QString>

#include <Magick++.h>

class CyanLayerTreeItem : public QTreeWidget, public QTreeWidgetItem
{
    Q_OBJECT

public:

    explicit CyanLayerTreeItem(QTreeWidget *parent = nullptr);
    ~CyanLayerTreeItem();

private:

    Magick::CompositeOperator _composite;
    int _id;
    QString _name;
    double _opacity;
    bool _visible;

public slots:

    Magick::CompositeOperator getComposite();
    void setComposite(Magick::CompositeOperator composite);
    int getLayerID();
    void setLayerID(int id);
    QString getLayerName();
    void setLayerName(QString name);
    double getOpacity();
    void setOpacity(double value);
    bool getVisibility();
    void setVisibility(bool visible);
};

#endif // CYANLAYERTREEITEM_H
