/*
# Cyan <http://prepress.sf.net> <https://cyan.fxarena.net>,
# Copyright (C) 2016, 2017, 2018 Ole-André Rodlie<ole.andre.rodlie@gmail.com>
#
# Cyan is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.
#
# Cyan is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Cyan.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>
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
    QAction *helpAction;
    bool ignoreAction;
    QProgressBar *progBar;

private slots:
    void readConfig();
    void writeConfig();

    void aboutCyan();

    void openImageDialog();
    void saveImageDialog();

    void openImage(QString file);
    void saveImage(QString file, bool notify = true, bool closeOnSave = false);

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

    void openHelp();

    void handleConvertWatcher();
    void handleReadWatcher();
};

#endif // CYAN_H
