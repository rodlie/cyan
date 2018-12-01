# Copyright or © or Copr. Ole-André Rodlie, INRIA.
#
# ole.andre.rodlie@gmail.com
#
# This software is governed by the CeCILL license under French law and
# abiding by the rules of distribution of free software. You can use,
# modify and / or redistribute the software under the terms of the CeCILL
# license as circulated by CEA, CNRS and INRIA at the following URL
# "https://www.cecill.info".
#
# As a counterpart to the access to the source code and rights to
# modify and redistribute granted by the license, users are provided only
# with a limited warranty and the software's author, the holder of the
# economic rights and the subsequent licensors have only limited
# liability.
#
# In this respect, the user's attention is drawn to the associated risks
# with loading, using, modifying and / or developing or reproducing the
# software by the user in light of its specific status of free software,
# that can mean that it is complicated to manipulate, and that also
# so that it is for developers and experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to test and test the software's suitability
# Requirements in the conditions of their systems
# data to be ensured and, more generally, to use and operate
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL license and that you accept its terms.

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
