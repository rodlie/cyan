/*
# Copyright Ole-André Rodlie.
#
# ole.andre.rodlie@gmail.com
#
# This software is governed by the CeCILL license under French law and
# abiding by the rules of distribution of free software. You can use,
# modify and / or redistribute the software under the terms of the CeCILL
# license as circulated by CEA, CNRS and INRIA at the following URL
# "https://www.cecill.info".
#
# As a counterpart to the access to the source code and rights to
# modify and redistribute granted by the license, users are provided only
# with a limited warranty and the software's author, the holder of the
# economic rights and the subsequent licensors have only limited
# liability.
#
# In this respect, the user's attention is drawn to the associated risks
# with loading, using, modifying and / or developing or reproducing the
# software by the user in light of its specific status of free software,
# that can mean that it is complicated to manipulate, and that also
# so that it is for developers and experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to test and test the software's suitability
# Requirements in the conditions of their systems
# data to be ensured and, more generally, to use and operate
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL license and that you accept its terms.
*/

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
