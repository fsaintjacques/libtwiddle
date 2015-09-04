#ifndef LIBTWIDDLE_BITMAP_CONST_H
#define LIBTWIDDLE_BITMAP_CONST_H

#include <libtwiddle/bitmap.h>

#define BYTES_PER_CACHELINE      64
#define BITMAPS_PER_CACHELINE    (BYTES_PER_CACHELINE / sizeof(bitmap_t))
#define BITMAPS_PER_BITMAP_CONST (BITMAPS_PER_CACHELINE - sizeof(struct tw_bitmap_info))
#define BITS_PER_BITMAP_CONST    (BITMAPS_PER_BITMAP_CONST * BITS_PER_BITMAP)

/* tw_bitmap_const should be used when the bitmap is smaller then a
 * cacheline, i.e. usually 64 bytes.
 */
struct tw_bitmap_const {
  struct tw_bitmap_info info;
  bitmap_t data[BITMAPS_PER_BITMAP_CONST];
};

#define tw_bitmap_const_init(nbits) (struct tw_bitmap_const) {.info = tw_bitmap_info_init(nbits)};

static inline void
tw_bitmap_const_zero(struct tw_bitmap_const *bitmap)
{
  for (int i=0; i < BITMAPS_PER_BITMAP_CONST / 8; ++i)
    bitmap->data[i] = 0UL;
}

static inline void
tw_bitmap_const_fill(struct tw_bitmap_const *bitmap)
{
  for (int i=0; i < BITMAPS_PER_BITMAP_CONST / 8; ++i)
    bitmap->data[i] = ~0UL;
}

#endif /* LIBTWIDDLE_BITMAP_CONST_H */
