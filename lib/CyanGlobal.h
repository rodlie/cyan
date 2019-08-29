/*
#
# Cyan <https://cyan.fxarena.net>
# Copyright Ole-André Rodlie, FxArena DA.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
*/

#ifndef CYANGLOBAL_H
#define CYANGLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CYAN_LIBRARY)
#  define CYANSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CYANSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // CYANGLOBAL_H
