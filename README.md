![sleipnir_logo.png](./docs/images/sleipnir_logo.png)

## What is Sleipnir?

Sleipnir is a cross-platform C99 library for writing neural networks

![sleipnir.png](./docs/images/sleipnir.png)

## Getting Started

This guide will help you get up and running with the project.

## Prerequisites

Before you begin, ensure that you have the following installed on your system:

- **Git**
- **CMake 3.13+**
- **Clang/GCC**

## Quickstart

Follow these steps to build `sleipnir`:

*Build `vigenere_decrypter` command line application:*
```shell
git clone https://github.com/neural-sleipnir/sleipnir.git
cmake -S. -Bbuild -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j
```

*To run tests use the following commands:*
```shell
cmake -S. -Bbuild -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug -j
cmake --build build --config Debug --target test
```

## Documentation

The docs directory contains design and coding style description.

* [Design Doc](docs/design.md) covers how sleipnir works
  underneath the hood, and why certain design choices were made.
* [Coding Style](docs/coding-style.md) covers coding style such as naming and explains how to manage allocated object by style.
