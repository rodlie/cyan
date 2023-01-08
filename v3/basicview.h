/*
#
# Cyan Graphics - https://cyan.graphics - https://github.com/rodlie/cyan
#
# Copyright (c) Ole-André Rodlie <ole.andre.rodlie@gmail.com>. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
#
*/

#ifndef BASICVIEW_H
#define BASICVIEW_H

#include <QString>
#include <QList>
#include <QUrl>
#include <QByteArray>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QResizeEvent>

#include "engine.h"

namespace Cyan
{
    class BasicView : public QGraphicsView
    {
        Q_OBJECT

    public:
        explicit BasicView(QWidget* parent = nullptr,
                           bool fit = false,
                           bool native = false);
        bool isFit();

    signals:
        void resetZoom();
        void zoomChanged(double scaleX,
                         double scaleY);
        void dropped(const QList<QUrl> &urls);

    public slots:
        void setZoom(double scaleX,
                     double scaleY);
        void setFit(bool value);
        void setImage(const Engine::Image &image,
                      bool getDetails = true,
                      bool getProfile = true);
        const QString getSourceDetails();
        const QByteArray getSourceProfile();

    private:
        bool _fit;
        bool _native;
        QString _sourceDetails;
        QByteArray _sourceProfile;

    protected:
        void wheelEvent(QWheelEvent* e);
        void mousePressEvent(QMouseEvent *e);
        void dragEnterEvent(QDragEnterEvent *e);
        void dragMoveEvent(QDragMoveEvent *e);
        void dragLeaveEvent(QDragLeaveEvent *e);
        void dropEvent(QDropEvent *e);
        void resizeEvent(QResizeEvent *e);
    };
}

#endif // BASICVIEW_H
