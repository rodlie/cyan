# Cyan

Free cross-platform prepress image viewer and converter.

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
* Gamma loader (Linux)

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
