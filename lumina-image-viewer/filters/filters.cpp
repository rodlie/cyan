#include "filters.h"
#include <QDebug>
#include <QDialog>
#include "filterdialog.h"
#include "common.h"

QStringList ImageFilters::filters() const
{
    return QStringList() << tr("Edge") << tr("Charcoal") << tr("Emboss") << tr("Flip") << tr("Flop") << tr("Wave") << SWIRL_FILTER;
}

Magick::Image ImageFilters::filterImage(const QString &filter, const Magick::Image &image)
{
    Magick::Image result = image;
    try {
        if (filter == tr("Edge")) {
            result.edge();
        } else if (filter == tr("Charcoal")) {
            result.charcoal();
        } else if (filter == tr("Emboss")) {
            result.emboss();
        } else if (filter == tr("Flip")) {
            result.flip();
        } else if (filter == tr("Flop")) {
            result.flop();
        } else if (filter == tr("Wave")) {
            result.wave();
        } else if (filter == SWIRL_FILTER) {
            result = swirlImage(image);
        }
    }
    catch(Magick::Error &error_ ) {
        qDebug() << error_.what();
    }
    catch(Magick::Warning &warn_ ) {
        qDebug() << warn_.what();
    }
    return result;
}

Magick::Image ImageFilters::swirlImage(const Magick::Image &image)
{
    Magick::Image result = image;
    int swirl = 0;
    int def = swirl;

    filterOptions options;
    options.name = SWIRL_FILTER;
    options.effect = SWIRL_EFFECT;
    options.option[SWIRL_DEGREES] = SWIRL_DEGREES_VALUE;
    options.min[SWIRL_DEGREES] = SWIRL_DEGREES_MIN;
    options.max[SWIRL_DEGREES] = SWIRL_DEGREES_MAX;
    options.type[SWIRL_DEGREES] = SWIRL_DEGREES_TYPE;

    Dialog *dialog = new Dialog(NULL, result, options);
    dialog->exec();
    if (swirl != def) { result.swirl(swirl); }
    return result;
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(imageFilters, ImageFilters);
#endif
