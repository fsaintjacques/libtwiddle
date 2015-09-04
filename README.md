LIBTWIDDLE
==========

libtwiddle is a small library that implements bit data structures:

  * bitmaps
  * bloom filters
  * tries

INSTALL
=======

libtwiddle uses cmake as its build manager.

Prerequisite libraries
----------------------

To build libtwiddle, you need the following libraries installed on
your system:

  * pkg-config
  * check (http://check.sourceforge.net)

Building from source
--------------------

In most cases, you should be able to build the source code using the following:

    $ mkdir build
    $ cd build
    $ cmake .. -DCMAKE_INSTALL_PREFIX=$PREFIX
    $ make
    $ make test
    $ make install

You might have to run the last command using sudo, if you need administrative
privileges to write to the $PREFIX directory.
