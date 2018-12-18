# Cyan

***Cyan v2.0 is current under development, please use v1.2 https://github.com/rodlie/cyan/tree/1.2***

## About
Cyan is an open source cross-platform raster image editor. Originally designed for prepress color work, but now aims to be a general-purpose image editor with a light footprint.

## Build

Cyan requires the following third-party software installed before build:

 * [Qt(Widgets/Concurrent)](https://www.qt.io/) 5.x
 * [Lcms2](http://www.littlecms.com/) 2.x
 * [libtiff](http://www.simplesystems.org/libtiff/) 4.0.x
 * [libjpeg](https://www.ijg.org/) 9
 * [libpng](http://www.libpng.org/pub/png/libpng.html) 1.6.x
 * zlib
 * bz2
 * xz

Doing a regular build:
```
mkdir build && cd build
qmake ..
make && make test
./Cyan
```
