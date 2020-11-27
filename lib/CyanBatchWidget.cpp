#include "CyanBatchWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QSettings>
#include <QDebug>

CyanBatchWidget::CyanBatchWidget(QWidget *parent) :
    QWidget(parent)
  , _tree(nullptr)
  , _progress(nullptr)
  , _apply(nullptr)
  , _cancel(nullptr)
  , _profile(nullptr)
  , _intent(nullptr)
  , _blackpoint(nullptr)
  , _format(nullptr)
  , _compression(nullptr)
  , _quality(nullptr)
  , _working(false)
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

    _profile = new QComboBox(this);

    QWidget *intentWidget = new QWidget(this);
    intentWidget->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *intentLayout = new QHBoxLayout(intentWidget);
    intentLayout->setContentsMargins(0, 0, 0 ,0);

    _intent = new QComboBox(this);

    QLabel *intentLabel = new QLabel(this);
    intentLabel->setText(tr("Intent"));
    intentLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    _blackpoint = new QCheckBox(this);
    _blackpoint->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _blackpoint->setText(tr("Black Point"));

    QWidget *formatWidget = new QWidget(this);
    formatWidget->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *formatLayout = new QHBoxLayout(formatWidget);
    formatLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *formatLabel = new QLabel(this);
    formatLabel->setText(tr("Format"));
    formatLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    _format = new QComboBox(this);

    QLabel *compressionLabel = new QLabel(this);
    compressionLabel->setText(tr("Compression"));
    compressionLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    _compression = new QComboBox(this);

    QLabel *qualityLabel = new QLabel(this);
    qualityLabel->setText(tr("Quality"));
    qualityLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    _quality = new QSpinBox(this);
    _quality->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    /*compressionLabel->setHidden(true);
    _compression->setHidden(true);
    qualityLabel->setHidden(true);
    _quality->setHidden(true);*/

    _tree = new QTreeWidget(this);

    QWidget *buttonWidget = new QWidget(this);
    buttonWidget->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    _apply = new QPushButton(this);
    _apply->setIcon(QIcon::fromTheme("convert_gray_to_color"));
    _apply->setText(tr("Convert"));

    _cancel = new QPushButton(this);
    _cancel->setIcon(QIcon::fromTheme("edit-delete"));
    _cancel->setText(tr("Cancel"));

    _progress = new QProgressBar(this);

    formatLayout->addWidget(formatLabel);
    formatLayout->addWidget(_format);
    formatLayout->addWidget(compressionLabel);
    formatLayout->addWidget(_compression);
    formatLayout->addWidget(qualityLabel);
    formatLayout->addWidget(_quality);

    profileLayout->addWidget(profileLabel);
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

    QTimer::singleShot(1, this, SLOT(setupUI()));
}

void CyanBatchWidget::setupUI()
{
    _quality->setRange(0, 100);
    _quality->setValue(100);
    reloadSettings();
}

void CyanBatchWidget::populateIntent(CyanCommon::RenderingIntent intent)
{
    _intent->clear();
    _intent->addItem(QIcon::fromTheme("video-display"), tr("Undefined"),
                     CyanCommon::UndefinedRenderingIntent);
    _intent->addItem(QIcon::fromTheme("video-display"), tr("Saturation"),
                     CyanCommon::SaturationRenderingIntent);
    _intent->addItem(QIcon::fromTheme("video-display"), tr("Perceptual"),
                     CyanCommon::PerceptualRenderingIntent);
    _intent->addItem(QIcon::fromTheme("video-display"), tr("Absolute"),
                     CyanCommon::AbsoluteRenderingIntent);
    _intent->addItem(QIcon::fromTheme("video-display"), tr("Relative"),
                     CyanCommon::RelativeRenderingIntent);
    _intent->setCurrentIndex(intent);
}

