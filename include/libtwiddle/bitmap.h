#ifndef LIBTWIDDLE_BITMAP_H
#define LIBTWIDDLE_BITMAP_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <libtwiddle/internal/utils.h>

/**
 * We define the bitmap to be a 64 bits block.
 */
#define bitmap_t uint64_t
#define BYTES_PER_BITMAP sizeof(bitmap_t)

/**
 * Computes the number of required `bitmap_t` to hold `nbits` bits.
 */
#define BITMAP_PER_BITS(nbits)   DIV_ROUND_UP(nbits, BYTES_PER_BITMAP)

/**
 * struct tw_bitmap_info - bitmap miscellaneous information
 * @size:  storage capacity in bits
 * @count: number of active bits
 */
struct tw_bitmap_info {
  uint32_t size;
  uint32_t count;
};

#define tw_bitmap_info_init(nbits) (struct tw_bitmap_info) {.size = nbits, .count = 0U}
#define tw_bitmap_info_empty(info) (info.count == 0U)
#define tw_bitmap_info_full(info) (info.size == info.count)

/**
 * struct tw_bitmap - bitmap data structure
 * @info: bitmap info header
 * @data: buffer holding the bits
 */
struct tw_bitmap {
  struct tw_bitmap_info info;
  bitmap_t data[];
};

/**
 * tw_bitmap_new() - allocates a bitmap
 * @nbits: number of bits the bitmap should hold
 *
 * Return: NULL if allocation failed, otherwise a pointer to the newly
 *         allocated `struct tw_bitmap`.
 */
struct tw_bitmap *
tw_bitmap_new(uint32_t nbits);

/**
 * tw_bitmap_free() - free a bitmap
 * @bitmap: pointer to bitmap to free
 */
void
tw_bitmap_free(struct tw_bitmap *bitmap);

/**
 * tw_bitmap_set() - set position in bitmap
 * @bitmap: bitmap affected
 * @pos:    the position of the bit to set
 */
void
tw_bitmap_set(struct tw_bitmap *bitmap, uint32_t pos);

/**
 * tw_bitmap_clear() - clear position in bitmap
 * @bitmap: bitmap affected
 * @pos:    the position of the bit to clear
 */
void
tw_bitmap_clear(struct tw_bitmap *bitmap, uint32_t pos);

/**
 * tw_bitmap_test() - test postition in bitmap
 * @bitmap: bitmap targetted
 * @pos:    the position of the bit to test
 *
 * Return: the value of the bitmap
 */
bool
tw_bitmap_test(struct tw_bitmap *bitmap, uint32_t pos);

/**
 * tw_bitmap_test_and_set() - test position in bitmap and set afterward
 * @bitmap: bitmap affected
 * @pos:    the position of the bit to test and clear
 *
 * Return: the value of the position in the bitmap before setting it.
 */
bool
tw_bitmap_test_and_set(struct tw_bitmap *bitmap, uint32_t pos);

/**
 * tw_bitmap_test_and_clear() - test position in bitmap and clear afterward
 * @bitmap: bitmap affected
 * @pos:    the position of the bit to test and set
 *
 * Return: the value of the position in the bitmap before clearing it.
 */
bool
tw_bitmap_test_and_clear(struct tw_bitmap *bitmap, uint32_t pos);

/**
 * tw_bitmap_empty() - verify if bitmap is empty
 * @bitmap: bitmap to verify
 *
 * Return: true if the bitmap is empty, false otherwise.
 */
bool
tw_bitmap_empty(struct tw_bitmap *bitmap);

/**
 * tw_bitmap_full() - verify if bitmap is full
 * @bitmap: bitmap to verify
 *
 * Return: true if the bitmap is full, false otherwise.
 */
bool
tw_bitmap_full(struct tw_bitmap *bitmap);

/**
 * tw_bitmap_zero() - clear all bits in a bitmap
 * @bitmap: bitmap to empty
 */
void
tw_bitmap_zero(struct tw_bitmap *bitmap);

/**
 * tw_bitmap_zero() - set all bits in a bitmap
 * @bitmap: bitmap to fill
 */
void
tw_bitmap_fill(struct tw_bitmap *bitmap);

#endif /* LIBTWIDDLE_BITMAP_H */
