# fsml

`fsml` a fork of `fstl` modified to read SML files.

`fstl` was originally written by [Matt Keeter](https://mattkeeter.com),
and is now primarily maintained by [@DeveloperPaul123](https://github.com/DeveloperPaul123).
The modifications in this fork are by Maeyanie.

fsml is designed to quickly load and render very high-polygon models;
showing 2 million triangles at 60+ FPS on a mid-range laptop.

## Setting `fsml` as the Default SML Viewer

### Windows

1. Right-click an SML file
2. Select `Open With` >>> `Choose another app`
3. Select `More Apps` and `Look for another app on this PC`
4. Enter the path to the `fsml` EXE file

### MacOS

1. Ctrl+click an SML file
2. Select `Get Info`
3. Navigate to the `Open with` section
4. Select `fsml` in the dropdown
5. Click `Change All`

### Linux

If `mimeopen` is available on your system, it can be used to set `fsml` as the default viewer for SML files.
Run the following in your terminal:

```bash
# replace example.sml with an actual file
mimeopen -d example.sml
```

The following output will result:

```
Please choose a default application for files of type model/sml

	1) Other...

use application #
```

Select the `Other` option and type `fsml` as the desired command to open SML files.
This will now become the system default, even when opening files from the file manager.

## Building

The only dependency for `fsml` is [Qt 5](https://www.qt.io),
plus [`cmake`](https://cmake.org/) for building.

### macOS

Install Qt from their website or [Homebrew](brew.sh).

Install `cmake` through Homebrew or equivalent.

Then, run through the following set of commands in a shell:

```
git clone https://github.com/Maeyanie/fsml
cd fsml
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=/usr/local/Cellar/qt/5.15.0/ ..
make -j8
./fsml.app/Contents/MacOS/fsml
```

You may need to edit the Qt path depending on your installation.

To package a standalone app, go to the app directory and run `package.sh`

```
cd ../app
./package.sh
```

This should produce two new files in the root directory:
- `fsml.app` is a standalone application that can be copied to `/Applications`
- `fsml.dmg` is a disk image that can be given to a friend

### Linux

Install Qt with your distro's package manager (required libraries are Core, Gui,
Widgets and OpenGL, e.g. `qt5-default` and `libqt5opengl5-dev` on Debian).

You can build fsml with CMake:
```
git clone https://github.com/Maeyanie/fsml
cd fsml
mkdir build
cd build
cmake ..
make -j8
./fsml
```

--------------------------------------------------------------------------------

# License

Copyright (c) 2022 Maeyanie
Copyright (c) 2014-2017 Matthew Keeter

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
