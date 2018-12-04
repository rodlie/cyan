# Cyan

Cyan is an open source cross-platform image viewer and converter, designed for [prepress](https://en.wikipedia.org/wiki/Prepress) (print) work. Like converting an image from [RGB](https://en.wikipedia.org/wiki/RGB_color_model) to [CMYK](https://en.wikipedia.org/wiki/CMYK_color_model), or the other way around. Cyan supports [color profiles](https://en.wikipedia.org/wiki/ICC_profile) complying with the [International Color Consortium](http://www.color.org/index.xalter) (ICC) standard, and strives to create as color-accurate images as possible, with support for [RGB](https://en.wikipedia.org/wiki/RGB_color_model), [CMYK](https://en.wikipedia.org/wiki/CMYK_color_model) and [GRAY](https://en.wikipedia.org/wiki/Grayscale) with up to [32-bit](https://en.wikipedia.org/wiki/32-bit) image depth.

![Promo](https://github.com/rodlie/cyan/raw/master/docs/images/cyan-promo-01.png)

# Features

* Supports RGB/CMYK/GRAY 8/16/32 bit images
* Supports ICC/ICM v2/v4 color profiles
* Supports PNG, JPEG, TIFF, XCF and PSD image input
* Supports TIFF, JPEG and PSD image output
* Supports TIFF, XCF and PSD image layers (v1.3+)
* Extract embedded ICC color profiles from images
* Add ICC color profile to image
* Edit ICC color profile tags
* Convert to and from RGB/CMYK/GRAY
* Change image bit depth
* GIMP integration (import/export)

# Requirements

 * [![Download for Windows](https://github.com/rodlie/cyan/raw/master/docs/images/download_for_windows.png)](https://github.com/rodlie/cyan/releases/latest) Microsoft Windows 7/8/10 x64
 * [![Download for Mac](https://github.com/rodlie/cyan/raw/master/docs/images/download_for_mac.png)](https://github.com/rodlie/cyan/releases/latest) Apple Mac OS X 10.10-10.14
 * [![Download for Linux](https://github.com/rodlie/cyan/raw/master/docs/images/download_for_linux.png)](https://github.com/rodlie/cyan/releases/latest) GNU/Linux x86_64 (glibc 2.23+/libgcc 5.4+)

## Recommended

 * A bitmap editor
   * [GIMP](https://www.gimp.org)
   * [Krita](https://krita.org)
   * [AzPainter](https://github.com/symbian9/azpainter)
 * A vector editor
   * [sK1](https://sk1project.net/)
   * [Inkscape](https://inkscape.org/)
 * A DTP editor
   * [Scribus](https://www.scribus.net/)
 * A color calibrated monitor (with available profile)
   * [DisplayCAL](https://displaycal.net/)
 * ICC color profiles
   * [colormanagement.org](http://www.colormanagement.org)
   * [Adobe ICC](https://www.adobe.com/support/downloads/iccprofiles/icc_eula_win_end.html)

# News
![Promo](https://github.com/rodlie/cyan/raw/master/docs/images/cyan-promo-02.png)

News and updates related to Cyan.

 * [Cyan 1.2.0 released](https://github.com/rodlie/cyan/releases/tag/1.2.0)
 * [The (long) road to Cyan 2](https://github.com/rodlie/cyan/issues/12)

# Usage
![Promo](https://github.com/rodlie/cyan/raw/master/docs/images/cyan-promo-04.png)

Cyan should be relatively easy to use and understand. The first thing you should know is that Cyan converts images using color profiles. Converting an image to another colorspace requires two profiles, one with the same colorspace as the image and a second profile with the desired output colorspace.

![Promo](https://github.com/rodlie/cyan/raw/master/docs/images/cyan-screenshot-01.png)

## Setup

Cyan requires some minimal setup before usage. You will need to select default color profiles for RGB, CMYK and GRAY. These are used when you load an image without an embedded color profile.

![Promo](https://github.com/rodlie/cyan/raw/master/docs/images/cyan-usage-02.png)

You can set a color profile for your monitor if wanted, this will enable a RGB preview/proof of the final image. Note that this will add a performance penalty and you will need a color calibrated monitor with the color profile (available in Cyan).

![Promo](https://github.com/rodlie/cyan/raw/master/docs/images/cyan-usage-05.png)

For final adjustments you have rendering intent and black point compensation.

![Promo](https://github.com/rodlie/cyan/raw/master/docs/images/cyan-usage-06.png)

## Convert

Load image and verify the input profile (the embedded is recommended).

![Promo](https://github.com/rodlie/cyan/raw/master/docs/images/cyan-usage-07.png)

Now selected the desired output profile (in this example we want a CMYK image for print).

![Promo](https://github.com/rodlie/cyan/raw/master/docs/images/cyan-usage-08.png)

When you are satisfied with the result just hit save. you can output as JPEG, TIFF and PSD.

## Mouse/Keyboard

* **Ctrl+O** - Open image
* **Ctrl+S** - Save image
* **Ctrl+E** - Save embedded profile (from loaded image)
* **Ctrl+Q** - Quit
* **Right mouse button** - Fit-to-view
* **Left mouse button** - Drag
* **Middle mouse button** - View 100%
* **Mouse wheel** - Zoom in/out

# Integration
![Promo](https://github.com/rodlie/cyan/raw/master/docs/images/cyan-promo-06.png)

Cyan offers tight integration with [GIMP](https://www.gimp.org) and works as an import/export plug-in in [GIMP](https://www.gimp.org) 2.4+. Just select ``Cyan/Import Image`` or ``Cyan/Export Image``.

![Promo](https://github.com/rodlie/cyan/raw/master/docs/images/cyan-promo-07.png)

## Import CMYK from GIMP

Open GIMP and select ``Cyan/Import Image``, in Cyan open the desired image and convert to RGB, then hit ``save`` and your image is automatically imported into GIMP.

## Export to CMYK from GIMP

When you are finished editing in GIMP select ``Cyan/Export Image``, in Cyan convert to CMYK then hit ``save``, you can save to TIFF, JPEG and PSD.

## GIMP notes

When Cyan starts it will scan your home folder for GIMP 2.x plug-in folders, if found it will install a Cyan Python plug-in in the folder that does not have it. 

If you upgrade GIMP and ``Cyan/*`` is gone just launch Cyan manually once to update the plug-in against your new version of GIMP, this will only happen on GIMP feature releases (2.8=>2.10 etc).

# Open Source
![Promo](https://github.com/rodlie/cyan/raw/master/docs/images/cyan-promo-05.png)

This software is governed by the CeCILL license under French law and abiding by the rules of distribution of free software. You can use, modify and / or redistribute the software under the terms of the CeCILL license as circulated by CEA, CNRS and INRIA at the following URL https://www.cecill.info.
## Build

Cyan requires the following third-party software installed before build:

 * [CMake](https://cmake.org/) 3.1+
 * [Qt(Widgets/Concurrent)](https://www.qt.io/) 5.x
 * [LCMS](http://www.littlecms.com/) 2.x
 * [ImageMagick](http://imagemagick.org/script/index.php) 6.9.9-24+
   * [LCMS](http://www.littlecms.com/) 2.x
   * [TIFF](http://www.simplesystems.org/libtiff/) 4.0.x
   * [JPEG](https://www.ijg.org/) 9
   * [PNG](http://www.libpng.org/pub/png/libpng.html) 1.6.x
   * With quantum depth 16+
   * With HDRI
   * With OpenMP

Doing a regular build:
```
mkdir build && cd build
cmake ..
make && make test
```

 ***Do not use or distribute Cyan if any of the unit tests fails!***

![GitHub top language](https://img.shields.io/github/languages/top/rodlie/cyan.svg) ![GitHub language count](https://img.shields.io/github/languages/count/rodlie/cyan.svg) ![SourceForge](https://img.shields.io/sourceforge/dm/prepress.svg) 
![GitHub Releases](https://img.shields.io/github/downloads/rodlie/cyan/latest/total.svg) ![Travis (.org)](https://img.shields.io/travis/rodlie/cyan.svg) ![GitHub last commit](https://img.shields.io/github/last-commit/rodlie/cyan.svg) ![GitHub issues](https://img.shields.io/github/issues-raw/rodlie/cyan.svg) ![GitHub closed issues](https://img.shields.io/github/issues-closed/rodlie/cyan.svg) [![GitHub release](https://img.shields.io/github/release/rodlie/cyan.svg)](https://github.com/rodlie/cyan/releases) [![Github commits (since latest release)](https://img.shields.io/github/commits-since/rodlie/cyan/latest.svg)](https://github.com/rodlie/cyan)
