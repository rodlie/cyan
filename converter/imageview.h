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

#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QGraphicsView>
#include <QString>
#include <QWheelEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QResizeEvent>

class ImageView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ImageView(QWidget* parent = Q_NULLPTR);
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
#endif // IMAGEVIEW_H
