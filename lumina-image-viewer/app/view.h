#ifndef VIEW_H
#define VIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QResizeEvent>
#include <QGraphicsScene>
#include <Magick++.h>

class View : public QGraphicsView
{
    Q_OBJECT

public:
    explicit View(QWidget* parent = NULL);
    bool fit;

private:
    Magick::Image _image;
    QGraphicsScene *_scene;

signals:
    void resetZoom();
    void myZoom(double scaleX, double scaleY);
    void myFit(bool value);
    void openImage(QString filename);
    void openProfile(QString filename);

public slots:
    void doZoom(double scaleX, double scaleY);
    void setFit(bool value);
    void resetImageZoom();
    Magick::Image getImage();
    void setImage(Magick::Image image);

private slots:
    void viewImage();
    Magick::Blob makePreview();
protected:
    void wheelEvent(QWheelEvent* event);
    void mousePressEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void resizeEvent(QResizeEvent *event);
};

#endif // VIEW_H
