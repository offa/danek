# Danek

[![Build Status](https://travis-ci.org/offa/danek.svg?branch=master)](https://travis-ci.org/offa/danek)
[![GitHub release](https://img.shields.io/github/release/offa/danek.svg)](https://github.com/offa/danek/releases)
[![License](https://img.shields.io/badge/license-MIT-yellow.svg)](LICENSE)
![C++](https://img.shields.io/badge/c++-14-green.svg)

A configuration library for C++. This is a fork of [**config4cpp**](https://github.com/config4star/config4cpp) which aims to modernization and quality improvements.

Please see [Contributing](CONTRIBUTING.md) for how to contribute to this project. For a ready to use version of *config4cpp* with *CMake* support, please see [here](https://github.com/offa/config4cpp).

## Goals

- Port the Code to *Modern C++*
- Improve code quality
- Full Unit Test Suite
- Port to CMake
- Maintain file compatible


## Requirements

- [**CMake**](http://www.cmake.org/)
- [**GTest**](https://github.com/google/googletest) (*Optional*)



## Building

Build and execute Unit Tests:

```
mkdir build && cd build
cmake ..
make
make unittest
```

Alternatively execute CTest: `make test` or `ctest`.


## License

[**MIT License**](LICENSE)

***Third party licenses*** are available in [*licenses*](licenses).

