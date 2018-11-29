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

cmake_minimum_required(VERSION 3.1)

option(BUILD_CONVERTER "Build Color Converter" ON)
option(BUILD_TESTS "Build Unit Tests" ON)
option(BUILD_LIB "Build shared library" OFF)
option(BUILD_UTILS "Build utils" OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/lib/cmake/modules/")

if(BUILD_CONVERTER)
    add_subdirectory(converter)
endif()

if(BUILD_UTILS)
    add_subdirectory(utils/geticc)
endif()

if(BUILD_LIB)
    add_subdirectory(lib)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
