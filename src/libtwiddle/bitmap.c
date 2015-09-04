#include <assert.h>
#include <stdlib.h>
#include <libtwiddle/bitmap.h>

struct tw_bitmap *
tw_bitmap_new(uint32_t nbits)
{
  assert(nbits <= UINT32_MAX);
  struct tw_bitmap *bitmap = calloc(1,
                                    sizeof(struct tw_bitmap_info) +
                                      BITMAP_PER_BITS(nbits) * BYTES_PER_BITMAP);
  bitmap->info = tw_bitmap_info_init(nbits);
  return bitmap;
}

void
tw_bitmap_free(struct tw_bitmap *bitmap)
{
  free(bitmap);
}

void
tw_bitmap_set(struct tw_bitmap *bitmap, uint32_t pos)
{
  tw_bitmap_test_and_set(bitmap, pos);
}

void
tw_bitmap_clear(struct tw_bitmap *bitmap, uint32_t pos)
{
  tw_bitmap_test_and_clear(bitmap, pos);
}

bool
tw_bitmap_test(struct tw_bitmap *bitmap, uint32_t pos)
{
  assert(bitmap && pos <= bitmap->info.size);
  return !!(bitmap->data[BYTE_POS(pos)] & MASK(pos));
}

bool
tw_bitmap_test_and_set(struct tw_bitmap *bitmap, uint32_t pos)
{
  assert(bitmap && pos <= bitmap->info.size);
  char *addr = (char *) &(bitmap->data[BYTE_POS(pos)]);
  bool prev = (*addr) & MASK(pos);

  if(!prev)
    bitmap->info.count++;

  *addr |= MASK(pos);

  return prev;
}

bool
tw_bitmap_test_and_clear(struct tw_bitmap *bitmap, uint32_t pos)
{
  assert(bitmap && pos <= bitmap->info.size);
  char *addr = (char *) &(bitmap->data[BYTE_POS(pos)]);
  bool prev = (*addr) & MASK(pos);

  if(prev)
    bitmap->info.count--;

  *addr &= ~MASK(pos);

  return prev;
}

bool
tw_bitmap_empty(struct tw_bitmap *bitmap)
{
  assert(bitmap);

  return tw_bitmap_info_empty(bitmap->info);
}

bool
tw_bitmap_full(struct tw_bitmap *bitmap)
{
  assert(bitmap);

  return tw_bitmap_info_full(bitmap->info);
}


void
tw_bitmap_zero(struct tw_bitmap *bitmap)
{
  assert(bitmap);
  for (size_t i = 0; i < BITMAP_PER_BITS(bitmap->info.size); ++i)
    bitmap->data[i] = 0UL;

  bitmap->info.count = 0U;
}

void
tw_bitmap_fill(struct tw_bitmap *bitmap)
{
  assert(bitmap);
  for (size_t i = 0; i < BITMAP_PER_BITS(bitmap->info.size); ++i)
    bitmap->data[i] = 0UL;

  bitmap->info.count = bitmap->info.size;
}

