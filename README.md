![cyan](https://github.com/olear/cyan/raw/master/res/cyan-header.png)
# Cyan
Prepress viewer and converter.

# Features

* Supports RGB/CMYK/GRAY 8/16/32 bit images
* Supports ICC v2/v4 color profiles
* Supports PNG/JPEG/TIFF images
* Add/Change color profile on images
* Convert to/from RGB/CMYK/GRAY
* Extract embedded color profile from images

# Requirements

* RGB/CMYK/GRAY ICC color profiles
* Color calibrated monitor with profile recommended

# Build

Build requirements:
* ImageMagick (Q32 HDRI with PNG/JPEG/TIFF/LCMS)
* LCMS 2+
* Qt 4.8+

```
qmake PREFIX=XXX (/usr/local is default install path)
make
make install (optional, use INSTALL_ROOT for packaging)
```

# License
Cyan is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2 as published by the Free Software Foundation.

