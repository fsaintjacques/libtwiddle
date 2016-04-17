#ifndef TWIDDLE_BITMAP_H
#define TWIDDLE_BITMAP_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <twiddle/internal/utils.h>

/**
 * We define the bitmap to be a 64 bits block.
 */
#define bitmap_t uint64_t
#define TW_BYTES_PER_BITMAP sizeof(bitmap_t)
#define TW_BITS_PER_BITMAP (TW_BYTES_PER_BITMAP * TW_BITS_IN_WORD)

#define BITMAP_POS(pos) (pos / TW_BITS_PER_BITMAP)
#define MASK(pos) (1ULL << (pos % TW_BITS_PER_BITMAP))

/**
 * Computes the number of required `bitmap_t` to hold `nbits` bits.
 */
#define TW_BITMAP_PER_BITS(nbits) TW_DIV_ROUND_UP(nbits, TW_BITS_PER_BITMAP)
#define TW_BITMAP_POS(nbits) (nbits / TW_BITS_PER_BITMAP)

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
  bitmap_t *data;
};

/**
 * tw_bitmap_new() - allocates a bitmap
 * @size: number of bits the bitmap should hold
 *
 * Bitmaps are static and do not grow in size. Bitmaps cannot contain more
 * than TW_BITMAP_MAX_POS.
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
 * Size of bitmap must be equals.
 *
 * Return: NULL if copy failed, otherwise a pointer to dst.
 */
struct tw_bitmap *tw_bitmap_copy(const struct tw_bitmap *src,
                                 struct tw_bitmap *dst);

/**
 * tw_bitmap_clone() - clone a bitmap into a new allocated bitmap
 * @bitmap: bitmap to clone
 *
 * Return: NULL if failed, otherwise a newly allocated bitmap initialized from
 * the requests bitmap. The caller is responsible to deallocated the bitmap
 * with tw_bitmap_free.
 */
struct tw_bitmap *tw_bitmap_clone(const struct tw_bitmap *bitmap);

/**
 * tw_bitmap_set() - set position in bitmap
 * @bitmap: bitmap affected
 * @pos:    position of the bit to set
 */
void tw_bitmap_set(struct tw_bitmap *bitmap, uint64_t pos);

/**
 * tw_bitmap_clear() - clear position in bitmap
 * @bitmap: bitmap affected
 * @pos:    position of the bit to clear
 */
void tw_bitmap_clear(struct tw_bitmap *bitmap, uint64_t pos);

/**
 * tw_bitmap_test() - test postition in bitmap
 * @bitmap: bitmap targetted
 * @pos:    position of the bit to test
 *
 * Return: value pos in the bitmap
 */
bool tw_bitmap_test(const struct tw_bitmap *bitmap, uint64_t pos);

/**
 * tw_bitmap_test_and_set() - test position in bitmap and set afterward
 * @bitmap: bitmap affected
 * @pos:    position of the bit to test and set
 *
 * Return: value of the position in the bitmap before setting it.
 */
bool tw_bitmap_test_and_set(struct tw_bitmap *bitmap, uint64_t pos);

/**
 * tw_bitmap_test_and_clear() - test position in bitmap and clear afterward
 * @bitmap: bitmap affected
 * @pos:    position of the bit to test and clear
 *
 * Return: value of the position in the bitmap before clearing it.
 */
bool tw_bitmap_test_and_clear(struct tw_bitmap *bitmap, uint64_t pos);

/**
 * tw_bitmap_empty() - verify if bitmap is empty
 * @bitmap: bitmap to verify
 *
 * Return: indicator if the bitmap is empty.
 */
bool tw_bitmap_empty(const struct tw_bitmap *bitmap);

/**
 * tw_bitmap_full() - verify if bitmap is full
 * @bitmap: bitmap to verify
 *
 * Return: indicator if the bitmap is full.
 */
bool tw_bitmap_full(const struct tw_bitmap *bitmap);

/**
 * tw_bitmap_count() - count the number of active bits
 * @bitmap: bitmap to count
 *
 * Return: number of active bits
 */
uint64_t tw_bitmap_count(const struct tw_bitmap *bitmap);

/**
 * tw_bitmap_density() - count the percentage of active bits
 * @bitmap: bitmap to count the density
 *
 * Return: the portion of active bits (count / size)
 */
float tw_bitmap_density(const struct tw_bitmap *bitmap);

/**
 * tw_bitmap_zero() - clear all bits in a bitmap
 * @bitmap: bitmap to empty
 *
 * Return: the bitmap
 */
struct tw_bitmap *tw_bitmap_zero(struct tw_bitmap *bitmap);

/**
 * tw_bitmap_fill() - set all bits in a bitmap
 * @bitmap: bitmap to fill
 *
 * Return: the bitmap
 */
struct tw_bitmap *tw_bitmap_fill(struct tw_bitmap *bitmap);

/**
 * tw_bitmap_find_first_zero() - find the first zero
 * @bitmap: bitmap to find first zero
 *
 * Return: -1 if not found, otherwise the bit position.
 */
int64_t tw_bitmap_find_first_zero(const struct tw_bitmap *bitmap);

/**
 * tw_bitmap_find_first_bit() - find the first bit
 * @bitmap: bitmap to find first bit
 *
 * Return: -1 if not found, otherwise the bit position.
 */
int64_t tw_bitmap_find_first_bit(const struct tw_bitmap *bitmap);

/**
 * tw_bitmap_not() - inverse all bits and zeroes in the bitmap
 * @bitmap: bitmap to inverse
 *
 * Return: NULL if failed, pointer to bitmap otherwise.
 */
struct tw_bitmap *tw_bitmap_not(struct tw_bitmap *bitmap);

/**
 * tw_bitmap_equal() - verify if bitmaps are equal
 * @a: first bitmap to check
 * @b: second bitmap to check
 *
 * Return: true if a and b are equal, false otherwise
 */
bool tw_bitmap_equal(const struct tw_bitmap *a, const struct tw_bitmap *b);

/**
 * tw_bitmap_union() - compute the union of src and dst into dst
 * @src: source bitmap to union
 * @dst: destionation bitmap to union
 *
 * Return: NULL if failed, otherwise pointer to dst.
 *
 * Only works on bitmap of the same size.
 */
struct tw_bitmap *tw_bitmap_union(const struct tw_bitmap *src,
                                  struct tw_bitmap *dst);

/**
 * tw_bitmap_intersection() - compute the intersection of src and dst into dst
 * @src: source bitmap to intersect
 * @dst: destionation bitmap to intersect
 *
 * Return: NULL if failed, otherwise pointer to dst.
 *
 * Only works on bitmap of the same size.
 */
struct tw_bitmap *tw_bitmap_intersection(const struct tw_bitmap *src,
                                         struct tw_bitmap *dst);

/**
 * tw_bitmap_xor() - compute the symetric difference of src and dst into dst
 * @src: source bitmap to xor
 * @dst: destionation bitmap to xor
 *
 * Return: NULL if failed, otherwise pointer to dst.
 *
 * Only works on bitmap of the same size.
 */
struct tw_bitmap *tw_bitmap_xor(const struct tw_bitmap *src,
                                struct tw_bitmap *dst);

#endif /* TWIDDLE_BITMAP_H */
