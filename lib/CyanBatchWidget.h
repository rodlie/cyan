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
    bool _working;

signals:
    void changedVisibleState(bool checked);

private slots:
    void setupUI();
    void populateIntent(CyanCommon::RenderingIntent intent = CyanCommon::UndefinedRenderingIntent);
    void populateIntentFromSettings();
    void populateBlackPointFromSettings();
    void populateFormat(CyanCommon::OutputFormatConverter format = CyanCommon::OutputFormatConverterTiff);
    void populateProfiles(const QString &defaultProfile = QString(),
                          Magick::ColorspaceType colorspace = Magick::UndefinedColorspace);
    void populateProfilesFromSettings(Magick::ColorspaceType colorspace = Magick::UndefinedColorspace);
    void reloadSettings();

public slots:
    void handleViewToggled(bool checked);
    bool isBusy();
};

#endif // CYANBATCHWIDGET_H
