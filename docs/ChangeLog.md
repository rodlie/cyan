# Changes

## 1.99.4

**Release date : TBA**

  * Adjustments layers (Non-destructive)
  
## 1.99.3

**Release date : 20191029 (TBA)**

Cyan 2.0 Alpha 3. This is a development release, not for end-users.

  * Improved Linux binaries (better compatibility)
  * Ui changes
  * Improved new image/layer dialog
  * Dropped OpenEXR in official binaries (has limited support in ImageMagick)
  * Option to add "Background" on new image or layer
    * Solid
    * Gradient
    * Radial
    * Plasma
    * Noise
  * Initial support for GIMP project format (XCF) *(read-only)*
    * This also includes multi-layered PSD and TIFF files
  * One unified load button/function
  * One unified save button/function
  * Proper "save as" function
  * Fix tabs styles bug
  * Fixed various bugs
  * Fix potential crash if no color profiles present
  * macOS support (Lion 10.7+)

## 1.99.2

**Release date : 20190929**

Cyan 2.0 Alpha 2. This is a development release, not for end-users.

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

## 1.2.2-rc1

**Release date : 20190915**

Fix minor issues on Linux/BSD.

* Fixed GIMP 2.10 Linux/BSD support
* Added support for ImageMagick v7

## 1.2.1

**Release date: 20181204** 

* Fixed GIMP 2.10 Windows support
* Fixed broken Cyan Python plug-in
* Support XCF (GIMP image format)
* Support reading layers from XCF, TIFF, PSD

## 1.2.0

**Release date: 20181203**

 * Major "behind-the-scene" changes
 * Improved performance
 * Save to JPEG
 * Enhanced image information
 * Third-party software updates
 * Fixed issues converting HDR images

## 1.0.0

**Release date: 20160814**

 * first public release
