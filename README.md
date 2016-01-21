LIBTWIDDLE
==========
[![Build Status](https://travis-ci.org/fsaintjacques/libtwiddle.svg?branch=develop)](https://travis-ci.org/fsaintjacques/libtwiddle)
[![License](https://img.shields.io/badge/license-LGPL--3.0-blue.svg?style=flat)](https://github.com/fsaintjacques/libtwiddle/blob/develop/LICENSE)


libtwiddle is a small library that implements bit data structures:

  * bitmaps (dense & RLE)
  * bloom filters (standard & active-active)
  * hyperloglog
  * minhash

See `EXAMPLES.md` or `tests/examples/` for examples.

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

Contributions
-------------

Contributions are more than welcome, see `CONTRIBUTING.md` for detailed
explanations.
