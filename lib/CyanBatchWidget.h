#ifndef CYANBATCHWIDGET_H
#define CYANBATCHWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>

#include "CyanCommon.h"

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
    QComboBox *_profile;
    QComboBox *_intent;
    QCheckBox *_blackpoint;
    QComboBox *_format;
    QComboBox *_compression;
    QSpinBox *_quality;

private slots:
    void setupUI();
    void populateIntent(CyanCommon::RenderingIntent intent = CyanCommon::UndefinedRenderingIntent);
    void populateIntentFromSettings();
    void populateFormat(CyanCommon::OutputFormatConverter format = CyanCommon::OutputFormatConverterTiff);
    void populateProfiles(const QString &defaultProfile = QString(),
                          Magick::ColorspaceType colorspace = Magick::sRGBColorspace);
    void populateProfilesFromSettings(Magick::ColorspaceType colorspace = Magick::sRGBColorspace);
};

#endif // CYANBATCHWIDGET_H
