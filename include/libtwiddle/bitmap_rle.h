#ifndef LIBTWIDDLE_BITMAP_RLE_H
#define LIBTWIDDLE_BITMAP_RLE_H

#include <assert.h>

#include <libtwiddle/bitmap.h>

/**
 * Run Length Encoding (RLE) bitmaps are compressed bitmaps. Depending on the
 * density of actives bits, it can compress considerably. This implementation
 * is semi mutable as you can only add increasing positions.
 */

struct tw_bitmap_rle_word {
  uint32_t pos;
  uint32_t count;
};

#define TW_BITMAP_RLE_WORD_PER_CACHELINE \
  (TW_CACHELINE / sizeof(struct tw_bitmap_rle_word))

#define tw_bitmap_rle_word_zero \
  (struct tw_bitmap_rle_word) {.pos = 0, .count = 0}
#define tw_bitmap_rle_word_full(nbits) \
  (struct tw_bitmap_rle_word) {.pos = 0, .count = nbits}

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

struct tw_bitmap_rle *
tw_bitmap_rle_new(uint32_t nbits);

void
tw_bitmap_rle_free(struct tw_bitmap_rle *bitmap);

struct tw_bitmap_rle *
tw_bitmap_rle_copy(const struct tw_bitmap_rle *src, struct tw_bitmap_rle *dst);

struct tw_bitmap_rle *
tw_bitmap_rle_clone(const struct tw_bitmap_rle *bitmap);

void
tw_bitmap_rle_set(struct tw_bitmap_rle *bitmap, uint32_t pos);

void
tw_bitmap_rle_set_word(struct tw_bitmap_rle *bitmap,
                       const struct tw_bitmap_rle_word *word);

void
tw_bitmap_rle_set_range(struct tw_bitmap_rle *bitmap,
                        uint32_t start,
                        uint32_t end);

bool
tw_bitmap_rle_test(const struct tw_bitmap_rle *bitmap, uint32_t pos);

bool
tw_bitmap_rle_empty(const struct tw_bitmap_rle *bitmap);

bool
tw_bitmap_rle_full(const struct tw_bitmap_rle *bitmap);

uint32_t
tw_bitmap_rle_count(const struct tw_bitmap_rle *bitmap);

float
tw_bitmap_rle_density(const struct tw_bitmap_rle *bitmap);

struct tw_bitmap_rle *
tw_bitmap_rle_zero(struct tw_bitmap_rle *bitmap);

struct tw_bitmap_rle *
tw_bitmap_rle_fill(struct tw_bitmap_rle *bitmap);

#endif /* LIBTWIDDLE_BITMAP_RLE_H */
