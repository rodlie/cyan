# Cyan2 (WIP)

***v2 is currently under early development and is not usable! please use [v1.2](https://github.com/rodlie/cyan/tree/1.2). This is a spare-time project, so development will be slow!***

Cyan v2 is/will be an open source cross-platform prepress color converter, image viewer and editor based on [ImageMagick](https://imagemagick.org).

## Requirements

 * Qt5 *(Widgets/Concurrent) 5.6+*
 * Little CMS *2.x*
 * ImageMagick *(MagickCore/Magick++) 7.0.8-29+*
   * system (distros) versions of ImageMagick is not recommended.
   * Files formats supported by ImageMagick defines what Cyan can read/write.
   * HDRI and Q16 is highly recommended.
   * Quantum depth in ImageMagick defines bit depth support in Cyan. This can't be changed after build.
   * Text support requires pangocairo support in ImageMagick, some text features require custom patches.
