#ifndef INTERFACES_H
#define INTERFACES_H

#include <QtPlugin>
#include <Magick++.h>

class QString;
class QStringList;

class FilterInterface
{
public:
    virtual ~FilterInterface() {}
    virtual QStringList filters() const = 0;
    virtual Magick::Image filterImage(const QString &filter, const Magick::Image &image) = 0;
};

Q_DECLARE_INTERFACE(FilterInterface, "org.lumina-desktop.viewer.FilterInterface/1.0")


#endif // INTERFACES_H
