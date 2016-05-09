#ifndef TWIDDLE_BITMAP_H
#define TWIDDLE_BITMAP_H

#include <stdbool.h>
#include <stdint.h>

#define TW_BITMAP_MAX_BITS (1UL << 48)
#define TW_BITMAP_MAX_POS (TW_BITMAP_MAX_BITS - 1)

/**
 * dense bitmap data structure
 *
 * This is the most basic implementation of a bitmap. It does not support
 * resizing and concurrent operations (unless constrained to reads only).
 *
 * There's a small overhead when setting/clearing bit to maintain the
 * number of active bits. This comes with a O(1) tw_bitmap_count and derived
 * operations.
 */
struct tw_bitmap {
  /** storage capacity in bits */
  uint64_t size;
  /** number of active bits */
  uint64_t count;
  /** pointer to stored bits */
  uint64_t *data;
};

/**
 * Creates a `struct tw_bitmap` with the requested number of bits.
 *
 * @param size number of bits the bitmap should hold, must be smaller or equal
 *             than `TW_BITMAP_MAX_BITS`
 *
 * @return `NULL` if allocation failed, otherwise a pointer to the newly
 *         allocated `struct tw_bitmap`
 *
 * @note group:bitmap
 */
struct tw_bitmap *tw_bitmap_new(uint64_t size);

/**
 * Free a `struct tw_bitmap`.
 *
 * @param bitmap to free
 *
 * @note group:bitmap
 */
void tw_bitmap_free(struct tw_bitmap *bitmap);

/**
 * Copy a source bitmap into a specified bitmap.
 *
 * @param src non-null bitmap to copy from
 * @param dst non-null bitmap to copy to
 *
 * @return `NULL` if copy failed, otherwise a pointer to dst
 *
 * @note group:bitmap
 */
struct tw_bitmap *tw_bitmap_copy(const struct tw_bitmap *src,
                                 struct tw_bitmap *dst);

/**
 * Clone a bitmap into a new allocated bitmap.
 *
 * @param bitmap non-null bitmap to clone
 *
 * @return `NULL` if failed, otherwise a newly allocated bitmap initialized from
 *         the requests bitmap. The caller is responsible to deallocate the
 *         bitmap with tw_bitmap_free
 *
 * @note group:bitmap
 */
struct tw_bitmap *tw_bitmap_clone(const struct tw_bitmap *bitmap);

/**
 * Set position in a `struct tw_bitmap`.
 *
 * @param bitmap non-null bitmap to set the position
 * @param pos position of the bit to set, must be smaller than `bitmap.size'
 *
 * @note group:bitmap
 */
void tw_bitmap_set(struct tw_bitmap *bitmap, uint64_t pos);

/**
 * Clear position in a `struct tw_bitmap`.
 *
 * @param bitmap bitmap non-null bitmap to clear the position
 * @param pos position of the bit to clear, must be smaller than `bitmap.size'
 *
 * @note group:bitmap
 */
void tw_bitmap_clear(struct tw_bitmap *bitmap, uint64_t pos);

/**
 * Test a position in a `struct tw_bitmap`.
 *
 * @param bitmap non-null bitmap to test position at
 * @param pos position of the bit to test, must be smaller than `bitmap.size'
 *
 * @return `false` if pre-conditions are not met, otherwise return the value
 *         pos in the bitmap
 *
 * @note group:bitmap
 */
bool tw_bitmap_test(const struct tw_bitmap *bitmap, uint64_t pos);

/**
 * Test a position in `struct tw_bitmap` and set the position afterward.
 *
 * @param bitmap non-null bitmap to test and set potition at
 * @param pos position of the bit to test and set
 *
 * @return `false` if pre-conditions are not met, otherwise return the value
 *         pos in the bitmap before setting it
 *
 * @note group:bitmap
 */
bool tw_bitmap_test_and_set(struct tw_bitmap *bitmap, uint64_t pos);

/**
 * Test a position in `struct tw_bitmap` and clear the position afterward.
 *
 * @param bitmap non-null bitmap to test and clear potition at
 * @param pos position of the bit to test and clear, must be smaller than
 *            `bitmap.size'
 *
 * @return `false` if pre-conditions are not met, otherwise return the value
 *         pos in the bitmap before clearing it
 *
 * @note group:bitmap
 */
bool tw_bitmap_test_and_clear(struct tw_bitmap *bitmap, uint64_t pos);

/**
 * Verify if a `struct tw_bitmap` is empty.
 *
 * @param bitmap non-null bitmap to verify emptyness
 *
 * @return `false` if pre-conditions are not met, otherwise indicator if the
 *         bitmap is empty
 *
 * @note group:bitmap
 */
bool tw_bitmap_empty(const struct tw_bitmap *bitmap);

