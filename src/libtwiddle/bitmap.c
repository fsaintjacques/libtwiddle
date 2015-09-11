#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <libtwiddle/bitmap.h>
#include <libtwiddle/internal/bitops.h>

static __always_inline
void
tw_bitmap_clear_extra_bits(struct tw_bitmap* bitmap)
{
  int32_t rest = bitmap->info.size % TW_BITS_PER_BITMAP;
  if (rest) {
    bitmap->data[bitmap->info.size / TW_BITS_PER_BITMAP] ^= ~0UL << rest;
  }
}

struct tw_bitmap *
tw_bitmap_new(uint32_t nbits)
{
  const size_t alloc_size = sizeof(struct tw_bitmap_info) +
                            TW_BITMAP_PER_BITS(nbits) * TW_BYTES_PER_BITMAP;
  struct tw_bitmap *bitmap = calloc(1, alloc_size);
  bitmap->info = tw_bitmap_info_init(nbits);
  return bitmap;
}

void
tw_bitmap_free(struct tw_bitmap *bitmap)
{
  free(bitmap);
}

struct tw_bitmap *
tw_bitmap_copy(const struct tw_bitmap *src, struct tw_bitmap *dst)
{
  assert(src && dst);

  /**
   * When bitmaps size are not equal, memory fragmentation becomes a problem if
   * one consecutively copy from a decreasing size bitmap, e.g.:
   *
   *   a.size == 32, b.size == 32, c.size == 31;
   *
   *   assert(tw_bitmap_copy(a, b) == b);  // ok
   *   assert(tw_bitmap_copy(c, b) == b);  // ok
   *   assert(tw_bitmap_copy(a, b) == b);  // fails because b.size is now 31
   *
   * No memory leaks are involved since calls to tw_bitmap_free don't depends
   * on bitmap->info.size;
   */
  if (dst->info.size != src->info.size)
    return NULL;

  tw_bitmap_info_copy(src->info, dst->info);
  memcpy(dst->data, src->data,
         TW_BITMAP_PER_BITS(src->info.size) * TW_BYTES_PER_BITMAP);

  return dst;
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

float
tw_bitmap_density(const struct tw_bitmap *bitmap)
{
  assert(bitmap);
  return tw_bitmap_info_density(bitmap->info);
}

struct tw_bitmap *
tw_bitmap_zero(struct tw_bitmap *bitmap)
{
  assert(bitmap);
  for (size_t i = 0; i < TW_BITMAP_PER_BITS(bitmap->info.size); ++i)
    bitmap->data[i] = 0UL;

  bitmap->info.count = 0U;

  return bitmap;
}

struct tw_bitmap *
tw_bitmap_fill(struct tw_bitmap *bitmap)
{
  assert(bitmap);
  for (size_t i = 0; i < TW_BITMAP_PER_BITS(bitmap->info.size); ++i)
    bitmap->data[i] = ~0UL;

  tw_bitmap_clear_extra_bits(bitmap);

  bitmap->info.count = bitmap->info.size;

  return bitmap;
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

struct tw_bitmap *
tw_bitmap_not(struct tw_bitmap *bitmap)
{
  assert(bitmap);

  for (size_t i = 0; i < TW_BITMAP_PER_BITS(bitmap->info.size); ++i)
    bitmap->data[i] ^= ~0UL;

  tw_bitmap_clear_extra_bits(bitmap);

  bitmap->info.count = bitmap->info.size - bitmap->info.count;

  return bitmap;
}

bool
tw_bitmap_equal(const struct tw_bitmap *a, const struct tw_bitmap *b)
{
  assert(a && b);

  if (a->info.size != b->info.size || a->info.count != b->info.count)
    return false;

  for (size_t i = 0; i < TW_BITMAP_PER_BITS(a->info.size); ++i)
    if (a->data[i] != b->data[i])
      return false;

  return true;
}

struct tw_bitmap *
tw_bitmap_union(const struct tw_bitmap *src, struct tw_bitmap *dst)
{
  assert(src && dst);

  if (src->info.size != dst->info.size)
    return NULL;

  uint32_t count = 0;
  for (size_t i = 0; i < TW_BITMAP_PER_BITS(src->info.size); ++i) {
    dst->data[i] |= src->data[i];
    count += tw_popcountl(dst->data[i]);
  }

  dst->info.count = count;

  return dst;
}

struct tw_bitmap *
tw_bitmap_intersection(const struct tw_bitmap *src, struct tw_bitmap *dst)
{
  assert(src && dst);

  if (src->info.size != dst->info.size)
    return NULL;

  uint32_t count = 0;
  for (size_t i = 0; i < TW_BITMAP_PER_BITS(src->info.size); ++i) {
    dst->data[i] &= src->data[i];
    count += tw_popcountl(dst->data[i]);
  }

  dst->info.count = count;

  return dst;
}

struct tw_bitmap *
tw_bitmap_xor(const struct tw_bitmap *src, struct tw_bitmap *dst)
{
  assert(src && dst);

  if (src->info.size != dst->info.size)
    return NULL;

  uint32_t count = 0;
  for (size_t i = 0; i < TW_BITMAP_PER_BITS(src->info.size); ++i) {
    dst->data[i] ^= src->data[i];
    count += tw_popcountl(dst->data[i]);
  }

  dst->info.count = count;

  return dst;
}
