EXAMPLES
========

bitmap
------

```C
#include <assert.h>
#include <twiddle/bitmap/bitmap.h>

int main() {
  const uint64_t nbits = 1024;
  struct tw_bitmap* bitmap = tw_bitmap_new(nbits);

  assert(bitmap);

  tw_bitmap_set(bitmap, 512);
  assert(tw_bitmap_test_and_clear(bitmap, 512));
  assert(!tw_bitmap_test(bitmap, 512));

  tw_bitmap_set(bitmap, 768);
  assert(tw_bitmap_find_first_bit(bitmap) == 768);

  tw_bitmap_free(bitmap);

  return 0;
}
```

bitmap-rle
------

```C
#include <assert.h>
#include <twiddle/bitmap/bitmap_rle.h>

int main() {
  /** allocate a bitmap containing 2 billions bits */
  const uint64_t nbits = 1UL << 31;
  struct tw_bitmap_rle* bitmap = tw_bitmap_rle_new(nbits);

  assert(bitmap);

  /** fill 1 billion bits */
  const uint64_t start = 0UL;
  const uint64_t end = 1UL << 30;
  tw_bitmap_rle_set_range(bitmap, start, end);

  /**
   * bitmap_rle DOES NOT support setting bits in non sorted order, e.g.
   * calling the following will raise an assert() call (or undefined behaviour
   * if compiled with NDEBUG):
   *
   * tw_bitmap_rle_set(bitmap, start - 1);
   */

  assert(tw_bitmap_rle_test(bitmap, start));
  assert(tw_bitmap_rle_test(bitmap, end));
  assert(tw_bitmap_rle_find_first_bit(bitmap)  == (int64_t)start);
  assert(tw_bitmap_rle_find_first_zero(bitmap) == (int64_t)end + 1);

  tw_bitmap_rle_free(bitmap);

  return 0;
}
```

bloomfilter
-----------

```C
#include <assert.h>
#include <string.h>

#include <twiddle/bloomfilter/bloomfilter.h>

int main() {
  const uint64_t nbits = 1024;
  const uint16_t k = 7;
  struct tw_bloomfilter *bf = tw_bloomfilter_new(nbits, k);
  assert(bf);

  const char *values[] = {"herp", "derp", "ferp", "merp"};

  for (size_t i = 0; i < ((sizeof(values) / sizeof(values[0]))); ++i) {
    tw_bloomfilter_set(bf, strlen(values[i]), values[i]);
    assert(tw_bloomfilter_test(bf, strlen(values[i]), values[i]));
  }

  assert(!tw_bloomfilter_test(bf, sizeof("nope"), "nope"));

  return 0;
}
```

hyperloglog
-----------

```C
#include <assert.h>
#include <stdio.h>

#include <twiddle/hyperloglog/hyperloglog.h>

int main() {
  const uint8_t precision = 16;
  struct tw_hyperloglog *hll = tw_hyperloglog_new(precision);
  assert(hll);

  const uint32_t n_elems = 10 * (1 << precision);
  for (size_t i = 0; i < n_elems ; ++i) {
    tw_hyperloglog_add(hll, sizeof(i), (char *) &i);
  }

  printf("estimated count: %f, real count: %d\n",
         tw_hyperloglog_count(hll),
         n_elems);

  tw_hyperloglog_free(hll);

  return 0;
}
```

minhash
-----------

```C
#include <assert.h>
#include <stdio.h>

#include <twiddle/hash/minhash.h>

int main()
{
  const uint32_t n_registers = 1 << 13;
  struct tw_minhash *a = tw_minhash_new(n_registers);
  assert(a);
  struct tw_minhash *b = tw_minhash_clone(a);
  assert(b);

  const uint32_t n_elems = 10 * n_registers;
  for (size_t i = 0; i < n_elems; ++i) {
    if (i % 3 == 0) {
      tw_minhash_add(a, (char *)&i, sizeof(i));
    }

    if (i % 5 == 0) {
      tw_minhash_add(b, (char *)&i, sizeof(i));
    }
  }

  printf("estimated jaccard: %f\n", tw_minhash_estimate(a, b));

  tw_minhash_free(b);
  tw_minhash_free(a);

  return 0;
}
```
