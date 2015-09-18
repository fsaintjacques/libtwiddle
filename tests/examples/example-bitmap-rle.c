#include <assert.h>
#include <libtwiddle/bitmap_rle.h>

int main(int argc, char* argv[]) {
  /** let's allocate a bitmap containing 2 billions bits */
  const uint32_t nbits = 1UL << 31;
  struct tw_bitmap_rle* bitmap = tw_bitmap_rle_new(nbits);

  assert(bitmap);

  /** let's fill 1 billion bits */
  const uint32_t start = 0UL;
  const uint32_t end = 1UL << 30;
  tw_bitmap_rle_set_range(bitmap, start, end);

  assert(tw_bitmap_rle_test(bitmap, start));
  assert(tw_bitmap_rle_test(bitmap, end));
  assert(tw_bitmap_rle_find_first_bit(bitmap)  == start);
  assert(tw_bitmap_rle_find_first_zero(bitmap) == end + 1);

  return 0;
}
