/*
# Copyright or © or Copr. Ole-André Rodlie, INRIA.
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

#ifndef CYAN_H
#define CYAN_H

#include <QMainWindow>
#include <QToolBar>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QByteArray>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QObject>
#include <QDockWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMap>
#include <QThread>
#include <QFutureWatcher>

#include "imageview.h"
#include "profiledialog.h"
#include "FXX.h"

class Cyan : public QMainWindow
{
    Q_OBJECT

public:
    Cyan(QWidget *parent = Q_NULLPTR);
    ~Cyan();

private:
    QFutureWatcher<FXX::Image> convertWatcher;
    QFutureWatcher<FXX::Image> readWatcher;
    FXX fx;
    QGraphicsScene *scene;
    ImageView *view;
    QToolBar *mainBar;
    QToolBar *convertBar;
    QToolBar *profileBar;
    QComboBox *rgbProfile;
    QComboBox *cmykProfile;
    QComboBox *grayProfile;
    QComboBox *inputProfile;
    QComboBox *outputProfile;
    QComboBox *monitorProfile;
    QComboBox *renderingIntent;
    QCheckBox *blackPoint;
    QPushButton *mainBarLoadButton;
    QPushButton *mainBarSaveButton;
    QMenuBar *menuBar;
    QMenu *fileMenu;
    QMenu *helpMenu;
    QAction *openImageAction;
    QAction *saveImageAction;
    QAction *quitAction;
    QAction *exportEmbeddedProfileAction;
    QComboBox *bitDepth;
    QString lockedSaveFileName;
    FXX::Image imageData;
    QDockWidget *imageInfoDock;
    QTreeWidget *imageInfoTree;
    bool ignoreAction;
    QProgressBar *progBar;

private slots:
    void readConfig();
    void writeConfig();

    void aboutCyan();

    void openImageDialog();
    void saveImageDialog();

    void openImage(QString file);
    void saveImage(QString file,
                   bool notify = true,
                   bool closeOnSave = false);

    void getColorProfiles(FXX::ColorSpace colorspace,
                          QComboBox *box,
                          bool isMonitor = false);

    void loadDefaultProfiles();
    void saveDefaultProfiles();

    void updateRgbDefaultProfile(int index);
    void updateCmykDefaultProfile(int index);
    void updateGrayDefaultProfile(int index);
    void updateMonitorDefaultProfile(int index);

    void imageClear();

    void resetImageZoom();

    void setImage(QByteArray image);
    void updateImage();

    QByteArray getMonitorProfile();
    QByteArray getOutputProfile();
    QByteArray getInputProfile();
    QByteArray readColorProfile(QString file);

    void getConvertProfiles();

    void inputProfileChanged(int);
    void outputProfileChanged(int);

    void enableUI();
    void disableUI();

    void exportEmbeddedProfileDialog();
    void exportEmbeddedProfile(QString file);

    bool hasProfiles();
    bool hasRGB();
    bool hasCMYK();
    bool hasGRAY();

    void bitDepthChanged(int index);
    void gimpPlugin();

    void openProfile(QString file);

    void renderingIntentUpdated(int);
    void blackPointUpdated(int);

    int supportedDepth();
    void clearImageBuffer();
    void parseImageInfo();

    QMap<QString,QString> genProfiles(FXX::ColorSpace colorspace);
    QByteArray getDefaultProfile(FXX::ColorSpace colorspace);

    void handleConvertWatcher();
    void handleReadWatcher();
};

#endif // CYAN_H
