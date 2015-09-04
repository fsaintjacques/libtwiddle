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
 * @size:     storage capacity in bits
 * @count:    number of active bits
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
 * @info:
 * @data:
 */
struct tw_bitmap {
  struct tw_bitmap_info info;
  bitmap_t data[];
};

/**
 * tw_bitmap_new() - allocates a `struct tw_bitmap`
 * @nbits: number of bits the bitmap should hold.
 *
 *
 * Return:
 */
struct tw_bitmap *
tw_bitmap_new(uint32_t nbits);

/**
 * tw_bitmap_free - free a `tw_bitmap`
 * @bitmap: a pointer to the bitmap to free
 */
void
tw_bitmap_free(struct tw_bitmap *bitmap);

/**
 * tw_bitmap_set - activate bit at `pos` in `tw_bitmap`
 * @bitmap: a pointer to the bitmap to free
 * @pos:
 */
void
tw_bitmap_set(struct tw_bitmap *bitmap, uint32_t pos);

void
tw_bitmap_clear(struct tw_bitmap *bitmap, uint32_t pos);

bool
tw_bitmap_test(struct tw_bitmap *bitmap, uint32_t pos);

bool
tw_bitmap_test_and_set(struct tw_bitmap *bitmap, uint32_t pos);

bool
tw_bitmap_test_and_clear(struct tw_bitmap *bitmap, uint32_t pos);


bool
tw_bitmap_empty(struct tw_bitmap *bitmap);

bool
tw_bitmap_full(struct tw_bitmap *bitmap);


void
tw_bitmap_zero(struct tw_bitmap *bitmap);

void
tw_bitmap_fill(struct tw_bitmap *bitmap);

#endif /* LIBTWIDDLE_BITMAP_H */
