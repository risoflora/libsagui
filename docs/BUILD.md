# Supported compilers

The Sagui library is compatible with the following compilers:

- GCC - [GNU Compiler Collection](https://gcc.gnu.org).
- LLVM - [CLang Project](https://clang.llvm.org).

and was successfully compiled in:

- GCC/Clang on many Linux distributions like Fedora, openSUSE, Debian, Ubuntu,
  Raspbian etc.
- MinGW-w64 on Linux and Windows.
- Google's [NDK](https://developer.android.com/ndk) on Linux.

## Build options

There are a few options of the Sagui building, they are:

```bash
-DSG_ABI_COMPLIANCE_CHECKER=<ON/OFF>
-DSG_BUILD_<TEST-NAME>_TESTING=<ON/OFF>
-DSG_BUILD_<EXAMPLE-NAME>_EXAMPLE=<ON/OFF>
-DSG_BUILD_EXAMPLES=<ON/OFF>
-DSG_HTTPS_SUPPORT=<ON/OFF>
-DSG_HTTP_COMPRESSION=<ON/OFF>
-DSG_PATH_ROUTING=<ON/OFF>
-DSG_PICKY_COMPILER=<ON/OFF>
-DSG_PVS_STUDIO=<ON/OFF>
```

and many specific variables, like `SG_LIBSAGUI_RC`, `GNUTLS_INCLUDE_DIR`,
`PCRE2_INCLUDE_DIR`, `PCRE2_JIT_SUPPORT` and so on. Please take a look at the
files in the [`/cmake`](https://github.com/risoflora/libsagui/tree/main/cmake)
directory for more information.

## Build types

One build type can be specified though the variable
`CMAKE_BUILD_TYPE=<Release|Debug>`. If omitted, it assumes the `Release` type by
default. A minimal command to prepare the release type is:

```bash
mkdir build
cd build/
cmake ..
```

it prints a build summary as following (assuming you are using Sagui 3.0.0):

```bash
-- The C compiler identification is GNU 9.3.1
-- Check for working C compiler: /usr/lib64/ccache/cc
-- Check for working C compiler: /usr/lib64/ccache/cc - works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Looking for __GNU_LIBRARY__
-- Looking for __GNU_LIBRARY__ - found
-- Looking for include file errno.h
-- Looking for include file errno.h - found
-- Looking for arpa/inet.h
-- Looking for arpa/inet.h - found
-- Looking for inet_ntop
-- Looking for inet_ntop - found

Sagui library 3.0.0 - building summary:

  Generator: Unix Makefiles
  Install: /usr/local
  System: Linux-5.6.7-100.fc30.x86_64 x86_64
  Compiler details:
    Executable: /usr/lib64/ccache/cc
    Version: 9.3.1
    Machine: x86_64-redhat-linux
    CFLAGS: -O2 -DNDEBUG -Wall -Werror -Wextra -Wpedantic -Wdeclaration-after-statement -Wstrict-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline
  Build: Release-x86_64 (shared)
  Additional features:
    HTTPS support: No
    HTTP compression: Yes
    Path routing: Yes
  Examples: Yes (str, strmap, httpauth, httpcookie, httpsrv, httpuplds, httpsrv_benchmark, httpsrv_sse, httpreq_payload, httpreq_isolate, httpcomp, entrypoint, router_simple, router_segments, router_vars, router_srv)
  Docs: No
  Run tests: No
  cURL tests: No

-- Configuring done
-- Generating done
-- Build files have been written to: ~/libsagui/build
```

Finally, build the shared library:

```bash
make sagui
```

it prints a log as following:

```bash
Scanning dependencies of target pcre2-10.34
[  2%] Creating directories for 'pcre2-10.34'
[  5%] Performing download step (download, verify and extract) for 'pcre2-10.34'
-- pcre2-10.34 download command succeeded.  See also ~/libsagui/build/pcre2-10.34/src/pcre2-10.34-stamp/pcre2-10.34-download-*.log
[  7%] No patch step for 'pcre2-10.34'
[ 10%] No update step for 'pcre2-10.34'
[ 13%] Performing configure step for 'pcre2-10.34'
-- pcre2-10.34 configure command succeeded.  See also ~/libsagui/build/pcre2-10.34/src/pcre2-10.34-stamp/pcre2-10.34-configure-*.log
[ 15%] Performing build step for 'pcre2-10.34'
-- pcre2-10.34 build command succeeded.  See also ~/libsagui/build/pcre2-10.34/src/pcre2-10.34-stamp/pcre2-10.34-build-*.log
[ 18%] Performing install step for 'pcre2-10.34'
-- pcre2-10.34 install command succeeded.  See also ~/libsagui/build/pcre2-10.34/src/pcre2-10.34-stamp/pcre2-10.34-install-*.log
[ 21%] Completed 'pcre2-10.34'
[ 21%] Built target pcre2-10.34
Scanning dependencies of target libmicrohttpd-0.9.70
[ 23%] Creating directories for 'libmicrohttpd-0.9.70'
[ 26%] Performing download step (download, verify and extract) for 'libmicrohttpd-0.9.70'
-- libmicrohttpd-0.9.70 download command succeeded.  See also ~/libsagui/build/libmicrohttpd-0.9.70/src/libmicrohttpd-0.9.70-stamp/libmicrohttpd-0.9.70-download-*.log
[ 28%] No patch step for 'libmicrohttpd-0.9.70'
[ 31%] No update step for 'libmicrohttpd-0.9.70'
[ 34%] Performing configure step for 'libmicrohttpd-0.9.70'
-- libmicrohttpd-0.9.70 configure command succeeded.  See also ~/libsagui/build/libmicrohttpd-0.9.70/src/libmicrohttpd-0.9.70-stamp/libmicrohttpd-0.9.70-configure-*.log
[ 36%] Performing build step for 'libmicrohttpd-0.9.70'
-- libmicrohttpd-0.9.70 build command succeeded.  See also ~/libsagui/build/libmicrohttpd-0.9.70/src/libmicrohttpd-0.9.70-stamp/libmicrohttpd-0.9.70-build-*.log
[ 39%] Performing install step for 'libmicrohttpd-0.9.70'
-- libmicrohttpd-0.9.70 install command succeeded.  See also ~/libsagui/build/libmicrohttpd-0.9.70/src/libmicrohttpd-0.9.70-stamp/libmicrohttpd-0.9.70-install-*.log
[ 42%] Completed 'libmicrohttpd-0.9.70'
[ 42%] Built target libmicrohttpd-0.9.70
Scanning dependencies of target zlib-1.2.11
[ 44%] Creating directories for 'zlib-1.2.11'
[ 47%] Performing download step (download, verify and extract) for 'zlib-1.2.11'
-- zlib-1.2.11 download command succeeded.  See also ~/libsagui/build/zlib-1.2.11/src/zlib-1.2.11-stamp/zlib-1.2.11-download-*.log
[ 50%] No patch step for 'zlib-1.2.11'
[ 52%] No update step for 'zlib-1.2.11'
[ 55%] Performing configure step for 'zlib-1.2.11'
-- zlib-1.2.11 configure command succeeded.  See also ~/libsagui/build/zlib-1.2.11/src/zlib-1.2.11-stamp/zlib-1.2.11-configure-*.log
[ 57%] Performing build step for 'zlib-1.2.11'
-- zlib-1.2.11 build command succeeded.  See also ~/libsagui/build/zlib-1.2.11/src/zlib-1.2.11-stamp/zlib-1.2.11-build-*.log
[ 60%] Performing install step for 'zlib-1.2.11'
-- zlib-1.2.11 install command succeeded.  See also ~/libsagui/build/zlib-1.2.11/src/zlib-1.2.11-stamp/zlib-1.2.11-install-*.log
[ 63%] Completed 'zlib-1.2.11'
[ 63%] Built target zlib-1.2.11
Scanning dependencies of target sagui
[ 65%] Building C object src/CMakeFiles/sagui.dir/sg_utils.c.o
[ 68%] Building C object src/CMakeFiles/sagui.dir/sg_extra.c.o
[ 71%] Building C object src/CMakeFiles/sagui.dir/sg_str.c.o
[ 73%] Building C object src/CMakeFiles/sagui.dir/sg_strmap.c.o
[ 76%] Building C object src/CMakeFiles/sagui.dir/sg_httpauth.c.o
[ 78%] Building C object src/CMakeFiles/sagui.dir/sg_httpuplds.c.o
[ 81%] Building C object src/CMakeFiles/sagui.dir/sg_httpreq.c.o
[ 84%] Building C object src/CMakeFiles/sagui.dir/sg_httpres.c.o
[ 86%] Building C object src/CMakeFiles/sagui.dir/sg_httpsrv.c.o
[ 89%] Building C object src/CMakeFiles/sagui.dir/sg_entrypoint.c.o
[ 92%] Building C object src/CMakeFiles/sagui.dir/sg_entrypoints.c.o
[ 94%] Building C object src/CMakeFiles/sagui.dir/sg_routes.c.o
[ 97%] Building C object src/CMakeFiles/sagui.dir/sg_router.c.o
[100%] Linking C shared library libsagui.so
[100%] Built target sagui
```

## Building the shared library (.so/.dll)

Supposing the shared library will be built using the `clang` front-end, set the
variable `CMAKE_C_COMPILER` to `clang`:

```bash
cmake -DCMAKE_C_COMPILER=clang ..
make sagui
```

Additional variables can be enabled. For example, to build and run the library
tests, choose the build type `Debug` and turn on the variable `BUILD_TESTING`:

```bash
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON ..
make all test
```

it compiles the library and runs its common tests, printing a log as:

```bash
Running tests...
Test project ~/libsagui/build
      Start  1: test_utils
 1/15 Test  #1: test_utils .......................   Passed    0.00 sec
      Start  2: test_extra
 2/15 Test  #2: test_extra .......................   Passed    0.00 sec
      Start  3: test_str
 3/15 Test  #3: test_str .........................   Passed    0.00 sec
      Start  4: test_strmap
 4/15 Test  #4: test_strmap ......................   Passed    0.00 sec
      Start  5: test_httpauth
 5/15 Test  #5: test_httpauth ....................   Passed    0.00 sec
      Start  6: test_httpuplds
 6/15 Test  #6: test_httpuplds ...................   Passed    0.00 sec
      Start  7: test_httpreq
 7/15 Test  #7: test_httpreq .....................   Passed    0.00 sec
      Start  8: test_httpres
 8/15 Test  #8: test_httpres .....................   Passed    0.00 sec
      Start  9: test_httpsrv
 9/15 Test  #9: test_httpsrv .....................   Passed    0.03 sec
      Start 10: test_entrypoint
10/15 Test #10: test_entrypoint ..................   Passed    0.00 sec
      Start 11: test_entrypoints
11/15 Test #11: test_entrypoints .................   Passed    0.00 sec
      Start 12: test_routes
12/15 Test #12: test_routes ......................   Passed    0.00 sec
      Start 13: test_router
13/15 Test #13: test_router ......................   Passed    0.00 sec
      Start 14: test_httpsrv_curl
14/15 Test #14: test_httpsrv_curl ................   Passed    0.52 sec
      Start 15: test_httpsrv_tls_curl
15/15 Test #15: test_httpsrv_tls_curl ............   Passed    0.04 sec

100% tests passed, 0 tests failed out of 15

Total Test time (real) =   0.64 sec
```

## Building the documentation

The documentation is available in HTML format which can be generated using the
[Doxygen tool](http://www.doxygen.nl). For example:

```bash
cmake -DSG_BUILD_HTML=ON ..
make doc
```

it generates the HTML reference into `build/docs` directory.

## Building the library with TLS (HTTPS support)

The HTTPS support is possible linking the [GnuTLS](https://www.gnutls.org)
library though `SG_HTTPS_SUPPORT` build option:

```bash
cmake -DCMAKE_C_COMPILER=clang -DSG_HTTPS_SUPPORT=ON ..
```

**NOTE:** If the development version of the GnuTLS library is not available in
the environment, the HTTPS support will be disable automatically.

## Building distribution packages

Distribution packages are available in `TAR.GZ` and `ZIP` files containing:

- library source
- static library
- shared library

To distribute the library source, perform:

```bash
cmake ..
make dist # or "make package_source"
```

it generates the files `libsagui-3.0.0.tar.gz` and `libsagui-3.0.0.zip`
containing the library source.

To distribute the static library:

```bash
cmake -DCMAKE_C_COMPILER=clang -DBUILD_SHARED_LIBS=OFF ..
make package
```

the package content:

```bash
tar -ztf libsagui-3.0.0.tar.gz
libsagui-3.0.0/include/
libsagui-3.0.0/include/sagui.h
libsagui-3.0.0/lib64/
libsagui-3.0.0/lib64/pkgconfig/
libsagui-3.0.0/lib64/pkgconfig/libsagui.pc
libsagui-3.0.0/lib64/libsagui.a
```

To distribute the shared library:

```bash
cmake -DCMAKE_C_COMPILER=clang ..
make package
```

the package content:

```bash
tar -ztf libsagui-3.0.0.tar.gz
libsagui-3.0.0/include/
libsagui-3.0.0/include/sagui.h
libsagui-3.0.0/lib64/
libsagui-3.0.0/lib64/libsagui.so.3.0.0
libsagui-3.0.0/lib64/pkgconfig/
libsagui-3.0.0/lib64/pkgconfig/libsagui.pc
libsagui-3.0.0/lib64/libsagui.so <symbolic link>
libsagui-3.0.0/lib64/libsagui.so.3 <symbolic link>
```

## Extra buildings

**Building the shared library for Windows 32/64 bits using MinGW installed on Linux:**

```bash
cmake -DCMAKE_TOOLCHAIN_FILE="../cmake/Toolchain-mingw32-Linux.cmake" ..
make sagui install/strip
```

for 64 bits:

```bash
cmake \
  -DMINGW64=ON \
  -DCMAKE_TOOLCHAIN_FILE="../cmake/Toolchain-mingw32-Linux.cmake" ..
make sagui install/strip
```

it builds and saves the DLL in `Output/bin/libsagui-2.dll`.

**NOTE:** the package `mingw32-winpthreads-static` (or `mingw64-winpthreads-static`
for 64 bits) must be installed in some Linux distributions (e.g: Fedora and openSUSE).

**Building the shared library for Windows 32/64 bits using MinGW installed on Windows:**

```bash
cmake -G "MSYS Makefiles" ..
make sagui install/strip
```

it builds and saves the DLL in `Output\bin\libsagui-2.dll`.

**Building and running the tests on Android using Google's NDK:**

Download the package `android-ndk-r18b-linux-x86_64.zip` from the
[Google's NDK page](https://developer.android.com/ndk/downloads), plug a device
with Android to the USB or setup an emulator and perform the following commands:

```bash
# prepare NDK building tools (r18b)
# note: change arch or api as you wish
export NDK=$HOME/android-ndk-r18b && \
  $NDK/build/tools/make_standalone_toolchain.py --force --arch arm --api 24 \
  --install-dir /tmp/my-android-toolchain

# prepare the environment variables
toolchain=/tmp/my-android-toolchain && \
  target_host=arm-linux-androideabi && \
  export PATH=$toolchain/bin:$PATH && \
  export AR=$target_host-ar && \
  export AS=$target_host-clang && \
  export CC=$target_host-clang && \
  export LD=$target_host-ld && \
  export STRIP=$target_host-strip && \
  export CFLAGS="-fPIE -fPIC" && \
  export LDFLAGS="-pie"

# build, upload and run the tests
cmake \
  -DCMAKE_SYSTEM_NAME=Android \
  -DCMAKE_ANDROID_ARM_MODE=ON \
  -DCMAKE_SYSTEM_VERSION=24 \
  -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a \
  -DCMAKE_ANDROID_STANDALONE_TOOLCHAIN=$toolchain \
  -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_SHARED_LIBS=OFF \
  -DBUILD_TESTING=ON ..
make all test
make clean_pushed_tests # cleans all uploaded tests

# or run the example_httpsrv example
make example_httpsrv
adb push examples/example_httpsrv /data/local/tmp/
adb shell -t /data/local/tmp/example_httpsrv
adb shell rm /data/local/tmp/example_httpsrv

# optional minimal cmake command
cmake \
  -DCMAKE_SYSTEM_NAME=Android \
  -DCMAKE_ANDROID_STANDALONE_TOOLCHAIN=$toolchain \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TESTING=ON ..

# extra - arm64
export NDK=$HOME/android-ndk-r18b && \
  $NDK/build/tools/make_standalone_toolchain.py --force --arch arm64 --api 24 \
  --install-dir /tmp/my-android-toolchain
toolchain=/tmp/my-android-toolchain && \
  target_host=aarch64-linux-android && \
  export PATH=$toolchain/bin:$PATH && \
  export AR=$target_host-ar && \
  export AS=$target_host-clang && \
  export CC=$target_host-clang && \
  export LD=$target_host-ld && \
  export STRIP=$target_host-strip && \
  export CFLAGS="-fPIE -fPIC" && \
  export LDFLAGS="-pie"
cmake \
  -DCMAKE_SYSTEM_NAME=Android \
  -DCMAKE_SYSTEM_VERSION=24 \
  -DCMAKE_ANDROID_ARCH_ABI=arm64-v8a \
  -DCMAKE_ANDROID_STANDALONE_TOOLCHAIN=$toolchain \
  -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TESTING=ON ..
make all test
make clean_pushed_tests
```

**Building the shared library for Raspbian using `arm-linux-gnu` installed
on Linux:**

```code
# Assuming you are on Fedora, install the "arm-linux-gnu":
# sudo dnf install gcc-arm-linux-gnu
# sudo dnf install glibc-arm-linux-gnu
export target_host=arm-linux-gnu && \
  export AR=$target_host-ar && \
  export AS=$target_host-gcc && \
  export CC=$target_host-gcc && \
  export LD=$target_host-ld && \
  export STRIP=$target_host-strip
cmake \
  -DCMAKE_C_COMPILER=arm-linux-gnu-gcc \
  -DCMAKE_INSTALL_PREFIX=./Output ..
make sagui install/strip
```
