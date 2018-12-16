/*
# Copyright Ole-André Rodlie, INRIA.
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

#include "openlayerdialog.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDebug>

#define tW 320
#define tH 256

OpenLayerDialog::OpenLayerDialog(QWidget *parent, std::vector<Magick::Image> layers)
    : QDialog(parent)
    , loadButton(Q_NULLPTR)
    , closeButton(Q_NULLPTR)
    ,_layers(layers)
    , tree(Q_NULLPTR)
    , previewLabel(Q_NULLPTR)
{
    setWindowTitle(tr("Open Image Layer?"));
    setWindowIcon(QIcon(":/cyan.png"));
    setAttribute(Qt::WA_DeleteOnClose, true);
    setMaximumWidth(tW*2);
    setMaximumHeight(tH+100);

    QFrame *containerFrame = new QFrame(this);
    QFrame *buttonFrame = new QFrame(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *containerLayout = new QHBoxLayout(containerFrame);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonFrame);

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(0);

    previewLabel = new QLabel(this);
    previewLabel->setStyleSheet("background-color: rgb(30,30,30);");
    previewLabel->setMaximumSize(QSize(tW, tH));
    previewLabel->setMinimumSize(QSize(tW, tH));
    previewLabel->setAlignment(Qt::AlignVCenter);

    loadButton = new QPushButton(this);
    loadButton->setText(tr("Open"));
    loadButton->setIcon(QIcon(":/cyan-open.png"));

    closeButton = new QPushButton(this);
    closeButton->setText(tr("Cancel"));

    mainLayout->addWidget(containerFrame);
    mainLayout->addStretch();
    mainLayout->addWidget(buttonFrame);

    tree = new QTreeWidget(this);
    tree->setHeaderLabels(QStringList() << "#");
    tree->setMaximumWidth(tW);
    tree->setMaximumHeight(tH);
    tree->setMinimumHeight(tH);

    containerLayout->addWidget(previewLabel);
    containerLayout->addWidget(tree);

    buttonLayout->addStretch();
    buttonLayout->addWidget(loadButton);
    buttonLayout->addWidget(closeButton);

    connect(closeButton, SIGNAL(released()),
            this, SLOT(close()));
    connect(loadButton, SIGNAL(released()),
            this, SLOT(handleLoadLayer()));

    connect(tree, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(viewLayer(QTreeWidgetItem*,int)));

    populateTree();
}

void OpenLayerDialog::populateTree()
{
    tree->clear();
    for (int i=0;i<static_cast<int>(_layers.size());++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(tree);
        item->setText(0,QString::number(i));
        if (i==0) {
            tree->setCurrentItem(item);
            viewLayer(item, 0);
        }
    }
}

void OpenLayerDialog::generateThumb(Magick::Image image)
{
    std::vector<unsigned char> preview = FXX::generateThumb(image, tW, tH);
    if (preview.size()>0) {
        previewLabel->setPixmap(QPixmap::fromImage(QImage::fromData(preview.data(),
                                                                    static_cast<int>(preview.size()))));
    }
}

void OpenLayerDialog::viewLayer(QTreeWidgetItem *item, int col)
{
    Q_UNUSED(col)
    if (!item) { return; }
    generateThumb(_layers[static_cast<unsigned long>(item->text(0).toInt())]);
}

void OpenLayerDialog::handleLoadLayer()
{
    QTreeWidgetItem *item = tree->currentItem();
    if (!item) { return; }
    emit loadLayer(_layers[static_cast<unsigned long>(item->text(0).toInt())]);
    close();
}

