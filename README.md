Catimg
======

Little program written in C with no dependencies that prints images in terminal.
It supports JPEG, PNG and GIF formats. This program was originally a script that
did the same by using ImageMagick `convert`.

Building
--------

```
cmake .
make install
```

Script Dependencies
-------------------
In order to use the script you need:

* ImageMagick 6.6.0-4 2012-08-17 Q16 (Not tested with earlier versions)
* shell with 256 colors support ( __change the__ `#!/usr/bin/env bash` __by
    yours__). In OS X you just have to update the bash version using
    [Homebrew](http://brew.sh/) (or any other method).

Examples --------

![Mario](http://i.imgur.com/Nt38dja.png) ![Nyan
Cat](http://i.imgur.com/sgy81sI.png)

Differences between Script and C version
----------------------------------------

Script version is **way more** accurate concerning colors but **considerably**
slower.

Contributing
------------

I'm open to any contributions that helps `catimg`!  If you find bugs, please
create an issue or do a pull request :smile:.

License
-------

[MIT](http://opensource.org/licenses/MIT)

Copyright (c) 2015 Eduardo San Martin Morote
