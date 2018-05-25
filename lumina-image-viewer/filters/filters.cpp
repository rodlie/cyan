#include "filters.h"
#include <QDebug>

QStringList ImageFilters::filters() const
{
    return QStringList() << tr("Edge") << tr("Charcoal") << tr("Emboss") << tr("Flip") << tr("Flop") << tr("Wave") << tr("Swirl");
}

Magick::Image ImageFilters::filterImage(const QString &filter, const Magick::Image &image)
{
    Magick::Image result = image;
    try {
        if (filter == tr("Edge")) {
            result.edge();
        } else if (filter == "Charcoal") {
            result.charcoal();
        } else if (filter == "Emboss") {
            result.emboss();
        } else if (filter == "Flip") {
            result.flip();
        } else if (filter == "Flop") {
            result.flop();
        } else if (filter == "Wave") {
            result.wave();
        } else if (filter == "Swirl") {
            result.swirl(45);
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

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(imageFilters, ImageFilters);
#endif
