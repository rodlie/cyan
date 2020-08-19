#include "CyanBatchWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

CyanBatchWidget::CyanBatchWidget(QWidget *parent) :
    QWidget(parent)
  , _tree(nullptr)
  , _progress(nullptr)
  , _apply(nullptr)
  , _cancel(nullptr)

{
    setAcceptDrops(true);

    setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

    _tree = new QTreeWidget(this);

    QWidget *buttonWidget = new QWidget(this);
    buttonWidget->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    _apply = new QPushButton(this);
    _apply->setText(tr("Convert"));

    _cancel = new QPushButton(this);
    _cancel->setText(tr("Cancel"));

    _progress = new QProgressBar(this);

    buttonLayout->addWidget(_progress);
    buttonLayout->addWidget(_apply);
    buttonLayout->addWidget(_cancel);

    mainLayout->addWidget(_tree);
    mainLayout->addWidget(buttonWidget);
}
