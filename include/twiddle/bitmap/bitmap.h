#ifndef TWIDDLE_BITMAP_H
#define TWIDDLE_BITMAP_H

#include <stdbool.h>
#include <stdint.h>

#define TW_BITMAP_MAX_BITS (1UL << 48)
#define TW_BITMAP_MAX_POS (TW_BITMAP_MAX_BITS - 1)

/**
 * struct tw_bitmap - bitmap data structure
 * @size:  storage capacity in bits
 * @count: number of active bits
 * @data:  buffer holding the bits
 *
 * This is the most basic implementation of a bitmap. It does not support
 * resizing and concurrent operations (unless constrained to reads only).
 *
 * There's a small overhead when setting/clearing bit to maintain the
 * number of active bits. This comes with a O(1) tw_bitmap_count and derived
 * operations.
 */
struct tw_bitmap {
  uint64_t size;
  uint64_t count;
  uint64_t *data;
};

/**
 * tw_bitmap_new() - allocates a bitmap
 * @size: number of bits the bitmap should hold
 *
 * Bitmaps are static and do not grow in size. Bitmaps cannot contain more
 * than TW_BITMAP_MAX_BITS.
 *
 * Return: NULL if allocation failed, otherwise a pointer to the newly
 *         allocated `struct tw_bitmap`.
 */
struct tw_bitmap *tw_bitmap_new(uint64_t size);

/**
 * tw_bitmap_free() - free a bitmap
 * @bitmap: bitmap to free
 */
void tw_bitmap_free(struct tw_bitmap *bitmap);

/**
 * tw_bitmap_copy() - copy src bitmap into dst
 * @src: bitmap to copy from
 * @dst: bitmap to copy to
 *
 * `src' and `dst' must be non-null and of equal size.
 *
 * Return: NULL if copy failed, otherwise a pointer to dst.
 */
struct tw_bitmap *tw_bitmap_copy(const struct tw_bitmap *src,
                                 struct tw_bitmap *dst);

/**
 * tw_bitmap_clone() - clone a bitmap into a new allocated bitmap
 * @bitmap: bitmap to clone
 *
 * `bitmap' must be non-null.
 *
 * Return: NULL if failed, otherwise a newly allocated bitmap initialized from
 * the requests bitmap. The caller is responsible to deallocate the bitmap
 * with tw_bitmap_free.
 */
struct tw_bitmap *tw_bitmap_clone(const struct tw_bitmap *bitmap);

/**
 * tw_bitmap_set() - set position in bitmap
 * @bitmap: bitmap affected
 * @pos:    position of the bit to set
 *
 * `bitmap' must be non-null and `pos' must be smaller than `bitmap.size'.
 */
void tw_bitmap_set(struct tw_bitmap *bitmap, uint64_t pos);

/**
 * tw_bitmap_clear() - clear position in bitmap
 * @bitmap: bitmap affected
 * @pos:    position of the bit to clear
 *
 * `bitmap' must be non-null and `pos' must be smaller than `bitmap.size'.
 */
void tw_bitmap_clear(struct tw_bitmap *bitmap, uint64_t pos);

/**
 * tw_bitmap_test() - test postition in bitmap
 * @bitmap: bitmap targetted
 * @pos:    position of the bit to test
 *
 * `bitmap' must be non-null and `pos' must be smaller than `bitmap.size'.
 *
 * Return: false if pre-conditions are not met, otherwise return the value
 * pos in the bitmap
 */
bool tw_bitmap_test(const struct tw_bitmap *bitmap, uint64_t pos);

/**
 * tw_bitmap_test_and_set() - test position in bitmap and set afterward
 * @bitmap: bitmap affected
 * @pos:    position of the bit to test and set
 *
 * `bitmap' must be non-null and `pos' must be smaller than `bitmap.size'.
 *
 * Return: false if pre-conditions are not met, otherwise return the value
 * pos in the bitmap before setting it.
 */
bool tw_bitmap_test_and_set(struct tw_bitmap *bitmap, uint64_t pos);

/**
 * tw_bitmap_test_and_clear() - test position in bitmap and clear afterward
 * @bitmap: bitmap affected
 * @pos:    position of the bit to test and clear
 *
 * `bitmap' must be non-null and `pos' must be smaller than `bitmap.size'.
 *
 * Return: false if pre-conditions are not met, otherwise return the value
 * pos in the bitmap before clearing it.
 */
bool tw_bitmap_test_and_clear(struct tw_bitmap *bitmap, uint64_t pos);

/**
 * tw_bitmap_empty() - verify if bitmap is empty
 * @bitmap: bitmap to verify
 *
 * `bitmap' must be non-null.
 *
 * Return: false if pre-conditions are not met, otherwise indicator if the
 * bitmap is empty.
 */
