#ifndef NEWMEDIADIALOG_H
#define NEWMEDIADIALOG_H

#include <QObject>
#include <QDialog>
#include <QSpinBox>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QRadioButton>

#include <Magick++.h>

#include "common.h"

class NewMediaDialog : public QDialog
{
    Q_OBJECT

public:

    NewMediaDialog(QWidget *parent = nullptr,
                   QString title = QObject::tr("New Image"),
                   Common::newDialogType dialogType = Common::newImageDialogType,
                   Magick::ColorspaceType colorspace = Magick::UndefinedColorspace,
                   QSize size = QSize(0,0));
    ~NewMediaDialog();

public slots:

    Magick::Image getImage();

private:

    Common::newDialogType _type;
    QSpinBox *_width;
    QSpinBox *_height;
    QPushButton *_ok;
    QPushButton *_cancel;
    Magick::Image _image;
    Magick::ColorspaceType _colorspace;
    QLineEdit *_label;
    QComboBox *_select;
    QRadioButton *_depth8;
    QRadioButton *_depth16;

private slots:

    void handleOk();
    void handleCancel();
    void createImage(QSize geo = QSize(1024, 1024),
                     Magick::ColorspaceType colorspace = Magick::sRGBColorspace,
                     size_t depth = 8);
};

#endif // NEWMEDIADIALOG_H
