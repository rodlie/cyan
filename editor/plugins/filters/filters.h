/*
#
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
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
#
*/

#ifndef IMAGEFILTERS_H
#define IMAGEFILTERS_H

#include <QObject>
#include <QStringList>
#include <Magick++.h>
#include "interfaces.h"

class ImageFilters : public QObject, public FilterInterface
{
    Q_OBJECT
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        Q_PLUGIN_METADATA(IID "org.lumina-desktop.viewer.FilterInterface")
#endif
    Q_INTERFACES(FilterInterface)

public:
    QStringList filters() const;
    Magick::Image filterImage(const QString &filter, const Magick::Image &image);
private:
    Magick::Image filteredImage;
private slots:
    Magick::Image swirlImage(const Magick::Image &image);
    Magick::Image waveImage(const Magick::Image &image);
    void handleFilterDialogDone();
};

#endif
