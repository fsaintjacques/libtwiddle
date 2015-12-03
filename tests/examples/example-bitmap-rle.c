#include <assert.h>
#include <twiddle/bitmap/bitmap_rle.h>

int main(int argc, char *argv[])
{
  /** allocate a bitmap containing 2 billions bits */
  const uint64_t nbits = 1UL << 31;
  struct tw_bitmap_rle *bitmap = tw_bitmap_rle_new(nbits);

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
  assert(tw_bitmap_rle_find_first_bit(bitmap) == start);
  assert(tw_bitmap_rle_find_first_zero(bitmap) == end + 1);

  return 0;
}
