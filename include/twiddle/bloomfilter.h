#ifndef TWIDDLE_BLOOMFILTER_H
#define TWIDDLE_BLOOMFILTER_H

#include <stdbool.h>
#include <stdint.h>

#include <twiddle/bitmap.h>

/**
 * struct tw_bloomfilter_info - bloomfilter header
 * @size: number of bits the bloomfilter holds
 * @k:    number of hash functions used
 */
struct tw_bloomfilter_info {
  uint32_t size;
  uint32_t k;
  uint32_t hash_seed;
};

#define tw_bloomfilter_info_init(s,k,h) \
  (struct tw_bloomfilter_info) {.size = s, .k = k, .hash_seed = h}
#define tw_bloomfilter_info_copy(src, dst) \
  dst = (struct tw_bloomfilter_info) {.size = src.size, .k = src.k, .hash_seed = src.hash_seed}
#define tw_bloomfilter_info_equal(src, dst) \
  (src.size == dst.size && src.k == dst.k && src.hash_seed == dst.hash_seed)

#define TW_BF_DEFAULT_SEED 3781869495U

/**
 * struct tw_bloomfilter - bloomfilter
 * @info:      header information
 * @hash_seed: seed used in hashing function
 * @bitmap:    bitmap holding the bits
 *
 * This bloomfilter is static and does not support automatic resizing. The
 * underlaying storage is struct tw_bitmap.
 */
struct tw_bloomfilter {
  struct tw_bloomfilter_info info;
  struct tw_bitmap *bitmap;
};

/**
 * tw_bloomfilter_new() - allocates a bloomfilter
 * @size: number of bits the bloomfilter should hold
 * @k:    number of hash functions used
 *
 * Return: NULL if allocation failed, otherwise a pointer to the newly
 *         allocated `struct tw_bloomfilter`.
 */
struct tw_bloomfilter *
tw_bloomfilter_new(uint32_t size, uint32_t k);

/**
 * tw_bloomfilter_free() - free a bloomfilter
 * @bf: bloomfilter to free
 */
void
tw_bloomfilter_free(struct tw_bloomfilter *bf);

/**
 * tw_bloomfilter_copy() - copy src bloomfilter into dst
 * @src: bloomfilter to copy from
 * @dst: bloomfilter to copy to
 *
 * Size of bloomfilter must be equals.
 *
 * Return: NULL if copy failed, otherwise a pointer to dst.
 */
struct tw_bloomfilter *
tw_bloomfilter_copy(const struct tw_bloomfilter *src,
                    struct tw_bloomfilter *dst);

/**
 * tw_bloomfilter_clone() - clone a bloomfilter into a newly allocated one
 * @bf: bloomfilter to clone
 *
 * Return: NULL if failed, otherwise a newly allocated bloomfilter initialized
 * from the requested bloomfilter. The caller is responsible to deallocate
 * with tw_bloomfilter_free.
 */
struct tw_bloomfilter *
tw_bloomfilter_clone(const struct tw_bloomfilter *bf);

/**
 * tw_bloomfilter_set() - set an element in a bloomfilter
 * @bf:   bloomfilter affected
 * @size: size of the key to add
 * @buf:  buf to the key to add
 */
void
tw_bloomfilter_set(struct tw_bloomfilter *bf,
                   size_t size, const char* buf);

/**
 * tw_bloomfilter_test() - test an element in a bloomfilter
 * @bf:   bloomfilter affected
 * @size: size of the key to test
 * @buf:  buf to the key to test
 *
 * Return: false if the element is not in the bloomfilter, true otherwise.
 */
bool
tw_bloomfilter_test(const struct tw_bloomfilter *bf,
                    size_t size, const char* buf);
/**
 * tw_bloomfilter_empty() - verify if bloomfilter is empty
 * @bf: bloomfilter to verify
 *
 * Return: true if the bloomfilter is empty, false otherwise.
 */
bool
tw_bloomfilter_empty(const struct tw_bloomfilter *bf);

/**
 * tw_bloomfilter_full() - verify if bloomfilter is full
 * @bf: bloomfilter to verify
 *
 * Return: true if the bloomfilter is full, false otherwise.
 */
bool
tw_bloomfilter_full(const struct tw_bloomfilter *bf);

/**
 * tw_bloomfilter_count() - count the number of active bits
 * @bf: bloomfilter to count
 *
 * Return: number of active bits
 */
uint32_t
tw_bloomfilter_count(const struct tw_bloomfilter *bf);

/**
 * tw_bloomfilter_density() - count the percentage of active bits
 * @bf: bloomfilter to count the density
 *
 * Return: the portion of active bits (count / size)
 */
float
tw_bloomfilter_density(const struct tw_bloomfilter *bf);

/**
 * tw_bloomfilter_zero() - clear all bits in a bloomfilter
 * @bf: bloomfilter to empty
 *
 * Return: the bloomfilter cleared
 */
struct tw_bloomfilter *
tw_bloomfilter_zero(struct tw_bloomfilter *bf);

/**
 * tw_bloomfilter_fill() - set all bits in a bloomfilter
 * @bf: bloomfilter to fill
 *
 * Return: the bloomfilter filled
 */
struct tw_bloomfilter *
tw_bloomfilter_fill(struct tw_bloomfilter *bf);

/**
 * tw_bloomfilter_not() - inverse all bits and zeroes in the bloomfilter
 * @bf: bloomfilter to inverse
 *
 * Return: NULL if failed, otherwise the bloomfilter.
 */
struct tw_bloomfilter *
tw_bloomfilter_not(struct tw_bloomfilter *bf);

/**
 * tw_bloomfilter_equal() - verify if bloomfilter are equal
 * @a: first bloomfilter to check
 * @b: second bloomfilter to check
 *
 * Return: true if equal, false otherwise
 *
 * In order to be comparable, bloomfilters must have the same size and the
 * same number of hash functions (k).
 */
bool
tw_bloomfilter_equal(const struct tw_bloomfilter *a,
                     const struct tw_bloomfilter *b);

/**
 * tw_bloomfilter_union() - computer the union of bloomfilters
 * @src: source bloomfilter to union
 * @dst: destionation bloomfilter to union
 *
 * Return: NULL if failed, otherwise pointer to dst.
 *
 * Only works on bloomfilter of the same size and same number of hash
 * functions(k). This store the results in dst.
 */
struct tw_bloomfilter *
tw_bloomfilter_union(const struct tw_bloomfilter *src,
                           struct tw_bloomfilter *dst);

/**
 * tw_bloomfilter_intersection() - compute the intersection of bloomfilters
 * @src: source bloomfilter to intersection
 * @dst: destionation bloomfilter to intersection
 *
 * Return: NULL if failed, otherwise pointer to dst.
 *
 * Only works on bloomfilter of the same size and same number of hash
 * functions(k). This store the results in dst.
 */
struct tw_bloomfilter *
tw_bloomfilter_intersection(const struct tw_bloomfilter *src,
                                  struct tw_bloomfilter *dst);

/**
 * tw_bloomfilter_xor() - compute the symetric difference of bloomfilters
 * @src: source bloomfilter to xor
 * @dst: destionation bloomfilter to xor
 *
 * Return: NULL if failed, otherwise pointer to dst.
 *
 * Only works on bloomfilter of the same size and same number of hash
 * functions(k). This store the results in dst.
 */
struct tw_bloomfilter *
tw_bloomfilter_xor(const struct tw_bloomfilter *src,
                         struct tw_bloomfilter *dst);

#endif /* TWIDDLE_BLOOMFILTER_H */
