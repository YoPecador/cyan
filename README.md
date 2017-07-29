![cyan](https://github.com/olear/cyan/raw/master/res/cyan-header.png)
# Cyan
Prepress viewer and converter.

# Features

* Supports RGB/CMYK/GRAY 8/16/32 bit images
* Supports ICC/ICM v2/v4 color profiles
* Supports PNG/JPEG/TIFF/PSD input
* Supports TIFF and PSD output
* Manage embedded ICC color profiles
* Edit ICC color profiles
* Convert to and from RGB/CMYK/GRAY
* Change image bit depth
* GIMP 2.x integration (Prepress workflow)

# Requirements

System requirements for downloads:

 * Windows 7+
 * Mac OS X 10.9+
 * Linux (GCC 4.8+/Glibc 2.17+)
   * Ubuntu requires libxcb-xinerama0 (sudo apt-get install libxcb-xinerama0)

# Build

Build requirements:

* ImageMagick/GraphicsMagick (Magick++) Q32(HDRI)/PNG/JPEG/TIFF/LCMS2
  * Add CONFIG+=gmagick to build against GraphicsMagick (missing black point, not supported)
  * Add CONFIG+=magick7 to build against ImageMagick 7 (only for testing, not supported)
  * ImageMagick 6.9.0-0 is the only certified cross-platform version compatible with Cyan.
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
 * Plug-ins

# License
Cyan is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2 as published by the Free Software Foundation.

