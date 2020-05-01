# Installation

## Debian and Ubuntu-based distributions

```bash
# Install required tools
sudo apt update
sudo apt install curl clang cmake

# Download latest release
export SG_VER="3.0.0" # Change to latest version
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

## RHEL, Fedora and CentOS-based distributions

```bash
# Install required tools
sudo dnf upgrade
sudo dnf install curl clang cmake

# Download latest release
export SG_VER="3.0.0" # Change to latest version
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

## openSUSE and SLE-based distributions

```bash
# Install required tools
sudo zypper refresh
sudo zypper install curl clang cmake

# Download latest release
export SG_VER="3.0.0" # Change to latest version
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

## MinGW-w64 `i686` and `x86_64` on Windows or any supported OS

```bash
# Install required tools
pacman -Syu
pacman -S --needed curl base-devel \
  mingw-w64-i686-toolchain mingw-w64-x86_64-toolchain \
  mingw-w64-i686-cmake mingw-w64-x86_64-cmake

# Download latest release
export SG_VER="3.0.0" # Change to latest version
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

## Request steps for your system

If you want the steps to install Sagui on your system and they are not available
yet, feel free to contribute to this project by adding and publishing them on
this page.
