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

* RGB/CMYK/GRAY ICC color profiles installed
* Color calibrated monitor (with profile) recommended

# Usage

On first run make sure to select the default profiles for images without an embedded color profile, this is important or else color convertion will not work properly on images without an embedded color profile. 

Then open your image, either push the 'Load' button, or select 'Open image' from the 'File' menu or use the CTRL+O shortcut. 

If you just want to change the image profile, select a new color profile from the 'Input' list, then save the image using the 'Save' button, or select 'Save image' from the 'File' menu or use the CTRL+S shortcut. 

If you want to change the colorspace (RGB to CMYK), use the 'Output' list, this will convert the image from the existing colorspace to the new colorspace defined in the selected color profile in the 'Output' list. Then save the image. 

If you want to "proof" the convertion make sure to select a profile in the 'Monitor' list and tick the checkbox on the right (can be toggled on/off using right mouse button in the viewer). 

Images are viewed at 100% in the viewer, you can zoom in/out using the mouse wheel, third mouse button will reset zoom to 100%.

# Build

Build requirements:
* ImageMagick (Q32 HDRI with PNG/JPEG/TIFF/LCMS)
* LCMS 2+
* Qt 4+ (with PNG and TIFF support)

```
qmake PREFIX=XXX (/usr/local is default install path)
make
make install (optional, use INSTALL_ROOT for packaging)
```

# License
Cyan is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2 as published by the Free Software Foundation.

