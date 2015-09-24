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
#define tw_bitmap_rle_word_equal(a, b) \
  (a.pos == b.pos && a.count == b.count)

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

/**
 * tw_bitmap_rle_new() - allocates a bitmap
 * @size: number of bits the bitmap should hold
 *
 * Bitmaps are static and do not grow in size.
 *
 * Return: NULL if allocation failed, otherwise a pointer to the newly
 *         allocated `struct tw_bitmap_rle`.
 */
struct tw_bitmap_rle *
tw_bitmap_rle_new(uint32_t nbits);

/**
 * tw_bitmap_rle_free() - free a bitmap
 * @bitmap: bitmap to free
 */
void
tw_bitmap_rle_free(struct tw_bitmap_rle *bitmap);

/**
 * tw_bitmap_rle_copy() - copy src bitmap into dst
 * @src: bitmap to copy from
 * @dst: bitmap to copy to
 *
 * Size of bitmap must be equals.
 *
 * Return: NULL if copy failed, otherwise a pointer to dst.
 */
struct tw_bitmap_rle *
tw_bitmap_rle_copy(const struct tw_bitmap_rle *src, struct tw_bitmap_rle *dst);

/**
 * tw_bitmap_rle_clone() - clone a bitmap into a new allocated bitmap
 * @bitmap: bitmap to clone
 *
 * Return: NULL if failed, otherwise a newly allocated bitmap initialized from
 * the requests bitmap. The caller is responsible to deallocated the bitmap
 * with tw_bitmap_rle_free.
 */
struct tw_bitmap_rle *
tw_bitmap_rle_clone(const struct tw_bitmap_rle *bitmap);

/**
 * tw_bitmap_rle_set() - set position in bitmap
 * @bitmap: bitmap affected
 * @pos:    position of the bit to set
 *
 * This function must always be called with a monotone increasing pos
 * argument.
 */
void
tw_bitmap_rle_set(struct tw_bitmap_rle *bitmap, uint32_t pos);

/**
 * tw_bitmap_rle_set_word() - set bitmap_rle_word in bitmap
 * @bitmap: bitmap affected
 * @word:   range of bits in the form (start_pos, count) to set in the bitmap
 *
 * This is a helper internal-ish function to add ranges in one go instead of
 * sequentially calling tw_bitmap_rle_set.
 */
void
tw_bitmap_rle_set_word(struct tw_bitmap_rle *bitmap,
                       const struct tw_bitmap_rle_word *word);

/**
 * tw_bitmap_rle_set_range() - set range in bitmap
 * @bitmap: bitmap affected
 * @word:   range of bits in the form (start_pos, end_pos) to set in the bitmap
 *
 * This is a helper internal-ish function to add ranges in one go instead of
 * sequentially calling tw_bitmap_rle_set.
 */
void
tw_bitmap_rle_set_range(struct tw_bitmap_rle *bitmap,
                        uint32_t start,
                        uint32_t end);

/**
 * tw_bitmap_rle_test() - test postition in bitmap
 * @bitmap: bitmap targetted
 * @pos:    position of the bit to test
 *
 * Return: value of pos in the bitmap
 */
bool
tw_bitmap_rle_test(const struct tw_bitmap_rle *bitmap, uint32_t pos);

/**
 * tw_bitmap_rle_empty() - verify if bitmap is empty
 * @bitmap: bitmap to verify
 *
 * Return: indicator if the bitmap is empty
 */
bool
tw_bitmap_rle_empty(const struct tw_bitmap_rle *bitmap);

/**
 * tw_bitmap_rle_full() - verify if bitmap is full
 * @bitmap: bitmap to verify
 *
 * Return: indicator if the bitmap is full.
 */
bool
tw_bitmap_rle_full(const struct tw_bitmap_rle *bitmap);

/**
 * tw_bitmap_rle_count() - count the number of active bits
 * @bitmap: bitmap to count
 *
 * Return: number of active bits
 */
uint32_t
tw_bitmap_rle_count(const struct tw_bitmap_rle *bitmap);

/**
 * tw_bitmap_rle_density() - count the percentage of active bits
 * @bitmap: bitmap to count the density
 *
 * Return: the portion of active bits (count / size)
 */
float
tw_bitmap_rle_density(const struct tw_bitmap_rle *bitmap);

/**
 * tw_bitmap_rle_zero() - clear all bits in a bitmap
 * @bitmap: bitmap to empty
 *
 * Return: the bitmap
 */
struct tw_bitmap_rle *
tw_bitmap_rle_zero(struct tw_bitmap_rle *bitmap);

/**
 * tw_bitmap_rle_fill() - set all bits in a bitmap
 * @bitmap: bitmap to fill
 *
 * Return: the bitmap
 */
struct tw_bitmap_rle *
tw_bitmap_rle_fill(struct tw_bitmap_rle *bitmap);

/**
 * tw_bitmap_rle_find_first_zero() - find the first zero
 * @bitmap: bitmap to find first zero
 *
 * Return: -1 if not found, otherwise the bit position.
 */
int64_t
tw_bitmap_rle_find_first_zero(const struct tw_bitmap_rle *bitmap);

/**
 * tw_bitmap_rle_find_first_bit() - find the first bit
 * @bitmap: bitmap to find first bit
 *
 * Return: -1 if not found, otherwise the bit position.
 */
int64_t
tw_bitmap_rle_find_first_bit(const struct tw_bitmap_rle *bitmap);

struct tw_bitmap_rle *
tw_bitmap_rle_not(const struct tw_bitmap_rle *bitmap,
                  struct tw_bitmap_rle *dst);

bool
tw_bitmap_rle_equal(const struct tw_bitmap_rle *a,
                    const struct tw_bitmap_rle *b);

#endif /* LIBTWIDDLE_BITMAP_RLE_H */
