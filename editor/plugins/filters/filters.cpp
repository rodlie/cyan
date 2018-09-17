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

#include "filters.h"
#include <QDebug>
#include <QDialog>
#include <QApplication>
#include "filterdialog.h"
#include "common.h"

QStringList ImageFilters::filters() const
{
    return QStringList() << WAVE_FILTER << SWIRL_FILTER;
}

Magick::Image ImageFilters::filterImage(const QString &filter, const Magick::Image &image)
{
    Magick::Image result = image;
    try {
        if (filter == WAVE_FILTER) {
            result = waveImage(image);
        } else if (filter == SWIRL_FILTER) {
            result = swirlImage(image);
        }
    }
    catch(Magick::Error &error_ ) {
        qDebug() << error_.what(); // TODO
    }
    catch(Magick::Warning &warn_ ) {
        qDebug() << warn_.what(); // TODO
    }
    return result;
}

Magick::Image ImageFilters::swirlImage(const Magick::Image &image)
{
    filteredImage = image;

    filterOptions options;
    options.name = SWIRL_FILTER;
    options.effect = SWIRL_EFFECT;
    options.option[SWIRL_DEGREES] = SWIRL_DEGREES_VALUE;
    options.min[SWIRL_DEGREES] = SWIRL_DEGREES_MIN;
    options.max[SWIRL_DEGREES] = SWIRL_DEGREES_MAX;
    options.type[SWIRL_DEGREES] = SWIRL_DEGREES_TYPE;

    Dialog *dialog = new Dialog(NULL, image, options);
    connect(dialog, SIGNAL(filterDone()), this, SLOT(handleFilterDialogDone()));
    dialog->exec();

    return filteredImage;
}

Magick::Image ImageFilters::waveImage(const Magick::Image &image)
{
    filteredImage = image;

    filterOptions options;
    options.name = WAVE_FILTER;
    options.effect = WAVE_EFFECT;

    options.option[WAVE_AMPLITUDE] = WAVE_AMPLITUDE_VALUE;
    options.min[WAVE_AMPLITUDE] = WAVE_AMPLITUDE_MIN;
    options.max[WAVE_AMPLITUDE] = WAVE_AMPLITUDE_MAX;
    options.type[WAVE_AMPLITUDE] = WAVE_AMPLITUDE_TYPE;

    options.option[WAVE_LENGTH] = WAVE_LENGTH_VALUE;
    options.min[WAVE_LENGTH] = WAVE_LENGTH_MIN;
    options.max[WAVE_LENGTH] = WAVE_LENGTH_MAX;
    options.type[WAVE_LENGTH] = WAVE_LENGTH_TYPE;

    Dialog *dialog = new Dialog(NULL, image, options);
    connect(dialog, SIGNAL(filterDone()), this, SLOT(handleFilterDialogDone()));
    dialog->exec();

    return filteredImage;
}

void ImageFilters::handleFilterDialogDone()
{
    Dialog *dialog = qobject_cast<Dialog*>(sender());
    if (!dialog) { return; }
    filteredImage = dialog->filteredImage;
    disconnect(dialog);
    dialog->close();
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(imageFilters, ImageFilters);
#endif
