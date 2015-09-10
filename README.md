LIBTWIDDLE
==========

libtwiddle is a small library that implements bit data structures:

  * bitmaps
  * bloom filters
  * tries

USAGE
=====

bitmap
------

```C
#include <assert.h>
#include <libtwiddle/bitmap.h>

int main(int argc, char* argv[]) {
  const uint32_t nbits = 1024;
  struct tw_bitmap* bitmap = tw_bitmap_new(nbits);

  assert(bitmap);

  tw_bitmap_set(bitmap, 512);
  assert(tw_bitmap_test_and_clear(bitmap, 512));
  assert(!tw_bitmap_test(bitmap, 512));

  tw_bitmap_set(bitmap, 768);
  assert(tw_bitmap_find_first_bit(bitmap) == 768);

  return 0;
}
```

bloomfilter
-----------

```C
#include <assert.h>
#include <string.h>

#include <libtwiddle/bloomfilter.h>

int main(int argc, char *argv[]) {
  const uint32_t nbits = 1024;
  const uint32_t k = 7;
  struct tw_bloomfilter *bf = tw_bloomfilter_new(nbits, k);
  assert(bf);

  char *values[] = {"herp", "derp", "ferp", "merp"};

  for (int i = 0; i < ((sizeof(values) / sizeof(values[0]))); ++i) {
    tw_bloomfilter_set(bf, strlen(values[i]), values[i]);
    assert(tw_bloomfilter_test(bf, strlen(values[i]), values[i]));
  }

  assert(!tw_bloomfilter_test(bf, sizeof("nope"), "nope"));

  return 0;
}
```

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
