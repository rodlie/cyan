# Cyan Pixel Editor

[![GitHub license](https://img.shields.io/github/license/rodlie/cyan)](https://github.com/rodlie/cyan/blob/master/COPYING)
![Travis (.org)](https://img.shields.io/travis/rodlie/cyan)
![GitHub last commit](https://img.shields.io/github/last-commit/rodlie/cyan)

![screenshot](images/screenshot.png)

Cyan 2 aims to be a simple general-purpose image editor and converter for RGB/CMYK/GRAY based on [ImageMagick](https://imagemagick.org).

***Cyan 2 is EXPERIMENTAL, not for regular use!***

## Shortcuts

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
| Arrows (+Shift)    | Move selected layer X/Y            |
| PageUp             | Move selected layer stack up       |
| PageDown           | Move selected layer stack down     |
| Mouse wheel        | Zoom                               |
| Third mouse        | Switch interactive tool            |
| Left mouse         | Drag (if move tool selected)       |
| Left mouse + Wheel | Brush size (if draw tool selected) |

## License

Copyright (c) Ole-André Rodlie. All rights reserved.

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

* Cyan include icons from [FatCow](https://www.fatcow.com) Hosting that are licensed under [CC Attribution 3.0 US](http://creativecommons.org/licenses/by/3.0/us/)
* Cyan Logo by Ole-André Rodlie is licensed under [CC Attribution-ShareAlike 4.0 International](http://creativecommons.org/licenses/by-sa/4.0/)
* Cyan include some components from Nokia Qt Solutions licensed under LGPL-v2.1+

Source code for third-party software included in the official builds are available on [SourceForge](https://sourceforge.net/projects/prepress/files/sdk/source/).

The following ICC color profiles are included with Cyan:

**sRGB_v4_ICC_preference.icc / sRGB2014.icc**

This profile is made available by the International Color Consortium, and may be copied, distributed, embedded, made, used, and sold without restriction. Altered versions of this profile shall have the original identification and copyright information removed and shall not be misrepresented as the original profile.

**sRGB_IEC61966-2-1_no_black_scaling.icc**

To anyone who acknowledges that the file "sRGB_IEC61966-2-1_no_black_scaling.icc" is provided "AS IS" WITH NO EXPRESS OR IMPLIED WARRANTY, permission to use, copy and distribute these file for any purpose is hereby granted without fee, provided that the file is not changed including the ICC copyright notice tag, and that the name of ICC shall not be used in advertising or publicity pertaining to distribution of the software without specific, written prior permission. ICC makes no representations about the suitability of this software for any purpose.

**SWOP2006_Coated5v2.icc**

This profile is made available by IDEAlliance, with permission of X-Rite, Inc., and may be used, embedded, exchanged, and shared without restriction. It may not be altered, or sold without written permission of IDEAlliance.

**ISOcoated_v2_300_bas.icc / ISOcoated_v2_grey1c_bas.icc**

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

## Changelog

## 1.99.4

**Release date : TBA**

  * Change layer stacking with PageUp/PageDown keys
  * Bug fixes
  * Dropped FFmpeg support (focus on primary functions instead)

## 1.99.3

**Release date : 20191103**

  * Added support for system theme/style
  * Updated to Qt 5.13.2 on Linux and Windows
  * Improved Linux binaries (better compatibility)
  * Ui changes
  * Improved new image/layer dialog
  * Dropped OpenEXR in official binaries (has limited support in ImageMagick anyway)
  * Option to add background on new image or layer
    * Solid
    * Gradient
    * Radial
    * Plasma
    * Noise
  * Initial support for GIMP project format (XCF) *(read-only)*
    * This also includes multi-layered PSD and TIFF files
  * One unified load button/function
  * One unified save button/function
  * Proper save function
  * Fix tabs styles bug
  * Fixed various bugs
  * Fix potential crash if no color profiles present
  * macOS support (Lion 10.7+)
## 1.99.2

**Release date : 20190929**

### General

* UI clean up
* Bug fixes

### Windows

* Updated Qt to 5.13.1
* Updated FFmpeg to 4.2.1

### Linux

Cyan will from now on be distributed as an AppImage. The AppImage will be built against Glibc 2.23 and GCC 5.5. Cyan should be compatible with most distributions released around 2016 and beyond. Note that some installations may require additional XCB libraries installed prior to running Cyan *(Fedora 30 was missing libxkbcommon-x11 on the XFCE Live spin)*.

### macOS

Work on the new SDK for macOS has begun, binaries will probably be available on the next release. The binary will be compatible with OS X Lion (10.7).

## 1.99.1

**Release date : 20190919**

First step on the road to version 2.0. This is a development release, not for end-users.

* Initial support for video frame import
* Initial support for guides
* Initial support for (widget) plugins
* Initial support for zoom
* Confirm close/quit if we have unsaved image(s)
* Complete re-write from ground up, now an image editor (well, it's a start)
  * The re-write started around the release of version 1.2 and continued until the end of 2018, then the project stalled until August 2019
* Includes a bunch of bugs and missing features, enjoy :)
