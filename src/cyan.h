/*
* Cyan <http://cyan.fxarena.net> <https://github.com/olear/cyan>,
* Copyright (C) 2016, 2017 Ole-André Rodlie
*
* Cyan is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as published
* by the Free Software Foundation.
*
* Cyan is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Cyan.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>
*/

#ifndef CYAN_H
#define CYAN_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QToolBar>
#include <QComboBox>
#include <QCheckBox>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QScrollBar>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPushButton>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QByteArray>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QProgressBar>

#include "yellow.h"
#include "magenta.h"

#ifdef Q_OS_UNIX
#ifndef Q_OS_MAC
#include "gamma.h"
#endif
#endif

class CyanView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit CyanView(QWidget* parent = NULL);
    bool fit;

signals:
    void resetZoom();
    void myZoom(double scaleX, double scaleY);
    void proof();
    void myFit(bool value);
    void openImage(QString file);
    void openProfile(QString file);

public slots:
    void doZoom(double scaleX, double scaleY);
    void setFit(bool value);

protected:
    void wheelEvent(QWheelEvent* event);
    void mousePressEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void resizeEvent(QResizeEvent *event);
};

class CyanProfile : public QDialog
{
    Q_OBJECT

public:
    CyanProfile(QWidget *parent = 0);
    ~CyanProfile();
    QLineEdit *profileFileName;
    QLineEdit *profileDescription;
    QLineEdit *profileCopyright;
    QPushButton *profileSaveButton;
    QPushButton *profileCloseButton;

private slots:
    void closeDialog();
};

class Cyan : public QMainWindow
{
    Q_OBJECT

public:
    Cyan(QWidget *parent = 0);
    ~Cyan();

private:
    Yellow cms;
    Magenta proc;
#ifdef Q_OS_UNIX
#ifndef Q_OS_MAC
    Gamma gamma;
#endif
#endif
    QGraphicsScene *scene;
    CyanView *view;
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
    QByteArray currentImageData;
    QByteArray currentImageProfile;
    QByteArray currentImageNewProfile;
    QAction *exportEmbeddedProfileAction;
    QComboBox *bitDepth;
    QString lockedSaveFileName;
    CyanProfile profileDialog;
    QDoubleSpinBox *cmyLevel;
    QDoubleSpinBox *kLevel;
    QLabel *cmyLevelLabel;
    QLabel *kLevelLabel;
    QLabel *inkDensity;
    QToolBar *cmykBar;
    QLabel *colorCyanMin;
    QLabel *colorCyanMax;
    QLabel *colorMagentaMin;
    QLabel *colorMagentaMax;
    QLabel *colorYellowMin;
    QLabel *colorYellowMax;
    QLabel *colorBlackMin;
    QLabel *colorBlackMax;
    QProgressBar *progBar;

private slots:
    void readConfig();
    void writeConfig();
    void aboutCyan();
    void openImageDialog();
    void saveImageDialog();
    void openImage(QString file);
    void saveImage(QString file);
    void getColorProfiles(int colorspace, QComboBox *box, bool isMonitor);
    void loadDefaultProfiles();
    void saveDefaultProfiles();
    void updateRgbDefaultProfile(int index);
    void updateCmykDefaultProfile(int index);
    void updateGrayDefaultProfile(int index);
    void updateMonitorDefaultProfile(int index);
    void getImage(magentaImage result);
    void imageClear();
    void resetImageZoom();
    void setImage(QByteArray image);
    void updateImage();
    QByteArray getMonitorProfile();
    QByteArray getOutputProfile();
    void getConvertProfiles();
    void inputProfileChanged(int);
    void outputProfileChanged(int);
    void enableUI();
    void disableUI();
    void exportEmbeddedProfileDialog();
    void exportEmbeddedProfile(QString file);
    bool imageModified();
    void handleSaveState();
    bool hasProfiles();
    bool hasRGB();
    bool hasCMYK();
    bool hasGRAY();
    void bitDepthChanged(int index);
    void gimpPlugin();
    void openProfile(QString file);
    void saveProfile();
    void handleConvertAdv();
    void renderingIntentUpdated(int);
    void blackPointUpdated(int);
};

#endif // CYAN_H
