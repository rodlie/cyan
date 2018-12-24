# Cyan

***v2.0 is currently under heavy development, please use v1.2 https://github.com/rodlie/cyan/tree/1.2***

## About

Cyan is an open source cross-platform image editor. Originally designed for prepress color convertion, but now aims to be a general-purpose image editor with a light footprint.

## Requirements

 * QtWidgets
 * QtConcurrent
 * lcms
 * zlib
 * bzlib
 * xz/lzma
 * libtiff
 * libjpeg
 * libpng
 * ffmpeg
 * latest ImageMagick6 with [patch](https://github.com/rodlie/ImageMagick6/commit/dffafb716da8d31f41a2e28e1b43a74d0edeaef3) (available as submodule)

## Build

```
git clone --recurse-submodules https://github.com/rodlie/cyan
cd cyan
mkdir build
cd build
qmake ..
make
```
