#include "filters.h"
#include <QDebug>
#include <magick/MagickCore.h>

QStringList ImageFilters::filters() const
{
    return QStringList() << tr("Grayscale");
}

Magick::Image ImageFilters::filterImage(const QString &filter, const Magick::Image &image)
{
    Magick::Image result = image;
    if (filter == tr("Grayscale")) {
        try {
            result.grayscale(Magick::UndefinedPixelIntensityMethod);
        }
        catch(Magick::Error &error_ ) {
            qDebug() << error_.what();
        }
        catch(Magick::Warning &warn_ ) {
            qDebug() << warn_.what();
        }
    }
    return result;
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(imageFilters, ImageFilters);
#endif
