# Installing

Download and install Sagui on [Ubuntu](https://www.ubuntu.com), [Raspbian](https://www.raspberrypi.org/downloads/raspbian), etc.:

```bash
# Install required tools.
sudo apt update
sudo apt install clang cmake

# Download latest release.
wget --continue --content-disposition https://github.com/risoflora/libsagui/archive/v1.0.0.tar.gz
tar -zxvf libsagui-1.0.0.tar.gz
cd libsagui-1.0.0/ && mkdir build && cd build/

# Configure, build and install.
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON ..
make && sudo make sagui install/strip
sudo ldconfig # cache update
```

to uninstall:

```bash
sudo make uninstall
```

Download and install on Windows ([MinGW-w64](http://www.msys2.org)):

```bash
# Install required tools.
pacman -Syu
pacman -S cmake wget

# Download latest release.
wget --continue --content-disposition https://github.com/risoflora/libsagui/archive/v1.0.0.tar.gz
tar -zxvf libsagui-1.0.0.tar.gz
cd libsagui-1.0.0/ && mkdir build && cd build/

# Configure, build and install.
cmake -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/mingw32 -DBUILD_SHARED_LIBS=ON ..
make sagui install/strip

# NOTE: use prefix "/mingw64" for x86_64. 
```

to uninstall:

```bash
make uninstall
```

# Missing steps for your system?

If you want the steps to install Sagui on your system and they are not available yet, feel free to contribute to this project by adding and publishing them on this page.