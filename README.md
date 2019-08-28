# Cyan v2 (WIP)

***v2 is currently under early development and is not usable! please use [v1.2](https://github.com/rodlie/cyan/tree/1.2). This is a spare-time project so development will be slow! I will not respond to v2 issues until beta status***

Cyan v2 aims be a simple open source cross-platform general-purpose image editor based on [ImageMagick](https://imagemagick.org).

## Features

...

## FAQ

...

## Build

### Requirements

 * Qt5 *(Widgets/Concurrent)*
 * Little CMS 2
 * ImageMagick *(Magick++) 7.0.8-29+*

...

## Project format (draft)

Cyan uses the [Magick Image File Format](https://imagemagick.org/script/miff.php) (MIFF) for it's own internal image format. It's a simple and open format that can be inspected and modified outside of Cyan *(ImageMagick needed)*.

But the MIFF is just a collection of random images, how does it work in Cyan?

Let's open up Cyan and create a new image (a empty layer is automatically added). Now save the image as ``cyan-image.miff`` (file=>save project).

Now let's use ImageMagick to inspect the file:

```
$ magick identify cyan-image.miff

cyan-image.miff[0] MIFF 1024x1024 1024x1024+0+0 8-bit TrueColor sRGB 0.040u 0:00.030
cyan-image.miff[1] MIFF 1024x1024 1024x1024+0+0 8-bit TrueColor sRGB 8.02099MiB 0.020u 0:00.019
```

As you can see the project contains two images. But in Cyan our project ony has one layer!? That's because the first image (0) is our "canvas". This image defines the project name, dimensions, bit depth, color space, ICC profile etc. The actual image data from the "canvas" should always be ignored.

To get more information from the "canvas" we can use ``-verbose`` when inspecting:

```
$ magick identify -verbose cyan-image.miff[0]
```

This will output a lot of information. The only difference between a normal MIFF and a MIFF from Cyan is additional properties.

```
$ magick identify -verbose cyan-image.miff[0]
...
  Properties:
    cyan-layer-label: New Image
    cyan-project: 1
...
```

* ``cyan-project:`` This identifies the MIFF as a Cyan project.
* ``cyan-layer-label:`` Cyan layer label (if "canvas" then project label).

The ICC profile embedded in the "canvas" is the main profile used on all layers/images in the project. This means that all layers/images in the project must have the same colorspace as defined in "canvas". The bit depth of each layer/image can of course be different.

Now let's inspect our first available layer (the layers are **not** in order, see ``cyan-layer-order``):

```
$ magick identify -verbose cyan-image.miff[1]
```

This will output even more information than before. The only difference between a normal MIFF and a MIFF from Cyan is additional properties.

```
$ magick identify -verbose cyan-image.miff[1]
...
  Properties:
    cyan-compose: Normal
    cyan-layer: 1
    cyan-layer-label: New Image
    cyan-layer-lock: 0
    cyan-layer-order: 0
    cyan-layer-x: 0
    cyan-layer-y: 0
    cyan-opacity: 1
    cyan-visibility: 1
...
```

* ``cyan-compose:`` Compose method for this layer. Currently not documented, see [source](https://github.com/rodlie/cyan/blob/master/editor/common/cyan_common.cpp#L56) for more info
* ``cyan-layer:`` This identifies the MIFF as a Cyan layer
* ``cyan-layer-label:`` Layer label
* ``cyan-layer-lock:`` Is layer locked?
* ``cyan-layer-order:`` Layer order
* ``cyan-layer-x:`` Layer X position
* ``cyan-layer-y:`` Layer Y position
* ``cyan-opacity:`` Layer opacity
* ``cyan-visibility:`` Is layer visible?

That's all we need to compose the final image.

**This is a draft**

**Since Cyan is under development this also goes for the file format, things will change (and break) at any time until we reach RC status.**