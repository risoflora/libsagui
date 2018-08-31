# Supported compilers

The Sagui library is compatible with the following compilers:

* `gcc` - from [GNU Compiler Collection](https://gcc.gnu.org).
* `llvm` - from [CLang Project](https://clang.llvm.org).

and was successfully tested in:

* MinGW-w64 - using latest `gcc` version [`i686`/`x86_64`]
* Google's NDK - using `clang` 6.0 from NDK-r17 [API `24` / ABIs `armeabi-v7a` and `aarch64`]
* GCC on Raspbian - version 6.3.0 [`i686` / `arm-linux-gnueabihf`]
* CLang on Raspbian - version 3.8.1 [`i686` / `armv6--linux-gnueabihf`]

# Build types

One build type can be specified though the variable `CMAKE_BUILD_TYPE=<Release|Debug>`. A minimal command to prepare the release type is:

```bash
mkdir build
cd build/
cmake -DCMAKE_BUILD_TYPE=Release ..
```

it prints a build summary as following:

```bash
-- The C compiler identification is GNU 7.3.0
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Looking for __GNU_LIBRARY__
-- Looking for __GNU_LIBRARY__ - found
-- Looking for include file errno.h
-- Looking for include file errno.h - found

Sagui library 1.0.0 - building summary:

  Generator: Unix Makefiles
  Install: /usr/local
  System: Linux-4.15.0-30-generic x86_64
  Compiler:
    Executable: /usr/bin/cc
    Version: 7.3.0
    Machine: x86_64-linux-gnu
    CFLAGS: -O3 -DNDEBUG -Wall -Werror -Wextra -Wpedantic -Wdeclaration-after-statement -Wstrict-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline
  Build: Release-x86_64 (static)
  HTTPS: No
  Examples: Yes (str, strmap, httpsrv, httpauth, httpuplds, httpcookie)
  Docs:
    HTML: No
    PDF: No
  Run tests: No
  cURL tests: No

-- Configuring done
-- Generating done
-- Build files have been written to: ~/libsagui/build
```

Finally, build the static library: 

```bash
make sagui
```

it prints a log as following:

```bash
Scanning dependencies of target libmicrohttpd-0.9.59
[  5%] Creating directories for 'libmicrohttpd-0.9.59'
[ 11%] Performing download step (download, verify and extract) for 'libmicrohttpd-0.9.59'
-- libmicrohttpd-0.9.59 download command succeeded.  See also ~/libsagui/build/libmicrohttpd-0.9.59/src/libmicrohttpd-0.9.59-stamp/libmicrohttpd-0.9.59-download-*.log
[ 16%] No patch step for 'libmicrohttpd-0.9.59'
[ 22%] No update step for 'libmicrohttpd-0.9.59'
[ 27%] Performing configure step for 'libmicrohttpd-0.9.59'
-- libmicrohttpd-0.9.59 configure command succeeded.  See also ~/libsagui/build/libmicrohttpd-0.9.59/src/libmicrohttpd-0.9.59-stamp/libmicrohttpd-0.9.59-configure-*.log
[ 33%] Performing build step for 'libmicrohttpd-0.9.59'
-- libmicrohttpd-0.9.59 build command succeeded.  See also ~/libsagui/build/libmicrohttpd-0.9.59/src/libmicrohttpd-0.9.59-stamp/libmicrohttpd-0.9.59-build-*.log
[ 38%] Performing install step for 'libmicrohttpd-0.9.59'
-- libmicrohttpd-0.9.59 install command succeeded.  See also ~/libsagui/build/libmicrohttpd-0.9.59/src/libmicrohttpd-0.9.59-stamp/libmicrohttpd-0.9.59-install-*.log
[ 44%] Completed 'libmicrohttpd-0.9.59'
[ 44%] Built target libmicrohttpd-0.9.59
Scanning dependencies of target sagui
[ 50%] Building C object src/CMakeFiles/sagui.dir/sg_utils.c.o
[ 55%] Building C object src/CMakeFiles/sagui.dir/sg_str.c.o
[ 61%] Building C object src/CMakeFiles/sagui.dir/sg_strmap.c.o
[ 66%] Building C object src/CMakeFiles/sagui.dir/sg_httputils.c.o
[ 72%] Building C object src/CMakeFiles/sagui.dir/sg_httpauth.c.o
[ 77%] Building C object src/CMakeFiles/sagui.dir/sg_httpuplds.c.o
[ 83%] Building C object src/CMakeFiles/sagui.dir/sg_httpreq.c.o
[ 88%] Building C object src/CMakeFiles/sagui.dir/sg_httpres.c.o
[ 94%] Building C object src/CMakeFiles/sagui.dir/sg_httpsrv.c.o
[100%] Linking C static library libsagui.a
[100%] Built target sagui
``` 

# Building the shared library (.so/.dll)

Supposing the shared library will be built using the `clang` front-end, set the variable `CMAKE_C_COMPILER` to `clang` and turn on the variable `BUILD_SHARED_LIBS`:

```bash
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=MinSizeRel -DBUILD_SHARED_LIBS=ON ..
make sagui
```

Additional variables can be enabled. For example, to build and run the library tests, choose the build type `Debug` and turn on the variable `BUILD_TESTING`:

```bash
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON -DBUILD_TESTING=ON ..
make all test
```

it compiles the library and runs its common tests, printing a log as:

```bash
...

Running tests...
Test project ~/libsagui/build
      Start  1: test_utils
 1/10 Test  #1: test_utils .......................   Passed    0.00 sec
      Start  2: test_str
 2/10 Test  #2: test_str .........................   Passed    0.00 sec
      Start  3: test_strmap
 3/10 Test  #3: test_strmap ......................   Passed    0.00 sec
      Start  4: test_httputils
 4/10 Test  #4: test_httputils ...................   Passed    0.00 sec
      Start  5: test_httpauth
 5/10 Test  #5: test_httpauth ....................   Passed    0.00 sec
      Start  6: test_httpuplds
 6/10 Test  #6: test_httpuplds ...................   Passed    0.00 sec
      Start  7: test_httpreq
 7/10 Test  #7: test_httpreq .....................   Passed    0.00 sec
      Start  8: test_httpres
 8/10 Test  #8: test_httpres .....................   Passed    0.00 sec
      Start  9: test_httpsrv
 9/10 Test  #9: test_httpsrv .....................   Passed    0.00 sec
      Start 10: test_httpsrv_curl
10/10 Test #10: test_httpsrv_curl ................   Passed    0.01 sec

100% tests passed, 0 tests failed out of 10

Total Test time (real) =   0.03 sec
```

# Building the documentation

The documentation is available in HTML and PDF formats, both generated using the [Doxygen tool](https://www.stack.nl/~dimitri/doxygen). For example:

```bash
cmake -DCMAKE_BUILD_TYPE=Release -DSG_BUILD_HTML=ON ..
make doc
```

it generates the HTML documentation, to get it in PDF, perform:

```
make pdf
```

both HTML and PDF files are saved in the `build/doc` directory.

# Building the library with TLS (HTTPS support)

The HTTPS support is possible linking the [GnuTLS](https://www.gnutls.org) library though `SG_HTTPS_SUPPORT` build option:

```bash
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=MinSizeRel -DBUILD_SHARED_LIBS=ON -DSG_HTTPS_SUPPORT=ON ..
```

**NOTE:** If the development version of the GnuTLS library is not available in the environment, the HTTPS support will be disable automatically.

# Building distribution packages

Distribution packages are available in `TAR.GZ` and `ZIP` files containing:

* library source
* static library
* shared library 

To distribute the library source, perform:

```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
make dist # or make package_source
```

it generates the files `libsagui-1.0.0.tar.gz` and `libsagui-1.0.0.zip` containing the library source.

To distribute the static library:

```bash
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=Release ..
make package
```

the package content:

```bash
libsagui-1.0.0/lib/
libsagui-1.0.0/lib/pkgconfig/
libsagui-1.0.0/lib/pkgconfig/libsagui.pc
libsagui-1.0.0/lib/libsagui.a
```

To distribute the shared library:

```bash
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=MinSizeRel -DBUILD_SHARED_LIBS=ON ..
make package
```

the package content:

```bash
libsagui-1.0.0/include/
libsagui-1.0.0/include/sagui.h
libsagui-1.0.0/lib/
libsagui-1.0.0/lib/pkgconfig/
libsagui-1.0.0/lib/pkgconfig/libsagui.pc
libsagui-1.0.0/lib/libsagui.so.1 <symbolic link>
libsagui-1.0.0/lib/libsagui.so
libsagui-1.0.0/lib/libsagui.so.1.0.0 <symbolic link>
```

# Extra buildings

**Building the shared library for Windows 32/64 bits using MinGW installed on Linux:**

```bash
cmake -DCMAKE_C_COMPILER=i686-w64-mingw32-gcc -DCMAKE_RC_COMPILER=i686-w64-mingw32-windres -DCMAKE_SYSTEM_NAME="Windows" -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX=./Output -DBUILD_SHARED_LIBS=ON ..
make sagui install/strip
```

for 64 bits:

```bash
cmake -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -DCMAKE_RC_COMPILER=x86_64-w64-mingw32-windres -DCMAKE_SYSTEM_NAME="Windows" -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX=./Output -DBUILD_SHARED_LIBS=ON ..
make sagui install/strip
```

it builds and saves the DLL in `Output/bin/libsagui-1.dll`.

**Building the shared library for Windows 32/64 bits using MinGW installed on Windows:**

```bash
cmake -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX=./Output -DBUILD_SHARED_LIBS=ON ..
make sagui install/strip
```

it builds and saves the DLL in `Output\bin\libsagui-1.dll`.

**Building and running the tests on Android using Google's NDK:**

Download the package `android-ndk-r17-linux-x86_64.zip` from the [Google's NDK page](https://developer.android.com/ndk/downloads), plug a device with Android to the USB or setup an emulator and perform the following commands:

```bash
# prepare NDK building tools (r17 or higher)
# note: change arch or api as you wish
export NDK=$HOME/android-ndk-r17
$NDK/build/tools/make_standalone_toolchain.py --force --arch arm --api 24 --install-dir /tmp/my-android-toolchain

# prepare the environment variables
toolchain=/tmp/my-android-toolchain && export PATH=$toolchain/bin:$PATH && target_host=arm-linux-androideabi && export AR=$target_host-ar && export AS=$target_host-clang && export CC=$target_host-clang && export LD=$target_host-ld && export STRIP=$target_host-strip && export CFLAGS="-fPIE -fPIC" && export LDFLAGS="-pie"

# build, upload and run the tests
cmake -DCMAKE_SYSTEM_NAME=Android -DCMAKE_ANDROID_ARM_MODE=ON -DCMAKE_SYSTEM_VERSION=24 -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a -DCMAKE_ANDROID_STANDALONE_TOOLCHAIN=$toolchain -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON ..
make all test
make clean_pushed_tests # cleans all uploaded tests

# or run the example_httpsrv example
make example_httpsrv
adb push examples/example_httpsrv /data/local/tmp/
adb shell -t /data/local/tmp/example_httpsrv
adb shell rm /data/local/tmp/example_httpsrv

# optional minimal cmake command
cmake -DCMAKE_SYSTEM_NAME=Android -DCMAKE_ANDROID_STANDALONE_TOOLCHAIN=$toolchain -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON ..

# extra - arm64
export NDK=$HOME/android-ndk-r17
$NDK/build/tools/make_standalone_toolchain.py --force --arch arm64 --api 24 --install-dir /tmp/my-android-toolchain
toolchain=/tmp/my-android-toolchain && export PATH=$toolchain/bin:$PATH && target_host=aarch64-linux-android && export AR=$target_host-ar && export AS=$target_host-clang && export CC=$target_host-clang && export LD=$target_host-ld && export STRIP=$target_host-strip && export CFLAGS="-fPIE -fPIC" && export LDFLAGS="-pie"
cmake -DCMAKE_SYSTEM_NAME=Android -DCMAKE_SYSTEM_VERSION=24 -DCMAKE_ANDROID_ARCH_ABI=arm64-v8a -DCMAKE_ANDROID_STANDALONE_TOOLCHAIN=$toolchain -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON ..
make all test
make clean_pushed_tests
```

**Building the shared library for Raspbian using `arm-linux-gnueabihf` installed on Linux:**

```code
export target_host=arm-linux-gnueabihf && export AR=$target_host-ar && export AS=$target_host-gcc && export CC=$target_host-gcc && export LD=$target_host-ld && export STRIP=$target_host-strip
cmake -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX=./Output -DBUILD_SHARED_LIBS=ON ..
make sagui install/strip
```

# Wishlist

Tests on:

* Orange PI i96
* Raspbian on ARM `x86_64`