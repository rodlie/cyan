# Cyan <http://prepress.sf.net> <https://cyan.fxarena.net>,
# Copyright (C) 2016, 2017, 2018 Ole-André Rodlie<ole.andre.rodlie@gmail.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License version 2.1 as published by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, see <http://www.gnu.org/licenses/lgpl-2.0.html>

cmake_minimum_required(VERSION 3.1)

if(MINGW)
    target_link_libraries(${PROJECT_NAME} Threads::Threads)
endif()

if(USE_PKGCONFIG)
    target_link_libraries(${PROJECT_NAME} ${MAGICK_LIBRARIES} ${MAGICK_LDFLAGS})
    #target_link_libraries(${PROJECT_NAME} ${MAGICK_STATIC_LIBRARIES} ${MAGICK_STATIC_LDFLAGS})
    target_link_libraries(${PROJECT_NAME} ${LCMS2_LIBRARIES})
else()
    target_link_libraries(${PROJECT_NAME} ${LCMS2_LIBRARY})
    target_link_libraries(${PROJECT_NAME} ${ImageMagick_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} ${ImageMagick_MagickCore_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} ${ImageMagick_MagickWand_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} ${ImageMagick_Magick++_LIBRARIES})
endif()
