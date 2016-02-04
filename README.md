LIBTWIDDLE
==========
[![Build Status](https://travis-ci.org/fsaintjacques/libtwiddle.svg?branch=develop)](https://travis-ci.org/fsaintjacques/libtwiddle)
[![License](https://img.shields.io/badge/license-LGPL--3.0-blue.svg?style=flat)](https://github.com/fsaintjacques/libtwiddle/blob/develop/LICENSE)


libtwiddle is a data structure library aimed with speed for modern `x86-64` on
linux. The following data structures are implemented:

  * bitmaps (dense & RLE)
  * bloom filters (standard & active-active)
  * hyperloglog
  * minhash

See `EXAMPLES.md` or `tests/examples/` for examples.

Why should you use libtwiddle?
------------------------------

  * Written for modern age; `gcc >= 4.8`, `C11`, and `x86-64`
  * Extensive testing; unit tests and random property testing with `hypothesis`
  * Python bindings; courtesy of property testing
  * Vectorized implementations; `AVX`, `AVX2`, and partial `AVX512`
  * Continuous integration

INSTALL
=======

libtwiddle uses cmake as its build manager.

Prerequisite libraries
----------------------

To build libtwiddle, you need the following libraries installed on
your system:

  * pkg-config
  * check (http://check.sourceforge.net)
  * latest C compilers (gcc >= 4.8, clang >= 3.5)

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

By default, libtwiddle will compile with `AVX` SIMD instructions. If you wish to
enable newer instructions, the following flags provide the option to do so:

  * AVX2;   `-DUSE_AVX2=ON`
  * AVX512; `-DUSE_AVX512=ON`

Note that `AVX2` implies activating `AVX`, and `AVX512` implies `AVX2`, etc.
Some functions can't be implemented with `AVX512`, thus fullback on `AVX2`.

If one wants to compile without SIMD support, cmake can be invoked with
`-DUSE_AVX=OFF -DUSE_AVX2=OFF -DUSE_AVX512=OFF`.

Contributions
-------------

Contributions are more than welcome, see `CONTRIBUTING.md` for detailed
explanations.
