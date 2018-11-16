# Cyan

Cyan is an open source cross-platform image viewer and converter, designed for [prepress](https://en.wikipedia.org/wiki/Prepress) (print) work. Like converting an image from [RGB](https://en.wikipedia.org/wiki/RGB_color_model) to [CMYK](https://en.wikipedia.org/wiki/CMYK_color_model), or the other way around. Cyan supports [color profiles](https://en.wikipedia.org/wiki/ICC_profile) complying with the [International Color Consortium](http://www.color.org/index.xalter) (ICC) standard, and strives to create as color-accurate images as possible, with support for [RGB](https://en.wikipedia.org/wiki/RGB_color_model), [CMYK](https://en.wikipedia.org/wiki/CMYK_color_model) and [GRAY](https://en.wikipedia.org/wiki/Grayscale) with up to [32-bit](https://en.wikipedia.org/wiki/32-bit) image depth.

![Promo](https://github.com/rodlie/cyan/raw/master/docs/images/cyan-promo-01.png)

# Features

* Supports RGB/CMYK/GRAY 8/16/32 bit images
* Supports ICC/ICM v2/v4 color profiles
* Supports PNG, JPEG, TIFF and PSD image input
* Supports TIFF, JPEG and PSD image output
* Extract embedded ICC color profiles
* Add ICC color profile to image
* Edit ICC color profile tags
* Convert to and from RGB/CMYK/GRAY
* Change image bit depth
* GIMP integration (import/export)

# Requirements

 * Microsoft Windows XP (+)
 * Apple Mac OS X Lion 10.7 (+)
 * Linux/FreeBSD (source-only)
 
 ## Recommended
 
 * A bitmap editor for more advanced usage
   * [GIMP](https://www.gimp.org)
   * [Krita](https://krita.org)
 * A color calibrated monitor (with available profile)
   * [DisplayCAL](https://displaycal.net/)
 * Additional ICC color profiles
   * [colormanagement.org](http://www.colormanagement.org)
   * [Adobe ICC](https://www.adobe.com/support/downloads/iccprofiles/icc_eula_win_end.html)
  
# News
![Promo](https://github.com/rodlie/cyan/raw/master/docs/images/cyan-promo-02.png)

News and updates related to Cyan.

 * [Status report 2018](https://sourceforge.net/p/prepress/blog/2018/11/cyan-2018-status-report/)
 
# Usage
![Promo](https://github.com/rodlie/cyan/raw/master/docs/images/cyan-promo-04.png)

...

# Integration
![Promo](https://github.com/rodlie/cyan/raw/master/docs/images/cyan-promo-06.png)

...

# Open Source
![Promo](https://github.com/rodlie/cyan/raw/master/docs/images/cyan-promo-05.png)

Cyan is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2 as published by the Free Software Foundation.

**Cyan is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.**  *See the GNU General Public License for more details.*

Cyan is copyright (c)2016-2018 Ole-André Rodlie. All rights reserved.

The source repository is hosted on [Github](https://github.com/rodlie/cyan) and the binaries (and third-party sources needed to create the binaries) are hosted on [Sourceforge](https://sourceforge.net/projects/prepress/files/).

## Build

Cyan requires the following third-party software installed before build:

 * [ImageMagick](http://imagemagick.org/script/index.php) 6.9.8-10
   * [LCMS](http://www.littlecms.com/) 2.x
   * [TIFF](http://www.simplesystems.org/libtiff/) 4.0.x
   * [JPEG](https://www.ijg.org/) 9
   * [PNG](http://www.libpng.org/pub/png/libpng.html) 1.6.x
   * With quantum depth 16+
   * With HDRI
   * With OpenMP
 * [LCMS](http://www.littlecms.com/) 2.x
 * [Qt(base)](https://www.qt.io/) 5.x
 
 Doing a regular user build:
 ```
 mkdir build && cd build
 qmake CONFIG+=release ..
 make
 ```
 
 Packaging example:
 ```
 qmake CONFIG+=release PREFIX=/usr ..
 make
 make INSTALL_ROOT=/path/to/pkg/tmp install
 ```
 
 ***Do not use Cyan if any of the unit tests fails. A failed unit test means that the image output from Cyan will not be as expected. The primary reason for a failed unit test is a unsupported version of ImageMagick.***
 
[![GPL2](https://img.shields.io/github/license/rodlie/cyan.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html) ![GitHub top language](https://img.shields.io/github/languages/top/rodlie/cyan.svg) ![GitHub language count](https://img.shields.io/github/languages/count/rodlie/cyan.svg) ![SourceForge](https://img.shields.io/sourceforge/dm/prepress.svg) ![Travis (.org)](https://img.shields.io/travis/rodlie/cyan.svg) ![GitHub last commit](https://img.shields.io/github/last-commit/rodlie/cyan.svg) ![GitHub issues](https://img.shields.io/github/issues-raw/rodlie/cyan.svg) ![GitHub closed issues](https://img.shields.io/github/issues-closed/rodlie/cyan.svg) [![GitHub release](https://img.shields.io/github/release/rodlie/cyan.svg)](https://github.com/rodlie/cyan/releases) [![Github commits (since latest release)](https://img.shields.io/github/commits-since/rodlie/cyan/latest.svg)](https://github.com/rodlie/cyan)
