# Installation

### [Debian](https://www.debian.org) and [Ubuntu](https://www.ubuntu.com)-based distributions:

```bash
# Install required tools
sudo apt update
sudo apt install curl clang cmake

# Download latest release
export SG_VER="2.4.7" # Change to latest version
curl -SL https://github.com/risoflora/libsagui/archive/v$SG_VER.tar.gz | tar -zx
cd libsagui-$SG_VER/ && mkdir build && cd build/

# Configure, build and install
cmake -DCMAKE_C_COMPILER=clang ..
make && sudo make sagui install/strip
sudo ldconfig # Rebuild library cache
```

to uninstall:

```bash
sudo make uninstall
```

### [RHEL](https://redhat.com/en/technologies/linux-platforms/enterprise-linux), [Fedora](https://getfedora.org) and [CentOS](https://www.centos.org/)-based distributions:

```bash
# Install required tools
sudo dnf upgrade
sudo dnf install curl clang cmake

# Download latest release
export SG_VER="2.4.7" # Change to latest version
curl -SL https://github.com/risoflora/libsagui/archive/v$SG_VER.tar.gz | tar -zx
cd libsagui-$SG_VER/ && mkdir build && cd build/

# Configure, build and install
cmake -DCMAKE_C_COMPILER=clang ..
make && sudo make sagui install/strip
sudo ldconfig # Rebuild library cache
```

to uninstall:

```bash
sudo make uninstall
```

### [openSUSE](https://opensuse.org/) and [SLE](https://www.suse.com/products/server)-based distributions:

```bash
# Install required tools
sudo zypper refresh
sudo zypper install curl clang cmake

# Download latest release
export SG_VER="2.4.7" # Change to latest version
curl -SL https://github.com/risoflora/libsagui/archive/v$SG_VER.tar.gz | tar -zx
cd libsagui-$SG_VER/ && mkdir build && cd build/

# Configure, build and install
cmake -DCMAKE_C_COMPILER=clang ..
make && sudo make sagui install/strip
sudo ldconfig # Rebuild library cache
```

to uninstall:

```bash
sudo make uninstall
```

### [MinGW-w64](http://www.msys2.org) `i686` and `x86_64` on [Windows](http://microsoft.com/windows) or any supported OS:

```bash
# Install required tools
pacman -Syu
pacman -S --needed curl base-devel mingw-w64-i686-toolchain mingw-w64-x86_64-toolchain mingw-w64-i686-cmake mingw-w64-x86_64-cmake

# Download latest release
export SG_VER="2.4.7" # Change to latest version
curl -SL https://github.com/risoflora/libsagui/archive/v$SG_VER.tar.gz | tar -zx
cd libsagui-$SG_VER/ && mkdir build && cd build/

# Configure, build and install
cmake -G "MSYS Makefiles" -DCMAKE_INSTALL_PREFIX=/mingw32 ..
make sagui install/strip

# NOTE: use prefix "/mingw64" for x86_64
```

to uninstall:

```bash
make uninstall
```

# Missing steps for your system?

If you want the steps to install Sagui on your system and they are not available yet, feel free to contribute to this project by adding and publishing them on this page.
