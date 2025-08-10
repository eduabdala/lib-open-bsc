# lib-open-bsc

This document explains how to build and install lib-open-bsc on Linux and Windows using the provided toolchains.

## Prerequisites

* CMake 3.15 or later
* Compiler with C++17 support

  * Linux: GCC or Clang
  * Windows: MinGW (via MSYS2/MinGW64)

## Building for Linux

```bash
mkdir -p build/linux/release
cd build/linux/release
cmake ../../../ -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## Building for Windows (MinGW)

```bash
cd C:/Azure/tool-fw-sdk
cmake \
    -S . \
    -B build/windows/release \
    -DCMAKE_TOOLCHAIN_FILE=build/windows/toolchain.cmake \
    -G "MinGW Makefiles" \
    -DCMAKE_BUILD_TYPE=Release
cmake --build build/windows/release -- -j
```

## Installing Artifacts

To copy the executable (Linux) and the DLL (Windows) to the project's `bin` directory, run:

```bash
# Linux
cmake --install build/linux/release --prefix .
```

```bash
# Windows
cmake --install build/windows/release --prefix .
```

After installation, you will have:

```
bin/MediumTerminal.exe
```