bool tw_bitmap_empty(const struct tw_bitmap *bitmap);

/**
 * tw_bitmap_full() - verify if bitmap is full
 * @bitmap: bitmap to verify
 *
 * `bitmap' must be non-null.
 *
 * Return: false if pre-conditions are not met, otherwise indicator if the
 * bitmap is full.
 */
bool tw_bitmap_full(const struct tw_bitmap *bitmap);

/**
 * tw_bitmap_count() - count the number of active bits
 * @bitmap: bitmap to count
 *
 * `bitmap' must be non-null.
 *
 * Return: 0 if pre-conditions are not met, otherwise number of active bits.
 */
uint64_t tw_bitmap_count(const struct tw_bitmap *bitmap);

/**
 * tw_bitmap_density() - count the percentage of active bits
 * @bitmap: bitmap to count the density
 *
 * `bitmap' must be non-null.
 *
 * Return: 0.0 if pre-conditions are not met, otherwise the portion of active
 * bits (count / size)
 */
float tw_bitmap_density(const struct tw_bitmap *bitmap);

/**
 * tw_bitmap_zero() - clear all bits in a bitmap
 * @bitmap: bitmap to empty
 *
 * `bitmap' must be non-null.
 *
 * Return: NULL if pre-conditions are not met, otherwise `bitmap' with zeroed
 * bits.
 */
struct tw_bitmap *tw_bitmap_zero(struct tw_bitmap *bitmap);

/**
 * tw_bitmap_fill() - set all bits in a bitmap
 * @bitmap: bitmap to fill
 *
 * `bitmap' must be non-null.
 *
 * Return: NULL if pre-conditions are not met, otherwise `bitmap' with filled
 * bits.
 */
struct tw_bitmap *tw_bitmap_fill(struct tw_bitmap *bitmap);

/**
 * tw_bitmap_find_first_zero() - find the first zero
 * @bitmap: bitmap to find first zero
 *
 * `bitmap' must be non-null.
 *
 * Return: -1 if not found or pre-conditions not met, otherwise the position
 * of the first zero.
 */
int64_t tw_bitmap_find_first_zero(const struct tw_bitmap *bitmap);

/**
 * tw_bitmap_find_first_bit() - find the first bit
 * @bitmap: bitmap to find first bit
 *
 * `bitmap' must be non-null.
 *
 * Return: -1 if not found or pre-conditions are not met, otherwise position
 * of the first bit.
 */
int64_t tw_bitmap_find_first_bit(const struct tw_bitmap *bitmap);

/**
 * tw_bitmap_not() - inverse all bits and zeroes in the bitmap
 * @bitmap: bitmap to inverse
 *
 * `bitmap' must be non-null.
 *
 * Return: NULL if failed, pointer to bitmap otherwise.
 */
struct tw_bitmap *tw_bitmap_not(struct tw_bitmap *bitmap);

/**
 * tw_bitmap_equal() - verify if bitmaps are equal
 * @fst: first bitmap to check
 * @snd: second bitmap to check
 *
 * `fst' and `snd' must be non-null and of same size.
 *
 * Return: false if pre-conditions are not met or bitmaps are not equal,
 * otherwise returns true.
 */
bool tw_bitmap_equal(const struct tw_bitmap *fst, const struct tw_bitmap *snd);

/**
 * tw_bitmap_union() - compute the union of src and dst into dst
 * @src: source bitmap to union
 * @dst: destionation bitmap to union
 *
 * `src' and `dst' must be non-null and of same size.
 *
 * Return: NULL if pre-conditions are not met, otherwise pointer to dst.
 */
struct tw_bitmap *tw_bitmap_union(const struct tw_bitmap *src,
                                  struct tw_bitmap *dst);

/**
 * tw_bitmap_intersection() - compute the intersection of src and dst into dst
 * @src: source bitmap to intersect
 * @dst: destionation bitmap to intersect
 *
 * `src' and `dst' must be non-null and of same size.
 *
 * Return: NULL if pre-conditions are not met, otherwise pointer to dst.
 */
struct tw_bitmap *tw_bitmap_intersection(const struct tw_bitmap *src,
                                         struct tw_bitmap *dst);

/**
 * tw_bitmap_xor() - compute the symetric difference of src and dst into dst
 * @src: source bitmap to xor
 * @dst: destionation bitmap to xor
 *
 * `src' and `dst' must be non-null and of same size.
 *
 * Return: NULL if pre-conditions are not met, otherwise pointer to dst.
 */
struct tw_bitmap *tw_bitmap_xor(const struct tw_bitmap *src,
                                struct tw_bitmap *dst);

#endif /* TWIDDLE_BITMAP_H */
