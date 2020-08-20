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
  , _profileType(nullptr)
  , _profile(nullptr)
  , _intent(nullptr)
  , _blackpoint(nullptr)
  , _format(nullptr)
  , _quality(nullptr)
{
    setAcceptDrops(true);

    setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

    QWidget *optWidget = new QWidget(this);
    optWidget->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *optLayout = new QVBoxLayout(optWidget);
    optLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *profileWidget = new QWidget(this);
    profileWidget->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *profileLayout = new QHBoxLayout(profileWidget);
    profileLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *profileLabel = new QLabel(this);
    profileLabel->setText(tr("Profile"));
    profileLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    _profileType = new QComboBox(this);
    _profileType->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    _profile = new QComboBox(this);

    QWidget *intentWidget = new QWidget(this);
    intentWidget->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *intentLayout = new QHBoxLayout(intentWidget);
    intentLayout->setContentsMargins(0, 0, 0 ,0);

    _intent = new QComboBox(this);

    QLabel *intentLabel = new QLabel(this);
    intentLabel->setText(tr("Rendering Intent"));
    intentLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    _blackpoint = new QCheckBox(this);
    _blackpoint->setText(tr("Black Point"));

    QWidget *formatWidget = new QWidget(this);
    formatWidget->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *formatLayout = new QHBoxLayout(formatWidget);
    formatLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *formatLabel = new QLabel(this);
    formatLabel->setText(tr("Format"));
    formatLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    _format = new QComboBox(this);

    QLabel *qualityLabel = new QLabel(this);
    qualityLabel->setText(tr("Quality"));
    qualityLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    _quality = new QSpinBox(this);

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

    formatLayout->addWidget(formatLabel);
    formatLayout->addWidget(_format);
    formatLayout->addWidget(qualityLabel);
    formatLayout->addWidget(_quality);

    profileLayout->addWidget(profileLabel);
    profileLayout->addWidget(_profileType);
    profileLayout->addWidget(_profile);

    intentLayout->addWidget(intentLabel);
    intentLayout->addWidget(_intent);
    intentLayout->addWidget(_blackpoint);

    optLayout->addWidget(profileWidget);
    optLayout->addWidget(intentWidget);
    optLayout->addWidget(formatWidget);

    buttonLayout->addWidget(_progress);
    buttonLayout->addWidget(_apply);
    buttonLayout->addWidget(_cancel);

    mainLayout->addWidget(optWidget);
    mainLayout->addWidget(_tree);
    mainLayout->addWidget(buttonWidget);
}
