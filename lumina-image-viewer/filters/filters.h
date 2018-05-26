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
private slots:
    Magick::Image swirlImage(const Magick::Image &image);
};

#endif
