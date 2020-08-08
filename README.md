# Cyan2 (experimental)

[![GitHub license](https://img.shields.io/github/license/rodlie/cyan)](https://github.com/rodlie/cyan/blob/master/COPYING)
![Travis (.org)](https://img.shields.io/travis/rodlie/cyan)
![GitHub last commit](https://img.shields.io/github/last-commit/rodlie/cyan)

![screenshot](docs/images/screenshot.png)

Cyan was an prepress color converter (and [GIMP](https://gimp.org) plug-in) but now aims to be a simple general-purpose image editor and converter for RGB/CMYK/GRAY based on [ImageMagick](https://imagemagick.org).

## Keyboard/Mouse shortcuts

| Shortcut           | Action                             |
| ------------------ | ---------------------------------- |
| Ctrl + N           | New                                |
| Ctrl + O           | Open                               |
| Ctrl + Q           | Quit                               |
| Ctrl + F4/W        | Close                              |
| Ctrl + 1           | Zoom 100%                          |
| Ctrl + 2           | Zoom to fit                        |
| Ctrl + Tab         | Switch image                       |
| Ctrl + G           | Add vertical guide                 |
| Ctrl + Shift + G   | Add horizontal guide               |
| Arrows (+Shift)    | Move selected layer                |
| Mouse wheel        | Zoom                               |
| Third mouse        | Switch interactive tool            |
| Left mouse         | Drag (if move tool selected)       |
| Left mouse + Wheel | Brush size (if draw tool selected) |

## License

Copyright (c) Ole-André Rodlie ``<ole.andre.rodlie@gmail.com>``. All rights reserved.

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

* Cyan include icons from [FatCow](https://www.fatcow.com) Hosting that are licensed under [CC Attribution 3.0 US](http://creativecommons.org/licenses/by/3.0/us/)
* Cyan Logo by Ole-André Rodlie is licensed under [CC Attribution-ShareAlike 4.0 International](http://creativecommons.org/licenses/by-sa/4.0/)
* Cyan include some components from Nokia Qt Solutions licensed under LGPL-v2.1+

Source code for third-party software included in the official builds are available on [SourceForge](https://sourceforge.net/projects/prepress/files/sdk/source/).

The following ICC color profiles are included with Cyan:

### sRGB_v4_ICC_preference.icc / sRGB2014.icc

This profile is made available by the International Color Consortium, and may be copied, distributed, embedded, made, used, and sold without restriction. Altered versions of this profile shall have the original identification and copyright information removed and shall not be misrepresented as the original profile.

### sRGB_IEC61966-2-1_no_black_scaling.icc

To anyone who acknowledges that the file "sRGB_IEC61966-2-1_no_black_scaling.icc" is provided "AS IS" WITH NO EXPRESS OR IMPLIED WARRANTY, permission to use, copy and distribute these file for any purpose is hereby granted without fee, provided that the file is not changed including the ICC copyright notice tag, and that the name of ICC shall not be used in advertising or publicity pertaining to distribution of the software without specific, written prior permission. ICC makes no representations about the suitability of this software for any purpose.

### SWOP2006_Coated5v2.icc

This profile is made available by IDEAlliance, with permission of X-Rite, Inc., and may be used, embedded, exchanged, and shared without restriction. It may not be altered, or sold without written permission of IDEAlliance.

### ISOcoated_v2_300_bas.icc / ISOcoated_v2_grey1c_bas.icc

Copyright (c) 2007-2010, basICColor GmbH

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must  
not
  claim that you wrote the original software. If you use this software
  in a product, an acknowledgment in the product documentation would be
  appreciated but is not required.

  2. Altered source versions must be plainly marked as such, and must  
not be
  misrepresented as being the original software.

  3. This notice may not be removed or altered from any source
  distribution.


## Build

Binary builds are recommended, but you can always build from source. The following instructions are for Linux.

### Requirements

 * cmake *3+*
 * pkg-config
 * Qt5 *(Widgets/Concurrent) 5.5+*
 * Little CMS *2+*
 * ImageMagick *(Magick++) 7.0.8-34+*
   * Minimum quantum depth of 16 is recommended
   * Zero configuration is recommended *(Windows/macOS)*
   * HDRI required
   * lcms2 required
   * zlib/bzip2/lzma required
   * png/tiff/jpeg support as a minimum
 * fontconfig *(Windows/macOS)*
 * ffmpeg *3+ (optional)*

### RHEL/CentOS/Fedora

On RHEL/CentOS/Fedora you will need to install the following packages:

```
sudo yum install git gcc-c++ autoconf libpng-devel libjpeg-turbo-devel lcms2-devel libwebp-devel pango-devel cairo-devel libtiff-devel xz-devel zlib-devel qt5-qtbase-devel cmake3
```

***Remember to replace ``cmake`` with ``cmake3`` on RHEL/CentOS when running the build command further down.***

### Ubuntu Xenial/Bionic

On Ubuntu you will need to install the following packages:

```
sudo apt-get install git build-essential cmake pkg-config qtbase5-dev libcairo2-dev libpango1.0-dev libwebp-dev liblcms2-dev libjpeg-dev libpng-dev libtiff-dev liblzma-dev zlib1g-dev libopenjp2-7-dev
```

* **Qt5 shipped with Ubuntu Bionic and beyond is not recommended!**

### Other distros

If your system has an up-to-date installation of ImageMagick v7 then you only need ``qtbase`` and  ``lcms2``.

Note that Cyan may at any time add additional (or expect certain) changes to ImageMagick, so it's recommended to build against the ImageMagick version available in our source repository.

### Build Cyan

Now you can build Cyan, this command will also build and link a static version of ImageMagick.
```
git clone https://github.com/rodlie/cyan
cd cyan
git submodule update --init
MKJOBS=4 STATIC=1 sh share/scripts/build-magick.sh
mkdir build && cd build
PKG_CONFIG_PATH=`pwd`/../ImageMagick/install/lib/pkgconfig cmake -DCMAKE_BUILD_TYPE=Release -DMAGICK_PKG_CONFIG=Magick++-7.Q16HDRI ..
make -j4
```

A more generic example:

```
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr <cyan_source_directory>
make -jX
sudo make install
```
