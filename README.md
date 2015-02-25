Catimg
==================
by Eduardo San Martin Morote aka Posva

posva13@gmail.com

This is a very simple script that print any image to the `stdout` using colors.
There are two C versions:

* One with no dependencies
* One using `MagickWand` to convert colors (more accurate) and loading images (more formats)

If `MagickWand` is available it is used otherwise the no-deps version is built. You can
force to build the no-deps version by passing `-DUSE_MAGICK=FALSE` to `cmake`.

To build simply do

```
mkdir build && cd build
cmake ..
make install
```

Script Dependencies
-------------------
In order to use the script you need:

* ImageMagick 6.6.0-4 2012-08-17 Q16 (Not tested with earlier versions)
* shell with 256 colors support ( __change the__ `#!/bin/bash` __by yours__). In OS X you just have to update the bash version using [Homebrew](http://brew.sh/) (or any other method).

Examples
--------

![Mario](http://i.imgur.com/Nt38dja.png)
![Nyan Cat](http://i.imgur.com/sgy81sI.png)

Differences between Script and C version
----------------------------------------

Script version is **way more** accurate concerning colors but **considerably** slower.

Contributing
------------

I'm open to any contributions that helps `catimg`!
If you find bugs, please create an issue or do a pull request :smile:.


