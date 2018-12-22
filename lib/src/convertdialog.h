#ifndef CONVERTDIALOG_H
#define CONVERTDIALOG_H

#include <QObject>
#include <QDialog>
#include <QPushButton>
#include <QComboBox>

#include <Magick++.h>

class ConvertDialog : public QDialog
{
    Q_OBJECT

public:

    ConvertDialog(QWidget *parent = nullptr,
                  const QString &title = QObject::tr("Assign Color Profile"),
                  const QString &filename = QString(),
                  Magick::ColorspaceType colorspace = Magick::sRGBColorspace);

public slots:

    const QString getProfile();

private:
    QString _filename;
    Magick::ColorspaceType _colorspace;
    QPushButton *_ok;
    QPushButton *_cancel;
    QComboBox *_box;

private slots:

    void populateProfiles();
    void handleOk();
    void handleCancel();
};

#endif // CONVERTDIALOG_H
