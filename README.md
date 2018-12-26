# Cyan

***v2.0 is currently under heavy development, please use v1.2 https://github.com/rodlie/cyan/tree/1.2***

## About

Cyan is an open source cross-platform image editor. Originally designed for prepress color convertion, but now aims to be a general-purpose image editor. v2 started from scratch so it might take a while to get to the level of features you might expect from an image editor. ETA for v2 is January 2019, but that may at any time change.

## Features

* Supports any image format supported by ImageMagick
  * Images with layers are also supported
* Supports any video format supported by FFMpeg (as image or layer)
  * Audio files with embedded images ("coverart") are also supported
* Color convertion using Little CMS (to and from RGB/CMYK/GRAY)
  * Convert images/layers
  * Assign profiles
  * Extract profiles
  * Edit profiles
* Project files uses [MIFF](https://imagemagick.org/script/miff.php)
  * Supports high compression and is compatible with all ImageMagick tools
* MDI (Multiple Document Interface) interface
* Flexible GUI (almost everything can be customized to fit preferred workflow)
  * Layout presets will be added in the future
* 16-bit image support (with optional 32-bit support)
* Layer support
  * Currently missing several features
  * SubLayers will be added in the future
* Basic brush support
  * More advanced features will be added in the future
* Work in RGB(A), CMYK(A) and GRAY.

## Requirements

 * QtWidgets 5
 * QtConcurrent 5
 * lcms 2
 * ffmpeg 3 (disable with ``CONFIG+=no_ffmpeg``)
 * ImageMagick 7.0.8-20+

## Build

Supported build options:
 * ``CONFIG+=no_ffmpeg`` - disable ffmpeg support
 * ``PREFIX=</usr/local>`` - installation folder
 * ``DOCDIR=<PREFIX/share/doc>`` - documentation folder
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
./editor/build/Cyan
```

Package build:
```
qmake PREFIX=/usr ..
make
make INSTALL_ROOT=<pkg_path> install
```
