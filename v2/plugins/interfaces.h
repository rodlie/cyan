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
