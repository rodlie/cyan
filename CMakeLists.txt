cmake_minimum_required(VERSION 3.1)

option(BUILD_CC "Build Color Converter" ON)
option(BUILD_TST "Build Unit Tests" ON)
option(BUILD_LIB "Build FXX library" OFF)
option(BUILD_UTILS "Build utils" OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/fxx/cmake/modules/")

if(BUILD_CC)
    add_subdirectory(cc)
endif()

if(BUILD_TST)
    enable_testing()
    add_subdirectory(tests)
endif()

if(BUILD_LIB)
    add_subdirectory(fxx)
endif()

if(BUILD_UTILS)
    add_subdirectory(utils/geticc)
endif()