/**
 * Verify if a `struct tw_bitmap` is full.
 *
 * @param bitmap non-null bitmap to verify fullness
 *
 * @return `false` if pre-conditions are not met, otherwise indicator if the
 *         bitmap is full
 *
 * @note group:bitmap
 */
bool tw_bitmap_full(const struct tw_bitmap *bitmap);

/**
 * Count the number of active bits in a `struct tw_bitmap`.
 *
 * @param bitmap non-null bitmap to count the number of active bits
 *
 * @return `0` if pre-conditions are not met, otherwise the number of active
 *         bits.
 *
 * @note group:bitmap
 */
uint64_t tw_bitmap_count(const struct tw_bitmap *bitmap);

/**
 * Count the proportion of active bits in a `struct tw_bitmap`.
 *
 * @param bitmap non-null bitmap to count the density
 *
 * @return `0.0` if pre-conditions are not met, otherwise the proportion of
 *         active bits, i.e. `bitmap.count / bitmap.size`
 *
 * @note group:bitmap
 */
float tw_bitmap_density(const struct tw_bitmap *bitmap);

/**
 * Clear all bits in a `struct tw_bitmap`.
 *
 * @param bitmap non-null bitmap to clear
 *
 * @return `NULL` if pre-conditions are not met, otherwise `bitmap' with zeroed
 *         bits.
 *
 * @note group:bitmap
 */
struct tw_bitmap *tw_bitmap_zero(struct tw_bitmap *bitmap);

/**
 * Set all bits in a `struct tw_bitmap`.
 *
 * @param bitmap non-null bitmap to fill
 *
 * @return `NULL` if pre-conditions are not met, otherwise `bitmap' with filled
 *         bits.
 *
 * @note group:bitmap
 */
struct tw_bitmap *tw_bitmap_fill(struct tw_bitmap *bitmap);

/**
 * Find the first zero in a `struct tw_bitmap`.
 *
 * @param bitmap non-null bitmap to find first zero
 *
 * @return `-1` if not found or pre-conditions not met, otherwise the position
 *         of the first zero.
 *
 * @note group:bitmap
 */
int64_t tw_bitmap_find_first_zero(const struct tw_bitmap *bitmap);

/**
 * Find the first bit in a `struct tw_bitmap`.
 *
 * @param bitmap non-null bitmap to find first bit
 *
 * @return `-1` if not found or pre-conditions not met, otherwise the position
 *         of the first bit.
 *
 * @note group:bitmap
 */
int64_t tw_bitmap_find_first_bit(const struct tw_bitmap *bitmap);

/**
 * Negate all bits and zeroes in a `struct tw_bitmap`.
 *
 * @param bitmap non-null bitmap to negate
 *
 * @return `NULL` if failed, otherwise a pointer to `bitmap`
 *
 * @note group:bitmap
 */
struct tw_bitmap *tw_bitmap_not(struct tw_bitmap *bitmap);

/**
 * Verify if `struct tw_bitmap`s are equals.
 *
 * @param fst non-null first bitmap to check
 * @param snd non-null second bitmap to check of same size as `fst`
 *
 * @return `false` if pre-conditions are not met or bitmaps are not equal,
 *         otherwise returns `true`
 *
 * @note group:bitmap
 */
bool tw_bitmap_equal(const struct tw_bitmap *fst, const struct tw_bitmap *snd);

/**
 * Compute the in-place union of `struct tw_bitmap`s.
 *
 * @param src non-null source bitmap to union
 * @param dst non-null destination bitmap to union of same size as `src`
 *
 * @return `NULL` if pre-conditions are not met, otherwise pointer to `dst`
 *
 * @note group:bitmap
 */
struct tw_bitmap *tw_bitmap_union(const struct tw_bitmap *src,
                                  struct tw_bitmap *dst);

/**
 * Compute the in-place intersection of `struct tw_bitmap`s.
 *
 * @param src non-null source bitmap to union
 * @param dst non-null destination bitmap to intersect of same size as `src`
 *
 * @return `NULL` if pre-conditions are not met, otherwise pointer to `dst`
 *
 * @note group:bitmap
 */
struct tw_bitmap *tw_bitmap_intersection(const struct tw_bitmap *src,
                                         struct tw_bitmap *dst);

/**
 * Compute the in-place xor of `struct tw_bitmap`s.
 *
 * @param src non-null source bitmap to union
 * @param dst non-null destination bitmap to xor of same size as `src`
 *
 * @return `NULL` if pre-conditions are not met, otherwise pointer to `dst`
 *
 * @note group:bitmap
 */
struct tw_bitmap *tw_bitmap_xor(const struct tw_bitmap *src,
                                struct tw_bitmap *dst);

#endif /* TWIDDLE_BITMAP_H */
