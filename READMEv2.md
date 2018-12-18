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

 ***Do not use or distribute Cyan if any of the unit tests fails!***

*This software is governed by the CeCILL license under French law and abiding by the rules of distribution of free software. You can use, modify and / or redistribute the software under the terms of the CeCILL license as circulated by CEA, CNRS and INRIA at the following URL https://www.cecill.info.*

![GitHub top language](https://img.shields.io/github/languages/top/rodlie/cyan.svg) ![GitHub language count](https://img.shields.io/github/languages/count/rodlie/cyan.svg) ![SourceForge](https://img.shields.io/sourceforge/dm/prepress.svg) 
![GitHub Releases](https://img.shields.io/github/downloads/rodlie/cyan/latest/total.svg) ![Travis (.org)](https://img.shields.io/travis/rodlie/cyan.svg) ![GitHub last commit](https://img.shields.io/github/last-commit/rodlie/cyan.svg) ![GitHub issues](https://img.shields.io/github/issues-raw/rodlie/cyan.svg) ![GitHub closed issues](https://img.shields.io/github/issues-closed/rodlie/cyan.svg) [![GitHub release](https://img.shields.io/github/release/rodlie/cyan.svg)](https://github.com/rodlie/cyan/releases) [![Github commits (since latest release)](https://img.shields.io/github/commits-since/rodlie/cyan/latest.svg)](https://github.com/rodlie/cyan)
