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
    filterEffectSwirl
};

// Swirl
#define SWIRL_FILTER tr("Swirl")
#define SWIRL_EFFECT filterEffectSwirl
#define SWIRL_DEGREES tr("Degrees")
#define SWIRL_DEGREES_VALUE 45
#define SWIRL_DEGREES_MIN -360
#define SWIRL_DEGREES_MAX 360
#define SWIRL_DEGREES_TYPE filterTypeDoubleSpinBox


#endif // COMMON_H
