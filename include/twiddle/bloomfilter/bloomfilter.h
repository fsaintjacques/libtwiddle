#ifndef TWIDDLE_BLOOMFILTER_H
#define TWIDDLE_BLOOMFILTER_H

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define TW_LOG_2 0.6931471805599453

#define tw_bloomfilter_optimal_m(n, p) (-n * log(p) / (TW_LOG_2 * TW_LOG_2))
#define tw_bloomfilter_optimal_k(n, m) (m / n * TW_LOG_2)

struct tw_bitmap;

/**
 * struct tw_bloomfilter - bloomfilter
 * @k:      number of hash functions
 * @bitmap: bitmap holding the bits
 *
 * This bloomfilter is static and does not support automatic resizing. The
 * underlaying storage is struct tw_bitmap.
 */
struct tw_bloomfilter {
  uint16_t k;
  struct tw_bitmap *bitmap;
};

/**
 * tw_bloomfilter_new() - allocates a bloomfilter
 * @size: number of bits the bloomfilter should hold
 * @k:    number of hash functions used
 *
 * `k' must be greater than 0, `size' must be within (0, TW_BITMAP_MAX_BITS].
 *
 * Return: NULL if allocation failed, otherwise a pointer to the newly
 *         allocated `struct tw_bloomfilter`.
 */
struct tw_bloomfilter *tw_bloomfilter_new(uint64_t size, uint16_t k);

/**
 * tw_bloomfilter_free() - free a bloomfilter
 * @bf: bloomfilter to free
 */
void tw_bloomfilter_free(struct tw_bloomfilter *bf);

/**
 * tw_bloomfilter_copy() - copy src bloomfilter into dst
 * @src: bloomfilter to copy from
 * @dst: bloomfilter to copy to
 *
 * `src' and `dst' must be non-null, and of equal size.
 *
 * Return: NULL if copy failed, otherwise a pointer to dst.
 */
struct tw_bloomfilter *tw_bloomfilter_copy(const struct tw_bloomfilter *src,
                                           struct tw_bloomfilter *dst);

/**
 * tw_bloomfilter_clone() - clone a bloomfilter into a newly allocated one
 * @bf: bloomfilter to clone
 *
 * `bf' must be non-null.
 *
 * Return: NULL if failed, otherwise a newly allocated bloomfilter initialized
 * from the requested bloomfilter. The caller is responsible to deallocate
 * with tw_bloomfilter_free.
 */
struct tw_bloomfilter *tw_bloomfilter_clone(const struct tw_bloomfilter *bf);

/**
 * tw_bloomfilter_set() - set an element in a bloomfilter
 * @bf:       bloomfilter affected
 * @key:      buffer of the key to add
 * @key_size: size of the buffer key to add
 *
 * `bf' and `key' must be non-null, `key_size' must be greater than 0.
 */
void tw_bloomfilter_set(struct tw_bloomfilter *bf, const void *key,
                        size_t key_size);

/**
 * tw_bloomfilter_test() - test an element in a bloomfilter
 * @bf:       bloomfilter affected
 * @key:      buffer of the key to test
 * @key_size: size of the buffer of key to test
 *
 * `bf' and `key' must be non-null, `key_size' must be greater than 0.
 *
 * Return: false if preconditions are not met, otherwise indicator if the
 * element is in the bloomfilter (with possibility of false positives).
 */
bool tw_bloomfilter_test(const struct tw_bloomfilter *bf, const void *key,
                         size_t key_size);
/**
 * tw_bloomfilter_empty() - verify if bloomfilter is empty
 * @bf: bloomfilter to verify
 *
 * `bf' must be non-null.
 *
 * Return: false if preconditions are not met or the bloomfilter is not empty,
 * true if the bloomfilter is empty.
 */
bool tw_bloomfilter_empty(const struct tw_bloomfilter *bf);

/**
 * tw_bloomfilter_full() - verify if bloomfilter is full
 * @bf: bloomfilter to verify
 *
 * `bf' must be non-null.
 *
 * Return: false if preconditions are not met or the bloomfilter is not full,
 * true if the bloomfilter is full.
 */
bool tw_bloomfilter_full(const struct tw_bloomfilter *bf);

/**
 * tw_bloomfilter_count() - count the number of active bits
 * @bf: bloomfilter to count
 *
 * `bf' must be non-null.
 *
 * Return: number of active bits
 */
uint64_t tw_bloomfilter_count(const struct tw_bloomfilter *bf);

/**
 * tw_bloomfilter_density() - count the percentage of active bits
 * @bf: bloomfilter to count the density
 *
 * `bf' must be non-null.
 *
 * Return: the portion of active bits (count / size)
 */
float tw_bloomfilter_density(const struct tw_bloomfilter *bf);

/**
 * tw_bloomfilter_zero() - clear all bits in a bloomfilter
 * @bf: bloomfilter to empty
 *
 * `bf' must be non-null.
 *
 * Return: the bloomfilter cleared
 */
struct tw_bloomfilter *tw_bloomfilter_zero(struct tw_bloomfilter *bf);

/**
 * tw_bloomfilter_fill() - set all bits in a bloomfilter
 * @bf: bloomfilter to fill
 *
 * `bf' must be non-null.
 *
 * Return: the bloomfilter filled
 */
struct tw_bloomfilter *tw_bloomfilter_fill(struct tw_bloomfilter *bf);

/**
 * tw_bloomfilter_not() - inverse all bits and zeroes in the bloomfilter
 * @bf: bloomfilter to inverse
 *
 * `bf' must be non-null.
 *
 * Return: NULL if failed, otherwise the bloomfilter.
 */
struct tw_bloomfilter *tw_bloomfilter_not(struct tw_bloomfilter *bf);

/**
 * tw_bloomfilter_equal() - verify if bloomfilter are equal
 * @fst: first bloomfilter to check
 * @snd: second bloomfilter to check
 *
 * `fst' and `snd' must be non-null.
 *
 * Return: true if equal, false otherwise
 *
 * In order to be comparable, bloomfilters must have the same size and the
 * same number of hash functions (k).
 */
bool tw_bloomfilter_equal(const struct tw_bloomfilter *a,
                          const struct tw_bloomfilter *b);

/**
 * tw_bloomfilter_union() - computer the union of bloomfilters
 * @src: source bloomfilter to union
 * @dst: destionation bloomfilter to union
 *
 * `src' and `dst' must be non-null and compatibles.
 *
 * Return: NULL if failed, otherwise pointer to dst.
 *
 * Only works on bloomfilter of the same size and same number of hash
 * functions(k). This store the results in dst.
 */
struct tw_bloomfilter *tw_bloomfilter_union(const struct tw_bloomfilter *src,
                                            struct tw_bloomfilter *dst);

/**
 * tw_bloomfilter_intersection() - compute the intersection of bloomfilters
 * @src: source bloomfilter to intersection
 * @dst: destionation bloomfilter to intersection
 *
 * `src' and `dst' must be non-null and compatibles.
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
 * `src' and `dst' must be non-null and compatibles.
 *
 * Return: NULL if failed, otherwise pointer to dst.
 *
 * Only works on bloomfilter of the same size and same number of hash
 * functions(k). This store the results in dst.
 */
struct tw_bloomfilter *tw_bloomfilter_xor(const struct tw_bloomfilter *src,
                                          struct tw_bloomfilter *dst);

#endif /* TWIDDLE_BLOOMFILTER_H */