void CyanBatchWidget::populateIntentFromSettings()
{
    QSettings settings;
    settings.beginGroup(QString("color"));
    int intent = settings.value(QString("intent"),
                                CyanCommon::PerceptualRenderingIntent).toInt();
    settings.endGroup();
    settings.beginGroup(QString("batch"));
    if (settings.value(QString("blackpoint")).isValid()) {
        intent = settings.value(QString("intent"),
                                CyanCommon::PerceptualRenderingIntent).toInt();
    }
    settings.endGroup();
    populateIntent(static_cast<CyanCommon::RenderingIntent>(intent));
}

void CyanBatchWidget::populateBlackPointFromSettings()
{
    QSettings settings;
    settings.beginGroup(QString("color"));
    bool blackPointState = settings.value(QString("blackpoint"), true).toBool();
    settings.endGroup();
    settings.beginGroup(QString("batch"));
    if (settings.value(QString("blackpoint")).isValid()) {
        blackPointState = settings.value(QString("blackpoint"), true).toBool();
    }
    settings.endGroup();
    _blackpoint->setChecked(blackPointState);
}

void CyanBatchWidget::populateFormat(CyanCommon::OutputFormatConverter format)
{
    _format->clear();
    _format->addItem(QIcon::fromTheme("document-new"), tr("TIFF"),
                     CyanCommon::OutputFormatConverterTiff);
    _format->addItem(QIcon::fromTheme("document-new"), tr("JPEG"),
                     CyanCommon::OutputFormatConverterJpeg);
    _format->addItem(QIcon::fromTheme("document-new"), tr("PSD"),
                     CyanCommon::OutputFormatConverterPsd);
    _format->setCurrentIndex(format);
}

void CyanBatchWidget::populateProfiles(const QString &defaultProfile,
                                       Magick::ColorspaceType colorspace)
{
    _profile->clear();
    int index = 1;
    _profile->addItem(QIcon::fromTheme("color-wheel"), tr("Select ..."));
    QMapIterator<QString, QString> profiles(CyanCommon::getColorProfiles(colorspace));
    while (profiles.hasNext()) {
        profiles.next();
        _profile->addItem(QIcon::fromTheme("color-wheel"), profiles.key(), profiles.value());
        if (profiles.value() == defaultProfile && !defaultProfile.isEmpty()) {
            _profile->setCurrentIndex(index);
        }
        index++;
    }
}

void CyanBatchWidget::populateProfilesFromSettings(Magick::ColorspaceType colorspace)
{
    QString profile;
    QSettings settings;
    settings.beginGroup(QString("color"));
    switch (colorspace) {
    case Magick::CMYKColorspace:
        profile = settings.value(QString("cmyk_profile")).toString();
        break;
    case Magick::GRAYColorspace:
        profile = settings.value(QString("gray_profile")).toString();
        break;
    default:
        profile = settings.value(QString("rgb_profile")).toString();
        ;
    }
    settings.endGroup();
    settings.beginGroup(QString("batch"));
    QString batchProfile = settings.value(QString("profile")).toString();
    settings.endGroup();
    if (!batchProfile.isEmpty()) { profile = batchProfile; }
    populateProfiles(profile, colorspace);
}

void CyanBatchWidget::reloadSettings()
{
    populateIntentFromSettings();
    populateBlackPointFromSettings();
    populateProfilesFromSettings();
    populateFormat();
}

void CyanBatchWidget::handleViewToggled(bool checked)
{
    qDebug() << "TOGGLE?" << checked;
    qDebug() << "VISIBLE?" << isVisible() << !visibleRegion().isEmpty();
    if (checked && (!isVisible() || visibleRegion().isEmpty()) ) {
        qDebug() << "SHOW WIDGET";
        show();
        emit changedVisibleState(checked);
    }
    else if (!checked && (isVisible() || !visibleRegion().isEmpty())) {
        qDebug() << "HIDE WIDGET";
        hide();
        emit changedVisibleState(checked);
    } else {
        qDebug() << "IGNORE CHANGE STATE";
        emit changedVisibleState((isVisible()&&!visibleRegion().isEmpty()));
    }
}

bool CyanBatchWidget::isBusy()
{
    return _working;
}
