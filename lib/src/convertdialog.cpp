#include "convertdialog.h"

#include <QIcon>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

#include "common.h"

ConvertDialog::ConvertDialog(QWidget *parent,
                             const QString &title,
                             const QString &filename,
                             Magick::ColorspaceType colorspace) :
    QDialog (parent)
  , _filename(filename)
  , _colorspace(colorspace)
  , _ok(nullptr)
  , _cancel(nullptr)
  , _box(nullptr)
{
    setWindowTitle(title);
    setWindowIcon(QIcon(":/icons/colors.png"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    _ok = new QPushButton(this);
    _cancel = new QPushButton(this);

    _ok->setText(tr("Assign"));
    _cancel->setText(tr("Cancel"));

    QWidget *buttonWidget = new QWidget(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);

    buttonLayout->addWidget(_ok);
    buttonLayout->addWidget(_cancel);

    _box = new QComboBox(this);

    mainLayout->addWidget(_box);
    mainLayout->addWidget(buttonWidget);
    mainLayout->addStretch();

    populateProfiles();

    connect(_ok,
            SIGNAL(released()),
            this,
            SLOT(handleOk()));
    connect(_cancel,
            SIGNAL(released()),
            this,
            SLOT(handleCancel()));
}

const QString ConvertDialog::getProfile()
{
    return _box->currentData().toString();
}

void ConvertDialog::populateProfiles()
{
    _box->clear();
    int index = -1;
    int c = 0;
    QMapIterator<QString, QString> i(Common::getColorProfiles(_colorspace));
    while (i.hasNext()) {
        i.next();
        _box->addItem(i.key(),
                      i.value());
        if (i.value() == _filename) { index = c; }
        c++;
    }
    _box->setCurrentIndex(index);
}

void ConvertDialog::handleOk()
{
    QDialog::accept();
}

void ConvertDialog::handleCancel()
{
    QDialog::reject();
}
