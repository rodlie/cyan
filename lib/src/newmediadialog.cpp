#include "newmediadialog.h"

#include <QIcon>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

NewMediaDialog::NewMediaDialog(QWidget *parent,
                               QString title,
                               Common::newDialogType dialogType,
                               Magick::ColorspaceType colorspace, QSize size) :
    QDialog (parent)
  , _type(dialogType)
  , _width(nullptr)
  , _height(nullptr)
  , _ok(nullptr)
  , _cancel(nullptr)
  , _colorspace(colorspace)
  , _label(nullptr)
  , _select(nullptr)
  , _depth8(nullptr)
  , _depth16(nullptr)
{
    setWindowTitle(title);
    setWindowIcon(_type==Common::newImageDialogType?QIcon(":/icons/image.png"):QIcon(":/icons/layer.png"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    _label = new QLineEdit(this);
    _width  = new QSpinBox(this);
    _height = new QSpinBox(this);
    _ok = new QPushButton(this);
    _cancel = new QPushButton(this);
    _select = new QComboBox(this);
    _depth8 = new QRadioButton(this);
    _depth16 = new QRadioButton(this);

    QWidget *buttonWidget = new QWidget(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);

    QWidget *sizeWidget = new QWidget(this);
    QHBoxLayout *sizeLayout = new QHBoxLayout(sizeWidget);

    QWidget *depthWidget = new QWidget(this);
    QHBoxLayout *depthLayout = new QHBoxLayout(depthWidget);

    _width->setRange(1, 10000);
    _height->setRange(1, 10000);

    if (size.width()>0 && size.height()>0) {
        _width->setValue(size.width());
        _height->setValue(size.height());
    } else {
        _width->setValue(1024);
        _height->setValue(1024);
    }

    _depth8->setText(tr("8-bit"));
    _depth8->setChecked(true);
    _depth16->setText(tr("16-bit"));

    _label->setPlaceholderText(title);

    _ok->setText(tr("New"));
    _cancel->setText(tr("Cancel"));

    _select->addItem(tr("RGB"), 0);
    _select->addItem(tr("CMYK"), 1);
    _select->addItem(tr("GRAY"), 2);

    sizeLayout->addWidget(_width);
    sizeLayout->addWidget(_height);

    depthLayout->addWidget(_depth8);
    depthLayout->addWidget(_depth16);

    buttonLayout->addWidget(_ok);
    buttonLayout->addWidget(_cancel);

    mainLayout->addWidget(_label);
    mainLayout->addWidget(sizeWidget);
    mainLayout->addWidget(_select);
    mainLayout->addWidget(depthWidget);
    mainLayout->addWidget(buttonWidget);
    mainLayout->addStretch();

    if (_type == Common::newLayerDialogType) {
        _select->hide();
    }

    connect(_ok,
            SIGNAL(released()),
            this,
            SLOT(handleOk()));
    connect(_cancel,
            SIGNAL(released()),
            this,
            SLOT(handleCancel()));
}

NewMediaDialog::~NewMediaDialog()
{

}

Magick::Image NewMediaDialog::getImage()
{
    return _image;
}

void NewMediaDialog::handleOk()
{
    Magick::ColorspaceType type = _colorspace;
    if (_type == Common::newImageDialogType) {
        switch (_select->currentData().toInt()) {
        case 0:
            type = Magick::sRGBColorspace;
            break;
        case 1:
            type = Magick::CMYKColorspace;
            break;
        case 2:
            type = Magick::GRAYColorspace;
            break;
        }
    }
    createImage(QSize(_width->value(), _height->value()),
                type,
                _depth8->isChecked()?8:16);

    QDialog::accept();
}

void NewMediaDialog::handleCancel()
{
    QDialog::reject();
}

void NewMediaDialog::createImage(QSize geo,
                                 Magick::ColorspaceType colorspace,
                                 size_t depth)
{
    QString label = _label->text();
    if (label.isEmpty()) {
        label = windowTitle();
    }
    _image.size(Magick::Geometry(static_cast<size_t>(geo.width()),
                                 static_cast<size_t>(geo.height())));
    _image.colorSpace(colorspace);
    _image.depth(depth);
    _image.label(label.toStdString());
    _image.matte(true);
    _image.quantumOperator(Magick::AlphaChannel,
                           Magick::MultiplyEvaluateOperator,
                           0.0);
}
