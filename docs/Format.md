## Cyan Image Format

![Promo](https://github.com/rodlie/cyan/raw/master/docs/images/cyan-promo-05.png)

**This is a draft**

Cyan uses the [Magick Image File Format](https://imagemagick.org/script/miff.php) (MIFF) for it's own internal image format. It's a simple and open format that can be inspected and modified outside of Cyan *(ImageMagick needed)*.

### How does it work?

Let's open up Cyan and create a new image (a empty layer is automatically added). Now save the image as ``cyan-image.miff``.

Let's use ImageMagick to inspect the file:

```
$ magick identify cyan-image.miff

cyan-image.miff[0] MIFF 1024x1024 1024x1024+0+0 8-bit TrueColor sRGB 0.040u 0:00.030
cyan-image.miff[1] MIFF 1024x1024 1024x1024+0+0 8-bit TrueColor sRGB 8.02099MiB 0.020u 0:00.019
```

As you can see the project contains two images. But in Cyan our project ony has one layer (image). That's because the first image (0) is our project canvas. This image defines the project name, dimensions, bit depth, color space, ICC profile etc. The actual image data from the project canvas should be empty and always be ignored.

To get more information from the project canvas we can use ``-verbose`` when inspecting:

```
$ magick identify -verbose cyan-image.miff[0]
```

This will output a lot of information. The only difference between a vanilla MIFF and a MIFF created in Cyan is additional properties.

```
$ magick identify -verbose cyan-image.miff[0]
...
  Properties:
    cyan-layer-label: New Image
    cyan-project: 1
...
```

* ``cyan-project:`` This identifies the MIFF as a Cyan project (and version).
* ``cyan-layer-label:`` Optional Cyan layer label.

The ICC profile embedded in the project canvas is the main profile used on all images in the project. This means that all images in the project must have the same color space.

Now let's inspect our first available "layer" (the images are **not** in order, see ``cyan-layer-order``):

```
$ magick identify -verbose cyan-image.miff[1]
```

This will output even more information than before. The only difference between a vanilla MIFF and a MIFF created in Cyan is additional properties.

```
$ magick identify -verbose cyan-image.miff[1]
...
  Properties:
    cyan-compose: 0
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

* ``cyan-layer-compose:`` Compose method for this layer
* ``cyan-layer:`` This identifies the MIFF as a Cyan layer
* ``cyan-layer-label:`` Layer label
* ``cyan-layer-lock:`` Is layer locked?
* ``cyan-layer-order:`` Layer order
* ``cyan-layer-x:`` Layer X position
* ``cyan-layer-y:`` Layer Y position
* ``cyan-opacity:`` Layer opacity
* ``cyan-visibility:`` Is layer visible?

