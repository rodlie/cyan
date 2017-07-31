![cyan](https://github.com/olear/cyan/raw/master/res/cyan-header.png)
# Cyan
Free cross-platform prepress toolkit.

# Features

* Supports RGB/CMYK/GRAY 8/16/32 bit images
* Supports ICC/ICM v2/v4 color profiles
* Supports PNG/JPEG/TIFF/PSD input
* Supports TIFF and PSD output
* Manage embedded ICC color profiles
* Edit ICC color profiles
* Convert to and from RGB/CMYK/GRAY
* Change image bit depth
* GIMP integration (import/export)

# Requirements

System requirements for downloads:

 * Windows 7+
 * Mac OS X 10.9+
 * Linux (GCC 4.8.5+/Glibc 2.17+)
   * Ubuntu requires libxcb-xinerama0 (sudo apt-get install libxcb-xinerama0)

# Build

Build requirements:

* ImageMagick (MagickCore/Magick++) Q32(HDRI)/PNG/JPEG/TIFF/LCMS2
* LCMS 2.x
* Qt 4/5 (Core/GUI with PNG/TIFF support)

```
qmake CONFIG+=release PREFIX=XXX (/usr/local is default install path)
make
make install (optional, use INSTALL_ROOT=XXX for packaging)
```

# Issues

 * Mac OS X binaries does not support multi-threading

# Roadmap

 * Batch converter

# License
Cyan is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2 as published by the Free Software Foundation.

