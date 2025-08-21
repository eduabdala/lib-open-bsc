# lib-open-bsc

<!-- Badges -->

![Project Status](https://img.shields.io/badge/status-in%20development-yellow)
![Version](https://img.shields.io/badge/version-1.0.0-blue)
![License](https://img.shields.io/badge/license-MIT-green)
![Build](https://img.shields.io/badge/build-unavailable-red)

---

## Description

This project provides **lib-open-bsc**, a library with build and install instructions for Linux and Windows using the provided toolchains.

---

## Table of Contents

* [Prerequisites](#prerequisites)
* [Building for Linux](#building-for-linux)
* [Building for Windows](#building-for-windows)
* [Installing Artifacts](#installing-artifacts)
* [Contact](#contact)

---

## Prerequisites

* CMake 3.15 or later
* Compiler with C++17 support

  * Linux: GCC or Clang
  * Windows: MinGW (via MSYS2/MinGW64)

---

## Building for Linux

```bash
mkdir -p build/linux/release
cd build/linux/release
cmake ../../../ -DCMAKE_BUILD_TYPE=Release
cmake --build .
cd ../../../
```

---

## Building for Windows (MinGW)

```bash
cmake \
    -S . \
    -B build/windows/release \
    -DCMAKE_TOOLCHAIN_FILE=build/windows/toolchain.cmake \
    -G "MinGW Makefiles" \
    -DCMAKE_BUILD_TYPE=Release
cmake --build build/windows/release -- -j
```

---

## Installing Artifacts

To copy the executable (Linux) and DLL (Windows) to the project's `bin` directory, run:

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

---

## Contact

* GitHub: [username](https://github.com/eduabdala)
* Email: [eduardoabdala9@outlook.com](eduardoabdala9@outlook.com)

---

<!-- Personal Notes -->

> Always keep the README updated, especially badges, version, and project status.
