# Catimg

Renders images in the terminal.

![mewtwo](https://cloud.githubusercontent.com/assets/664177/10267523/f81296de-6a97-11e5-99d0-d2124bd6a9e3.png)

`catimg` is a little program written in C with no dependencies that prints images in terminal.
It supports JPEG, PNG and GIF formats. This program was originally a script that
did the same by using ImageMagick `convert`.

## Installation

Homebrew

```sh
brew install catimg
```

Arch Linux

```sh
yaourt -S catimg
```

Fedora 25 and later

```sh
dnf -y install catimg
```

## Building

```sh
cmake .
make install
```

## Script Dependencies

In order to use the script you need:

- ImageMagick 6.6.0-4 2012-08-17 Q16 (Not tested with earlier versions)
- shell with 256 colors support ( **change the** `#!/usr/bin/env bash` **by
  yours**). In OS X you just have to update the bash version using
  [Homebrew](http://brew.sh/) (or any other method).

## Examples

![pikachu gif](https://cloud.githubusercontent.com/assets/664177/10122393/6846151e-6518-11e5-9a21-9be0c0765267.gif)

## High Resolution vs Low Resolution

By using unicode characters catimg will try to display pictures in higher resolution. You can force the resolution with the `-r` option.

![cats](https://cloud.githubusercontent.com/assets/664177/10267481/da74d106-6a96-11e5-937d-efd452e53170.png)
![catslow](https://cloud.githubusercontent.com/assets/664177/10267494/07464a48-6a97-11e5-813e-0645f56b4c4f.png)

## Differences between Script and C version

Script version is **way more** accurate concerning colors but **considerably**
slower.

## Contributing

I'm open to any contributions that helps `catimg`! If you find bugs, please
create an issue or do a pull request :smile:.

## License

[MIT](http://opensource.org/licenses/MIT)

Copyright (c) Eduardo San Martin Morote
