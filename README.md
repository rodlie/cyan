# Cyan 2.0

[![GitHub license](https://img.shields.io/github/license/rodlie/cyan)](https://github.com/rodlie/cyan/blob/master/COPYING)
![Travis (.org)](https://img.shields.io/travis/rodlie/cyan)
![GitHub last commit](https://img.shields.io/github/last-commit/rodlie/cyan)

![screenshot](docs/images/screenshot.png)

Cyan was an prepress color converter (and [GIMP](https://gimp.org) plug-in) but now aims to be a general-purpose image editor based on [ImageMagick](https://imagemagick.org).

***Currently under early development, not usable yet! please use [v1.2](https://github.com/rodlie/cyan/tree/1.2). You can track development in our [ChangeLog](https://cyan.fxarena.net/ChangeLog).***

## Features

* Simple and lightweight
* Multiple Document Interface
* Supports RGB/CMYK/GRAY 8/16/32 bit images
* Supports ICC/ICM v2/v4 color profiles
* Supports PNG, JPEG, TIFF, EXR, XCF, PSD, MIFF and many more
  * Support for SVG and similar will be added in the future
* Supports TIFF, EXR, XCF, MIFF and PSD image layers
  * Support for Krita (KRA) and OpenRaster (ORA) will be added in the future
* Extract embedded ICC color profiles from images
* Embed ICC color profile to image(s)
* Edit ICC color profile tags
* Convert to and from RGB/CMYK/GRAY
* Over 40 different layer compositing modes

***Everything is subject to change during development of v2.***

## FAQ

...

## License

This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

* Color profiles are licensed under [CC Attribution-No Derivative Works 3.0](https://creativecommons.org/licenses/by-nd/3.0/)
* Icons are licensed under [CC Attribution 3.0 US](http://creativecommons.org/licenses/by/3.0/us/)
* Logo is licensed under [CC Attribution-ShareAlike 4.0 International](http://creativecommons.org/licenses/by-sa/4.0/)

## Build

### Requirements

 * Qt5 *(Widgets/Concurrent) 5.5+*
 * Little CMS 2
 * ImageMagick *(Magick++) 7.0.8+*
 * fontconfig

### RHEL/CentOS 7

On RHEL/CentOS you will need to install the following packages:

```
sudo yum install git gcc-c++ autoconf libpng-devel libjpeg-turbo-devel lcms2-devel OpenEXR-devel libwebp-devel pango-devel cairo-devel libtiff-devel xz-devel zlib-devel fontconfig-devel qt5-qtbase-devel
```

CMake 3.x is needed to build Cyan, install from EPEL:
```
sudo yum install epel-release
sudo yum install cmake3
```

***Remember to replace ``cmake`` with ``cmake3`` when running the build command further down.***

### Ubuntu Xenial/Bionic

On Ubuntu you will need to install the following packages:

```
sudo apt-get install git build-essential cmake pkg-config qtbase5-dev libfontconfig1-dev libcairo2-dev libpango1.0-dev libwebp-dev liblcms2-dev libopenexr-dev libjpeg-dev libpng-dev libtiff-dev liblzma-dev zlib1g-dev libopenjp2-7-dev
```

### Other distros

If your system have an up-to-date installation of ImageMagick then you only need ``qtbase``, ``lcms2`` and ``fontconfig``.

### Build Cyan

Now you can build Cyan, this command will also build and link a static version of ImageMagick.
```
git clone https://github.com/rodlie/cyan
cd cyan
MKJOBS=4 STATIC=1 sh share/scripts/build-magick.sh
mkdir build && cd build
PKG_CONFIG_PATH=`pwd`/../ImageMagick/install/lib/pkgconfig cmake -DCMAKE_BUILD_TYPE=Release -DMAGICK_PKG_CONFIG=Magick++-7.Q16HDRI ..
make -j4
```

A more generic example:

```
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr -DMAGICK_PKG_CONFIG=Magick++-7.Q16HDRI <cyan_source_directory>
make -jX
sudo make install
```


