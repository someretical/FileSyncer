# Building

## Windows

All commands are assumed to be run from the VS >2022 developer command prompt (>x86-64).

### zlib

```bash
# ZLIB_DIR = <path to zlib repository root>
> cmake --fresh
> cmake --build . --config Debug
> cmake --build . --config Release
```

Now the `.lib` and `.dll` files are in `ZLIB_DIR\Debug` and `ZLIB_DIR\Release`. Release files are not necessarily compatible with debug files (and vice versa) when linking against them.

The header files are in `ZLIB_DIR`.

TODO figure out how to build zlib into a static `.lib` file.
> Note that `.LIB` files can be either static libraries (containing object files) or import libraries (containing symbols to allow the linker to link to a DLL).

### Botan

#### Static
```bash
# BOTAN_DIR = <path to botan repository root>
> python configure.py --cc=msvc --os=windows --build-targets=static
> nmake
```
Include directory is `BOTAN_DIR/build/include`, `.lib` file located at `BOTAN_DIR/botan-3.lib`.

```cmake
add_library(Botan IMPORTED UNKNOWN)
set_target_properties(
        Botan::Botan
        PROPERTIES
        IMPORTED_LOCATION "BOTAN_DIR/botan-3.lib"
        INTERFACE_INCLUDE_DIRECTORIES "BOTAN_DIR/build/include"
)
```

#### Dynamic
```
> python configure.py --cc=msvc --os=windows --build-targets=shared
```

Add `--with-zlib --with-external-includedir=ZLIB_DIR --with-external-libdir=ZLIB_DIR\Release` to the above command to build with zlib.

Include directory is `BOTAN_DIR/build/include`, `.lib` file located at `BOTAN_DIR/botan-3.lib`, `.dll` file located at `BOTAN_DIR/botan-3.dll`.

```cmake
add_library(Botan IMPORTED UNKNOWN)
set_target_properties(
        Botan
        PROPERTIES
        IMPORTED_LOCATION "BOTAN_DIR/botan-3.lib"
        INTERFACE_INCLUDE_DIRECTORIES "BOTAN_DIR/build/include"
)
```
Make sure to deploy `botan-3.dll` with the executable!

#### Amalgamation
```bash
> python configure.py --cc=msvc --os=windows --minimized-build --disable-shared --amalgamation --enable-modules=argon2,system_rng
```
Include directory is `BOTAN_DIR`.

```cmake
add_library(Botan STATIC)
target_sources(
        Botan
        PUBLIC
        BOTAN_DIR/botan_all.h
        PRIVATE
        BOTAN_DIR/botan_all.cpp
)
target_include_directories(
        Botan
        PUBLIC
        BOTAN_DIR
)
```

### OpenSSL

```
> set OPENSSL_BASE_DIR=E:\openssl
> set OPENSSL_OUTPUT_DIR=%OPENSSL_BASE_DIR%\VC-WIN64A-static
> set OPENSSL_BUILD_DIR=%OPENSSL_BASE_DIR%\build

> mkdir %OPENSSL_BUILD_DIR%
> pushd %OPENSSL_BUILD_DIR%
> mkdir %OPENSSL_OUTPUT_DIR%
> perl %OPENSSL_BASE_DIR%\Configure VC-WIN64A no-shared --prefix=%OPENSSL_OUTPUT_DIR% --openssldir=%OPENSSL_OUTPUT_DIR%
> nmake
> nmake install
```

Notes:
- `no-shared` is required to build .lib files that can be linked against statically. We do not want .lib files that actually point to .dll files.

### Qt 6.x.x

**NOTE THAT OPENSSL_OUTPUT_DIR MUST USE FORWARD SLASHES!!!**

```bash
> set OPENSSL_OUTPUT_DIR=E:/openssl/VC-WIN64A-static
> set QT_BASE_DIR=E:\Qt\6.5.0
> mkdir %QT_BASE_DIR%\build
> mkdir %QT_BASE_DIR%\msvc2019_64-static-release
> pushd %QT_BASE_DIR%\build
> %QT_BASE_DIR%\Src\configure ^
-prefix %QT_BASE_DIR%\msvc2019_64-static-release ^
-release ^
-static ^
-static-runtime ^
-c++std c++20 ^
-make libs ^
-nomake tools ^
-nomake examples ^
-nomake tests ^
-nomake benchmarks ^
-nomake manual-tests ^
-nomake minimal-static-tests ^
-submodules qtbase,qtsvg,qtcharts ^
-skip qtimageformats ^
-skip qtlanguageserver ^
-skip qtshadertools ^
-skip qtdeclarative ^
-skip qtquicktimeline ^
-skip qtquick3d ^
-skip qtmultimedia ^
-skip qt3d ^
-skip qt5compat ^
-skip qtactiveqt ^
-skip qtcoap ^
-skip qtconnectivity ^
-skip qtdatavis3d ^
-skip qtwebsockets ^
-skip qthttpserver ^
-skip qttools ^
-skip qtserialport ^
-skip qtpositioning ^
-skip qtwebchannel ^
-skip qtwebengine ^
-skip qtdoc ^
-skip qtgrpc ^
-skip qtinsighttracker ^
-skip qtlocation ^
-skip qtlottie ^
-skip qtmqtt ^
-skip qtnetworkauth ^
-skip qtopcua ^
-skip qtquick3dphysics ^
-skip qtquickeffectmaker ^
-skip qtremoteobjects ^
-skip qtscxml ^
-skip qtsensors ^
-skip qtserialbus ^
-skip qtspeech ^
-skip qttranslations ^
-skip qtvirtualkeyboard ^
-skip qtwayland ^
-skip qtwebview ^
-openssl-linked -- -D OPENSSL_USE_STATIC_LIBS=ON -D OPENSSL_ROOT_DIR=%OPENSSL_OUTPUT_DIR%
> cmake  --build . --parallel 16
> cmake --install .
```

Distinction between x86_amd64 and amd64_x86 in CLion

Add environment variable `OPENSSL_ROOT_DIR=%OPENSSL_OUTPUT_DIR%`.
Add Qt dynamic libraries to `PATH`. `E:\Qt\6.5.0\msvc2019_64\bin`.

```cmake
IF (CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_PREFIX_PATH "E:/Qt/6.5.0/msvc2019_64-static-release/lib/cmake")

    IF (MSVC)
        add_compile_options($<$<CONFIG:Release>:/MT>)
    ENDIF ()
ELSEIF (CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_PREFIX_PATH "E:/Qt/6.5.0/msvc2019_64/lib/cmake")
ENDIF ()
```