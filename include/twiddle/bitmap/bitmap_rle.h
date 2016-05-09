#ifndef TWIDDLE_BITMAP_RLE_H
#define TWIDDLE_BITMAP_RLE_H

#include <stdbool.h>
#include <stdint.h>

struct tw_bitmap_rle_word {
  uint64_t pos;
  uint64_t count;
};

/**
 * run-length encoding (RLE) bitmap data structure
 *
 * RLE bitmaps are compressed bitmaps. Depending on the density of actives
 * bits, it can compress considerably. This implementation is semi mutable
 * as you can only add increasing positions.
 */
struct tw_bitmap_rle {
  /** storage capacity in bits */
  uint64_t size;
  /** number of active bits */
  uint64_t count;
  /** position of the last set bit */
  uint64_t last_pos;
  /** index of the last used `struct tw_bitmap_rle_word` in @data */
  uint64_t last_word_idx;
  /** number of allocated `struct tw_bitmap_rle_word` in @data */
  uint64_t alloc_word;
  /** buffer holding the bits */
  struct tw_bitmap_rle_word *data;
};

/**
 * Creates a `struct tw_bitmap_rle` with the requested number of bits.
 *
 * @param size number of bits the bitmap should hold, must be smaller or
 *             equal than `TW_BITMAP_MAX_BITS`.
 *
 * @return `NULL` if allocation failed, otherwise a pointer to the newly
 *         allocated `struct tw_bitmap_rle`
 *
 * @note group:bitmap_rle
 */
struct tw_bitmap_rle *tw_bitmap_rle_new(uint64_t nbits);

/**
 * Free a `struct tw_bitmap_rle`.
 *
 * @param bitmap to free
 *
 * @note group:bitmap_rle
 */
void tw_bitmap_rle_free(struct tw_bitmap_rle *bitmap);

/**
 * Copy a source bitmap into a specified bitmap.
 *
 * @param src non-null bitmap to copy from
 * @param dst non-null bitmap to copy to
 *
 * @return `NULL` if copy failed, otherwise a pointer to dst
 *
 * @note group:bitmap_rle
 */
struct tw_bitmap_rle *tw_bitmap_rle_copy(const struct tw_bitmap_rle *src,
                                         struct tw_bitmap_rle *dst);

/**
 * Clone a bitmap into a new allocated bitmap.
 *
 * @param bitmap non-null bitmap to clone
 *
 * @return `NULL` if failed, otherwise a newly allocated bitmap initialized
 *         from the requests bitmap
 *
 * @note group:bitmap_rle
 */
struct tw_bitmap_rle *tw_bitmap_rle_clone(const struct tw_bitmap_rle *bitmap);

/**
 * Set position in a `struct tw_bitmap_rle`.
 *
 * @param bitmap non-null bitmap to set the position
 * @param pos position of the bit to set, must be smaller than `bitmap.size',
 *            note that `pos` must be called in a monotone increasing fashion
 *
 * @note group:bitmap_rle
 */
void tw_bitmap_rle_set(struct tw_bitmap_rle *bitmap, uint64_t pos);

/**
 * Set a contiguous range in a `struct tw_bitmap_rle`.
 *
 * @param bitmap non-null bitmap to set the range
 * @param word range of bits in the form (start_pos, count) to set in the
 *             bitmap, `word.pos' must be smaller than `bitmap.size', and
 *             `word.pos' + `word.count' must be smaller than
 *             `TW_BITMAP_MAX_BITS`
 *
 * @note group:bitmap_rle
 */
void tw_bitmap_rle_set_word(struct tw_bitmap_rle *bitmap,
                            const struct tw_bitmap_rle_word *word);

/**
 * Set a contiguous range in a `struct tw_bitmap_rle`.
 *
 * @param bitmap non-null bitmap to set the range
 * @param start starting position to start setting bits, must be smaller than
 *              `bitmap.size'
 * @param end end position to stop setting bits, must be smaller than
 *              `bitmap.size'
 *
 * @note group:bitmap_rle
 */
void tw_bitmap_rle_set_range(struct tw_bitmap_rle *bitmap, uint64_t start,
                             uint64_t end);

/**
 * Test a position in a `struct tw_bitmap_rle`.
 *
 * @param bitmap non-null bitmap to test position at
 * @param pos position of the bit to test, must be smaller than `bitmap.size'
 *
 * @return `false` if pre-conditions are not met, otherwise return the value
 *         pos in the bitmap
 *
 * @note group:bitmap_rle
 */
bool tw_bitmap_rle_test(const struct tw_bitmap_rle *bitmap, uint64_t pos);

/**
 * Verify if a `struct tw_bitmap_rle` is empty.
 *
 * @param bitmap non-null bitmap to verify emptyness
 *
 * @return `false` if pre-conditions are not met, otherwise indicator if the
 *         bitmap is empty
 *
 * @note group:bitmap_rle
 */
bool tw_bitmap_rle_empty(const struct tw_bitmap_rle *bitmap);

