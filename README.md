# Cyan Image Editor 2.0

[![GitHub license](https://img.shields.io/github/license/rodlie/cyan)](https://github.com/rodlie/cyan/blob/master/COPYING)
![SourceForge](https://img.shields.io/sourceforge/dm/prepress.svg) 
![GitHub Releases](https://img.shields.io/github/downloads/rodlie/cyan/latest/total.svg)
![Travis (.org)](https://img.shields.io/travis/rodlie/cyan)
![GitHub last commit](https://img.shields.io/github/last-commit/rodlie/cyan)
[![GitHub issues](https://img.shields.io/github/issues/rodlie/cyan)](https://github.com/rodlie/cyan/issues)
![GitHub closed issues](https://img.shields.io/github/issues-closed/rodlie/cyan.svg)
[![GitHub release](https://img.shields.io/github/release/rodlie/cyan.svg)](https://github.com/rodlie/cyan/releases)
[![Github commits (since latest release)](https://img.shields.io/github/commits-since/rodlie/cyan/latest.svg)](https://github.com/rodlie/cyan)
![GitHub top language](https://img.shields.io/github/languages/top/rodlie/cyan.svg) ![GitHub language count](https://img.shields.io/github/languages/count/rodlie/cyan.svg)
![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/rodlie/cyan)
![GitHub repo size](https://img.shields.io/github/repo-size/rodlie/cyan)

![screenshot](docs/images/screenshot.png)

Cyan was an prepress color converter (and [GIMP](https://gimp.org) plug-in) but now aims to be a general-purpose image editor based on [ImageMagick](https://imagemagick.org). It will still have prepress features, but with more options. Note that we started from scratch with this version, so development will be slow, there are a lot to do before this is a usable image editor in any form.

***Currently under early development, not usable yet! please use [v1.2](https://github.com/rodlie/cyan/tree/1.2).***

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

 * Qt5 *(Core/Gui/Widgets/Concurrent) 5.6+*
 * Little CMS 2
 * ImageMagick *(Magick++) 7.0.8-29+*
 * fontconfig *(Windows/macOS)*

```
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr -DMAGICK_PKG_CONFIG=Magick++-7.Q16HDRI <cyan_source_directory>
make -jX
sudo make install
```

``MAGICK_PKG_CONFIG``: Name of the pkg-config .pc file to use. The default is ``Magick++``. Check your system for the correct name. Note that ImageMagick version 6 is not supported, you must have at least version 7.0.8-29.
