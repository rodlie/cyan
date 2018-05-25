#ifndef VIEWER_H
#define VIEWER_H

#include <QMainWindow>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QResizeEvent>
#include <QGraphicsScene>
#include <QThread>

class ImageHandler : public QObject
{
    Q_OBJECT

public:
    explicit ImageHandler(QObject *parent = 0);
    ~ImageHandler();

signals:
    void returnImage(QByteArray image, QByteArray profile, QString filename);
    void errorMessage(QString filename, QString message);
    void warningMessage(QString filename, QString message);

public slots:
    void requestImage(QString filename);
    void readImage(QString filename);

private:
    QThread t;
};

class View : public QGraphicsView
{
    Q_OBJECT

public:
    explicit View(QWidget* parent = NULL);
    bool fit;

signals:
    void resetZoom();
    void myZoom(double scaleX, double scaleY);
    void myFit(bool value);
    void openImage(QString filename);
    void openProfile(QString filename);

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

class Viewer : public QMainWindow
{
    Q_OBJECT

public:
    Viewer(QWidget *parent = NULL);
    ~Viewer();

signals:
    void openImage(QString filename);

private:
    QToolBar *mainToolBar;
    QMenuBar *mainMenu;
    QStatusBar *mainStatusBar;
    View *mainView;
    QGraphicsScene *mainScene;

    QAction *openImageAct;
    QAction *saveImageAct;
    QAction *quitAct;

    QByteArray imageData;
    QByteArray imageProfile;
    QString imageFilename;

    ImageHandler *imageBackend;

private slots:
    void setupUI();
    void saveSettings();
    void loadSettings();
    void saveImage(QString filename);
    void loadImage(QString filename);
    void saveImageDialog();
    void loadImageDialog();
    void handleImage(QByteArray image, QByteArray profile, QString filename);
    void handleError(QString filename, QString message);
    void handleWarning(QString filename, QString message);
    void clearImage();
    void resetImageZoom();
    void viewImage();
};

#endif // VIEWER_H
