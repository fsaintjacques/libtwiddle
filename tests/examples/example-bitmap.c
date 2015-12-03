#include <assert.h>
#include <twiddle/bitmap/bitmap.h>

int main(int argc, char *argv[])
{
  const uint64_t nbits = 1024;
  struct tw_bitmap *bitmap = tw_bitmap_new(nbits);

  assert(bitmap);

  tw_bitmap_set(bitmap, 512);
  assert(tw_bitmap_test_and_clear(bitmap, 512));
  assert(!tw_bitmap_test(bitmap, 512));

  tw_bitmap_set(bitmap, 768);
  assert(tw_bitmap_find_first_bit(bitmap) == 768);

  return 0;
}
