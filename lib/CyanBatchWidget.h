#ifndef CYANBATCHWIDGET_H
#define CYANBATCHWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QProgressBar>

class CyanBatchWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CyanBatchWidget(QWidget *parent = nullptr);

private:
    QTreeWidget *_tree;
    QProgressBar *_progress;
    QPushButton *_apply;
    QPushButton *_cancel;
};

#endif // CYANBATCHWIDGET_H
