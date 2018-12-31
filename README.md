# Cyan

***v2.0 is currently under development, please use v1.2 https://github.com/rodlie/cyan/tree/1.2***

Cyan is an open source cross-platform image editor. Originally designed for prepress color convertion, but now aims to be a general-purpose image editor. v2 started from scratch so it might take a while to get to the level of features you might expect from an image editor. ETA for v2 is January 2019, but that may at any time change.

## Features

* Supports any image format supported by ImageMagick
* Color management using Little CMS
* Layers support
* Basic brush support

## Requirements

 * QtWidgets 5.6+
 * QtConcurrent 5.6+
 * lcms 2.x
 * ImageMagick 7.0.8-20+

## Build

Supported build options:
 * ``CONFIG+=with_ffmpeg`` - enable **experimental** ffmpeg support
 * ``PREFIX=</usr/local>`` - installation folder *(currently not used)*
 * ``DOCDIR=<PREFIX/share/doc>`` - documentation folder *(currently not used)*
 * ``MANDIR=<PREFIX/share/man>`` - manual folder *(currently not used)*
 * ``LIBDIR=<PREFIX/lib>`` - library folder *(currently not used)*

Normal build:
```
git clone https://github.com/rodlie/cyan
cd cyan
mkdir build
cd build
qmake ..
make
./src/build/Cyan
```

Package build (currently not available):
```
qmake PREFIX=/usr ..
make
make INSTALL_ROOT=<pkg_path> install
```
