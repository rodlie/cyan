#ifndef CYANBATCHWIDGET_H
#define CYANBATCHWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>

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
    QComboBox *_profileType;
    QComboBox *_profile;
    QComboBox *_intent;
    QCheckBox *_blackpoint;
    QComboBox *_format;
    QSpinBox *_quality;
};

#endif // CYANBATCHWIDGET_H
