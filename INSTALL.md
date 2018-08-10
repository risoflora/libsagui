# Installing

This page shows the official binaries and packages for each stable version of the Sagui library. They are available for the common systems and below are the steps to install them.  

# Windows (pacman)

The binaries for Windows can be installed in two ways. The first one is using the`pacman` as following:

**MinGW-w64 i686:**

```bash
pacman -S mingw-w64-i686-libsagui
```

**MinGW-w64 x86_64:**

```bash
pacman -S mingw-w64-x86_64-libsagui
```

or downloading the library package (`32`/`64` bits) at the [releases page](https://github.com/risoflora/libsagui/releases).

# Ubuntu / Raspbian (APT)

On systems based in Debian like Ubuntu and Raspbian, the library can be installed using the `apt` package manager:

**Configuring the PPA:**

```bash
sudo add-apt-repository ppa:silvioprog/libsagui
```

**Installing the library for x86 or x86_64:**

```bash
sudo apt install libsagui1
```

**Installing the development files and the library for x86 or x86_64 (optional):**

```bash
sudo apt install libsagui-dev
```

# From sources

To build and install:

```bash
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON ..
make
sudo make sagui install/strip
sudo ldconfig # cache update
```

to uninstall:

```bash
sudo make uninstall
```

# Missing binaries/package for your system?

If you want binaries and/or package installation for your system and they are not available yet, feel free to contribute to this project by adding and publishing them on this page.