/**
 * Verify if a `struct tw_bitmap_rle` is full.
 *
 * @param bitmap non-null bitmap to verify fullness
 *
 * @return `false` if pre-conditions are not met, otherwise indicator if the
 *         bitmap is full
 *
 * @note group:bitmap_rle
 */
bool tw_bitmap_rle_full(const struct tw_bitmap_rle *bitmap);

/**
 * Count the number of active bits in a `struct tw_bitmap_rle`.
 *
 * @param bitmap non-null bitmap to count the number of active bits
 *
 * @return `0` if pre-conditions are not met, otherwise the number of active
 *         bits.
 *
 * @note group:bitmap_rle
 */
uint64_t tw_bitmap_rle_count(const struct tw_bitmap_rle *bitmap);

/**
 * Count the proportion of active bits in a `struct tw_bitmap_rle`.
 *
 * @param bitmap non-null bitmap to count the density
 *
 * @return `0.0` if pre-conditions are not met, otherwise the proportion of
 *         active bits, i.e. `bitmap.count / bitmap.size`
 *
 * @note group:bitmap_rle
 */
float tw_bitmap_rle_density(const struct tw_bitmap_rle *bitmap);

/**
 * Clear all bits in a `struct tw_bitmap_rle`.
 *
 * @param bitmap non-null bitmap to clear
 *
 * @return `NULL` if pre-conditions are not met, otherwise `bitmap' with zeroed
 *         bits.
 *
 * @note group:bitmap_rle
 */
struct tw_bitmap_rle *tw_bitmap_rle_zero(struct tw_bitmap_rle *bitmap);

/**
 * Set all bits in a `struct tw_bitmap_rle`.
 *
 * @param bitmap non-null bitmap to fill
 *
 * @return `NULL` if pre-conditions are not met, otherwise `bitmap' with filled
 *         bits.
 *
 * @note group:bitmap_rle
 */
struct tw_bitmap_rle *tw_bitmap_rle_fill(struct tw_bitmap_rle *bitmap);

/**
 * Find the first zero in a `struct tw_bitmap_rle`.
 *
 * @param bitmap non-null bitmap to find first zero
 *
 * @return `-1` if not found or pre-conditions not met, otherwise the position
 *         of the first zero.
 *
 * @note group:bitmap_rle
 */
int64_t tw_bitmap_rle_find_first_zero(const struct tw_bitmap_rle *bitmap);

/**
 * Find the first bit in a `struct tw_bitmap_rle`.
 *
 * @param bitmap non-null bitmap to find first bit
 *
 * @return `-1` if not found or pre-conditions not met, otherwise the position
 *         of the first bit.
 *
 * @note group:bitmap_rle
 */
int64_t tw_bitmap_rle_find_first_bit(const struct tw_bitmap_rle *bitmap);

/**
 * Negate all bits and zeroes in a `struct tw_bitmap_rle`.
 *
 * @param bitmap non-null bitmap to negate
 *
 * @return `NULL` if failed, otherwise a pointer to `bitmap`
 *
 * @note group:bitmap_rle
 */
struct tw_bitmap_rle *tw_bitmap_rle_not(const struct tw_bitmap_rle *bitmap,
                                        struct tw_bitmap_rle *dst);

/**
 * Verify if `struct tw_bitmap_rle`s are equals.
 *
 * @param fst non-null first bitmap to check
 * @param snd non-null second bitmap to check of same size as `fst`
 *
 * @return `false` if pre-conditions are not met or bitmaps are not equal,
 *         otherwise returns `true`
 *
 * @note group:bitmap_rle
 */
bool tw_bitmap_rle_equal(const struct tw_bitmap_rle *fst,
                         const struct tw_bitmap_rle *snd);

/**
 * Compute the intersection of `struct tw_bitmap_rle`s.
 *
 * @param a non-null first operand bitmap to union
 * @param b non-null second operand bitmap to union of same size as `a`
 * @param dst non-null destination bitmap to union of same size as `a`
 *
 * @return `NULL` if pre-conditions are not met, otherwise pointer to `dst`
 *
 * @note group:bitmap_rle
 */
struct tw_bitmap_rle *tw_bitmap_rle_union(const struct tw_bitmap_rle *a,
                                          const struct tw_bitmap_rle *b,
                                          struct tw_bitmap_rle *dst);

/**
 * Compute the union of `struct tw_bitmap_rle`s.
 *
 * @param a non-null first operand bitmap to union
 * @param b non-null second operand bitmap to union
 * @param dst non-null destination bitmap to union of same size as `a`
 *
 * @return `NULL` if pre-conditions are not met, otherwise pointer to `dst`
 *
 * @note group:bitmap_rle
 */
struct tw_bitmap_rle *tw_bitmap_rle_intersection(const struct tw_bitmap_rle *a,
                                                 const struct tw_bitmap_rle *b,
                                                 struct tw_bitmap_rle *dst);

#endif /* TWIDDLE_BITMAP_RLE_H */
