Catimg
======

Renders images in the terminal.

![Sonic](https://cloud.githubusercontent.com/assets/664177/10122399/7ff610ba-6518-11e5-8c2c-994149a89d69.png)

`catimg` is a little program written in C with no dependencies that prints images in terminal.
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

Examples
--------

![pikachu gif](https://cloud.githubusercontent.com/assets/664177/10122393/6846151e-6518-11e5-9a21-9be0c0765267.gif)
![cats](https://cloud.githubusercontent.com/assets/664177/10122420/9c2764d6-6519-11e5-8c76-e1636afec60f.png)

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
