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
