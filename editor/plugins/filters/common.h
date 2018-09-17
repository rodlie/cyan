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

#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QMap>
#include <QVariant>

struct filterOptions {
    QString name;
    int effect;
    QMap<QString, QVariant> option;
    QMap<QString, QVariant> min;
    QMap<QString, QVariant> max;
    QMap<QString, int> type;
};
Q_DECLARE_METATYPE(filterOptions);

enum filterTypes {
    filterTypeNone,
    filterTypeIntSpinBox,
    filterTypeDoubleSpinBox
};

enum filterEffect {
    filterEffectNone,
    filterEffectSwirl,
    filterEffectWave
};

// Swirl
#define SWIRL_FILTER tr("Distort/Swirl")
#define SWIRL_EFFECT filterEffectSwirl
#define SWIRL_DEGREES tr("Degrees")
#define SWIRL_DEGREES_VALUE 45
#define SWIRL_DEGREES_MIN -360
#define SWIRL_DEGREES_MAX 360
#define SWIRL_DEGREES_TYPE filterTypeDoubleSpinBox

// Wave
#define WAVE_FILTER tr("Distort/Wave")
#define WAVE_EFFECT filterEffectWave
#define WAVE_AMPLITUDE tr("Amplitude")
#define WAVE_AMPLITUDE_VALUE 25
#define WAVE_AMPLITUDE_MIN 0
#define WAVE_AMPLITUDE_MAX 1000
#define WAVE_AMPLITUDE_TYPE filterTypeDoubleSpinBox
#define WAVE_LENGTH tr("Length")
#define WAVE_LENGTH_VALUE 150
#define WAVE_LENGTH_MIN 0.1
#define WAVE_LENGTH_MAX 1000
#define WAVE_LENGTH_TYPE filterTypeDoubleSpinBox

#endif // COMMON_H
