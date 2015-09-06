#include <assert.h>
#include <stdlib.h>
#include <libtwiddle/bitmap.h>
#include <libtwiddle/internal/bitops.h>

struct tw_bitmap *
tw_bitmap_new(uint32_t nbits)
{
  assert(nbits <= UINT32_MAX);
  struct tw_bitmap *bitmap = calloc(1,
                                    sizeof(struct tw_bitmap_info) +
                                      TW_BITMAP_PER_BITS(nbits) * TW_BYTES_PER_BITMAP);
  bitmap->info = tw_bitmap_info_init(nbits);
  return bitmap;
}

void
tw_bitmap_free(struct tw_bitmap *bitmap)
{
  free(bitmap);
}

void __always_inline
tw_bitmap_set(struct tw_bitmap *bitmap, uint32_t pos)
{
  tw_bitmap_test_and_set(bitmap, pos);
}

void __always_inline
tw_bitmap_clear(struct tw_bitmap *bitmap, uint32_t pos)
{
  tw_bitmap_test_and_clear(bitmap, pos);
}

bool
tw_bitmap_test(const struct tw_bitmap *bitmap, uint32_t pos)
{
  assert(bitmap && pos < bitmap->info.size);
  return variable_test_bit(pos % 64, &(bitmap->data[TW_BITMAP_POS(pos)]));
}

bool
tw_bitmap_test_and_set(struct tw_bitmap *bitmap, uint32_t pos)
{
  assert(bitmap && pos < bitmap->info.size);
  bool val = __test_and_set_bit(pos % 64, &(bitmap->data[TW_BITMAP_POS(pos)]));
  if(!val)
    bitmap->info.count++;
  return val;
}

bool
tw_bitmap_test_and_clear(struct tw_bitmap *bitmap, uint32_t pos)
{
  assert(bitmap && pos < bitmap->info.size);
  bool val = __test_and_clear_bit(pos % 64, &(bitmap->data[TW_BITMAP_POS(pos)]));
  if(val)
    bitmap->info.count--;
  return val;
}

bool
tw_bitmap_empty(const struct tw_bitmap *bitmap)
{
  assert(bitmap);
  return tw_bitmap_info_empty(bitmap->info);
}

bool
tw_bitmap_full(const struct tw_bitmap *bitmap)
{
  assert(bitmap);
  return tw_bitmap_info_full(bitmap->info);
}

uint32_t
tw_bitmap_count(const struct tw_bitmap *bitmap)
{
  assert(bitmap);
  return tw_bitmap_info_count(bitmap->info);
}

void
tw_bitmap_zero(struct tw_bitmap *bitmap)
{
  assert(bitmap);
  for (size_t i = 0; i < TW_BITMAP_PER_BITS(bitmap->info.size); ++i)
    bitmap->data[i] = 0UL;

  bitmap->info.count = 0U;
}

void
tw_bitmap_fill(struct tw_bitmap *bitmap)
{
  assert(bitmap);
  for (size_t i = 0; i < TW_BITMAP_PER_BITS(bitmap->info.size); ++i)
    bitmap->data[i] = ~0UL;

  bitmap->info.count = bitmap->info.size;
}

int64_t
tw_bitmap_find_first_zero(const struct tw_bitmap *bitmap)
{
  assert(bitmap);

  /**
   * This check is required since we allocate memory on multiple of bitmap_t.
   * Thus if bitmap.size is not aligned on 64 bits, a full bitmap
   * would incorrectly report nbits+1 as the position of the first zero.
   */
  if (tw_unlikely(tw_bitmap_full(bitmap)))
    return -1;

  for (size_t i = 0; i < TW_BITMAP_PER_BITS(bitmap->info.size); ++i) {
    const int pos = tw_ffzl(bitmap->data[i]);
    if(pos)
      return (i * TW_BITS_PER_BITMAP) + (pos-1);
  }

  return -1;
}

int64_t
tw_bitmap_find_first_bit(const struct tw_bitmap *bitmap)
{
  assert(bitmap);

  /**
   * This check is required since we allocate memory on multiple of bitmap_t
   * Thus if bitmap.size is not aligned on 64 bits, a full bitmap
   * filled with tw_bitmap_fill and then cleared manually with tw_bitmap_clear,
   * could report the first bit to be greater than nbits.
   */
  if (tw_unlikely(tw_bitmap_empty(bitmap)))
    return -1;

  for (size_t i = 0; i < TW_BITMAP_PER_BITS(bitmap->info.size); ++i) {
    const uint64_t pos = tw_ffsl(bitmap->data[i]);
    if(pos)
      return (i * TW_BITS_PER_BITMAP) + (pos-1);
  }

  return -1;
}
