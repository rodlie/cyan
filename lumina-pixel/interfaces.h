/*
#
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#
*/

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
