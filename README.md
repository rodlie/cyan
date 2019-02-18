# Cyan2

***v2 is currently under development, please use v1.2 https://github.com/rodlie/cyan/tree/1.2 for color convertion and/or Gimp plug-in***

Cyan2 is an open source cross-platform image editor. Originally designed for prepress color convertion, but now aims to be a general-purpose image editor. v2 started from scratch so it might take a while to get to the level of features you might expect from an image editor.

Note that this is currently a spare-time project, so development will be slow.

## Requirements

 * QtWidgets 5.6+
 * QtConcurrent 5.6+
 * Little CMS 2.x
 * ImageMagick 7.0.8-20+

## Build

Supported build options:
 * ``CONFIG += with_ffmpeg`` - enable *experimental* ffmpeg support
 * ``PREFIX = </usr/local>`` - install prefix
 * ``DOCDIR = <PREFIX/share/doc>`` - documentation install path
 * ``MANDIR = <PREFIX/share/man>`` - manual install path
 * ``BINDIR = <PREFIX/bin>`` - executable install path
 * ``ICONDIR = <PREFIX/share/icons>`` - icon themes path
 * ``ICCDIR = <PREFIX/share/color/icc>`` - ICC color profile path
 * ``APPDIR = <PREFIX/share/applications>`` - .desktop (applications) path
 * ``MAGICK = <Magick++-7.Q16HDRI>`` - ImageMagick pkg-config .pc file

Normal build:
```
mkdir build && cd build
qmake ..
make
```

Install:

```
sudo make install
```

Or run from the build folder:
```
make INSTALL_ROOT=`pwd` install
./usr/local/bin/Cyan
```

Package build:
```
qmake PREFIX=/usr
make
make INSTALL_ROOT=<pkg_path> install
```
