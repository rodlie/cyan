# Copyright Ole-André Rodlie, INRIA.
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
enable_testing()

project(Cyan VERSION 1.2.1)

if(NOT PROJECT_VERSION_TWEAK)
    set(PROJECT_VERSION_TWEAK 0)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_definitions(-DCYAN_VERSION="${PROJECT_VERSION}")
add_definitions(-DCYAN_GIT="$ENV{GIT}")

set(SOURCES src/main.cpp src/cyan.cpp src/imageview.cpp src/profiledialog.cpp src/helpdialog.cpp src/openlayerdialog.cpp src/FXX.cpp res/cyan.qrc docs/docs.qrc)
set(HEADERS src/cyan.h src/imageview.cpp src/profiledialog.cpp src/helpdialog.cpp src/openlayerdialog.h src/FXX.h)
set(RESOURCE_FILES res/cyan.qrc docs/docs.qrc)
set(RESOURCE_FOLDER res)

set(TEST_SOURCES src/tst_cyan.cpp src/FXX.cpp res/tests.qrc)
set(TEST_HEADERS src/FXX.h)
set(TEST_RESOURCE_FILES res/tests.qrc)

set(COMPANY "Cyan")
set(COPYRIGHT "Copyright Ole-Andre Rodlie, INRIA. All rights reserved.")
set(IDENTIFIER "net.fxarena.cyan")

if(CMAKE_BUILD_TYPE MATCHES RELEASE)
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()

if(APPLE)
    set(ICON_FILE ${RESOURCE_FOLDER}/${PROJECT_NAME}.icns)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.7")
    set_source_files_properties(${PROJECT_NAME}.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
    set(MACOSX_BUNDLE_COPYRIGHT ${COPYRIGHT})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER ${IDENTIFIER})
    set(MACOSX_BUNDLE_ICON_FILE Cyan)
endif()

if(WIN32)
    set(ICON_FILE ${CC_RESOURCE_FOLDER}/cyan.ico)
endif()

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5Test REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../lib/cmake/modules/")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

option(USE_PKG_CONFIG "Use pkg-config" ON)
option(USE_Q32 "Q32 workaround if not using pkg-config" OFF)

if(MINGW OR APPLE)
    find_package(OpenMP)
endif()

if(MINGW)
    find_package(Threads)
endif()

if(USE_PKG_CONFIG)
    find_package(PkgConfig)
    pkg_search_module(MAGICK REQUIRED ImageMagick++)
    pkg_search_module(LCMS2 REQUIRED lcms2)
    add_compile_options(${MAGICK_CFLAGS})
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

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES_FILES})
add_executable(tests ${TEST_SOURCES} ${TEST_HEADERS} ${TEST_RESOURCES_FILES})

target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} Qt5::Concurrent)
target_link_libraries(tests Qt5::Test)

if(MINGW)
    target_link_libraries(${PROJECT_NAME} Threads::Threads)
endif()

if(USE_PKG_CONFIG)
    target_link_libraries(${PROJECT_NAME} ${MAGICK_LIBRARIES} ${LCMS2_LIBRARIES} ${MAGICK_LDFLAGS} ${LCMS2_LDFLAGS})
    target_link_libraries(tests ${MAGICK_LIBRARIES} ${LCMS2_LIBRARIES} ${MAGICK_LDFLAGS} ${LCMS2_LDFLAGS})
    #target_link_libraries(${PROJECT_NAME} ${MAGICK_STATIC_LIBRARIES} ${LCMS2_STATIC_LIBRARIES} ${MAGICK_STATIC_LDFLAGS} ${LCMS2_STATIC_LDFLAGS})
    #target_link_libraries(tests ${MAGICK_STATIC_LIBRARIES} ${LCMS2_STATIC_LIBRARIES} ${MAGICK_STATIC_LDFLAGS} ${LCMS2_STATIC_LDFLAGS})
else()
    target_link_libraries(${PROJECT_NAME} ${LCMS2_LIBRARY})
    target_link_libraries(${PROJECT_NAME} ${ImageMagick_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} ${ImageMagick_MagickCore_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} ${ImageMagick_MagickWand_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} ${ImageMagick_Magick++_LIBRARIES})
    target_link_libraries(tests ${LCMS2_LIBRARY})
    target_link_libraries(tests ${ImageMagick_LIBRARIES})
    target_link_libraries(tests ${ImageMagick_MagickCore_LIBRARIES})
    target_link_libraries(tests ${ImageMagick_MagickWand_LIBRARIES})
    target_link_libraries(tests ${ImageMagick_Magick++_LIBRARIES})
endif()

add_test(NAME tests COMMAND tests)

if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)
    install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(DIRECTORY ${RESOURCE_FOLDER}/hicolor DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons)
    install(FILES ${RESOURCE_FOLDER}/cyan.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
    install(FILES LICENSE DESTINATION ${CMAKE_INSTALL_DOCDIR}-${PROJECT_VERSION})
endif()
