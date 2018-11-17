#
# Cyan <http://prepress.sf.net> <https://cyan.fxarena.net>,
# Copyright (C) 2016, 2017, 2018 Ole-André Rodlie<ole.andre.rodlie@gmail.com>
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

CONFIG += c++11
QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig
PKGCONFIG += lcms2
MAGICK_CONFIG = ImageMagick++
!isEmpty(MAGICK): MAGICK_CONFIG = $${MAGICK}
PKG_CONFIG_BIN = pkg-config
!isEmpty(CUSTOM_PKG_CONFIG): PKG_CONFIG_BIN = $${CUSTOM_PKG_CONFIG}

PKGCONFIG += $${MAGICK_CONFIG}
LIBS += `$${PKG_CONFIG_BIN} --libs --static $${MAGICK_CONFIG}`

isEmpty(PREFIX): PREFIX = /usr/local
isEmpty(DOCDIR): DOCDIR = $$PREFIX/share/doc
isEmpty(MANDIR): MANDIR = $$PREFIX/share/man
