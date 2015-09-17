#ifndef LIBTWIDDLE_BITMAP_RLE_H
#define LIBTWIDDLE_BITMAP_RLE_H

#include <assert.h>

#include <libtwiddle/bitmap.h>

/**
 * Run Length Encoding (RLE) bitmaps are compressed bitmaps. Depending on the
 * density of actives bits, it can compress considerably.
 */

struct tw_bitmap_rle_word {
  uint32_t pos;
  uint32_t count;
};

#define TW_BITMAP_RLE_WORD_PER_CACHELINE \
  (TW_CACHELINE / sizeof(struct tw_bitmap_rle_word))

/**
 * struct tw_bitmap_rle - rle-bitmap data structure
 * @info:       bitmap info header
 * @last_pos:   position of the last set bit
 * @cur_word:   index of the current bitmap_t in @data
 * @alloc_size: allocated size (in bitmap_t) of @data
 * @data:       buffer holding the bits
 */
struct tw_bitmap_rle {
  struct tw_bitmap_info info;

  uint32_t last_pos;
  uint32_t cur_word;
  uint32_t alloc_word;

  struct tw_bitmap_rle_word *data;
};

static inline
struct tw_bitmap_rle_word *
tw_bitmap_rle_get_next_word(struct tw_bitmap_rle *bitmap)
{
  if (bitmap->alloc_word == bitmap->cur_word) {
    const uint32_t alloc_word = bitmap->alloc_word * 2;
    const uint32_t alloc_size = alloc_word * sizeof(struct tw_bitmap_rle_word);
    struct tw_bitmap_rle_word *new_word =
      realloc(bitmap->data,
              alloc_size);
    assert(new_word);
    bitmap->data = new_word;
    bitmap->alloc_word = alloc_word;
  }

  return &(bitmap->data[++(bitmap->cur_word)]);
}


struct tw_bitmap_rle *
tw_bitmap_rle_new(uint32_t nbits);

void
tw_bitmap_rle_free(struct tw_bitmap_rle *bitmap);

struct tw_bitmap_rle *
tw_bitmap_rle_copy(const struct tw_bitmap_rle *src, struct tw_bitmap_rle *dst);

void
tw_bitmap_rle_set(struct tw_bitmap_rle *bitmap, uint32_t pos);

bool
tw_bitmap_rle_test(const struct tw_bitmap_rle *bitmap, uint32_t pos);

#endif /* LIBTWIDDLE_BITMAP_RLE_H */
