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

#ifndef OPENLAYERDIALOG_H
#define OPENLAYERDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QString>
#include <QTreeWidget>
#include <QLabel>
#include <QTreeWidgetItem>
#include "FXX.h"

class OpenLayerDialog : public QDialog
{
    Q_OBJECT

public:
    OpenLayerDialog(QWidget *parent = Q_NULLPTR,
                  std::vector<Magick::Image> layers = std::vector<Magick::Image>());
    QPushButton *loadButton;
    QPushButton *closeButton;

signals:
    void loadLayer(Magick::Image image);

private:
    std::vector<Magick::Image> _layers;
    QTreeWidget *tree;
    QLabel *previewLabel;

private slots:
    void populateTree();
    void generateThumb(Magick::Image image);
    void viewLayer(QTreeWidgetItem* item, int col);
    void handleLoadLayer();
};
#endif // OPENLAYERDIALOG_H
