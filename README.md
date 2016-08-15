libtwiddle
==========
[![Build Status](https://travis-ci.org/fsaintjacques/libtwiddle.svg?branch=develop)](https://travis-ci.org/fsaintjacques/libtwiddle)
[![License](https://img.shields.io/badge/license-LGPL--3.0-blue.svg?style=flat)](https://github.com/fsaintjacques/libtwiddle/blob/develop/LICENSE)


libtwiddle is a data structure library aiming for speed on modern
Linux x86-64 systems. The following data structures are implemented:

  * bitmaps (dense & RLE);
  * Bloom filters (standard & active-active);
  * HyperLogLog
  * MinHash

See [`EXAMPLES.md`](EXAMPLES.md) and
[`tests/examples/`](tests/examples/) for examples.

Why should you use libtwiddle?
------------------------------

  * Written for the modern age; `gcc >= 4.8`, using C11 features, on x86-64
  * Extensive testing; unit tests and random property testing with `hypothesis`
  * Python bindings; courtesy of property testing
  * Vectorized implementations; AVX, AVX2, and partial support for AVX512
  * Continuous integration

Install
=======

libtwiddle uses CMake as its build manager.

Prerequisite libraries
----------------------

To build libtwiddle, you need the following:

  * a recent C compiler (`gcc >= 4.8` or `clang >= 3.5`).
  * CMake build tool (`cmake >= 3.0`)

Building from source
--------------------

In most cases, you should be able to build the source code using the following:

    $ mkdir build
    $ cd build
    $ cmake .. -DCMAKE_INSTALL_PREFIX=$PREFIX
    $ make
    $ make test
    $ make install

Building with SIMD support
--------------------------

By default, libtwiddle will compile with AVX SIMD instructions. Use
the following flags to enable newer instructions:

  * For AVX2:   `-DUSE_AVX2=ON`;
  * for AVX512: `-DUSE_AVX512=ON`.

Note that AVX2 implies AVX, and AVX512 implies AVX2. Some functions
can't be implemented with AVX512, and will fallback to AVX2 code.

To compile without SIMD support, invoke CMake with `-DUSE_AVX=OFF
-DUSE_AVX2=OFF -DUSE_AVX512=OFF`.

Contributions
-------------

Contributions are more than welcome, see
[`CONTRIBUTING.md`](CONTRIBUTING.md) for details.
