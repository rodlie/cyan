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

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

option(USE_PKGCONFIG "USE_PKGCONFIG" ON)
option(USE_Q32 "USE Q32" OFF)

if(MINGW OR APPLE)
    find_package(OpenMP)
endif()
if(MINGW)
    find_package(Threads)
endif()

if(USE_PKGCONFIG)
    find_package(PkgConfig)
    pkg_search_module(MAGICK REQUIRED ImageMagick++)
    pkg_search_module(LCMS2 REQUIRED lcms2)
    add_compile_options(${MAGICK_CFLAGS})
    #add_compile_options(${MAGICK_STATIC_CFLAGS})
    include_directories(${MAGICK_INCLUDE_DIRS} ${LCMS2_INCLUDE_DIRS})
else()
    find_package(LCMS2 REQUIRED)
    include_directories(${LCMS2_INCLUDE_DIR})
    find_package(ImageMagick COMPONENTS MagickCore REQUIRED)
    find_package(ImageMagick COMPONENTS MagickWand REQUIRED)
    find_package(ImageMagick COMPONENTS Magick++ REQUIRED)
    include_directories(${ImageMagick_INCLUDE_DIRS})
    add_compile_options(${MAGICK_CFLAGS})
    if(USE_Q32)
        add_definitions(-DMAGICKCORE_QUANTUM_DEPTH=32)
    else()
        add_definitions(-DMAGICKCORE_QUANTUM_DEPTH=16)
    endif()
        add_definitions(-DMAGICKCORE_HDRI_ENABLE=1)
endif()
