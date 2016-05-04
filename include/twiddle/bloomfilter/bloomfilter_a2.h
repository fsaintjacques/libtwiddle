#ifndef TWIDDLE_BLOOMFILTER_A2_H
#define TWIDDLE_BLOOMFILTER_A2_H

#include <stdbool.h>
#include <stdint.h>

struct tw_bloomfilter;

/**
 * struct tw_bloomfilter_a2 - aging bloom filter with active buffers
 * @density: density threshold to trigger rotation
 * @active:  pointer to active bloomfilter
 * @passive: pointer to passive bloomfilter
 *
 * The paper "Aging bloom filter with two active buffers for dynamic sets"
 * describe a method where 2 bloom filters are used to implement a FIFO.
 *
 * Elements are added to `active` until `density` (on active).
 * Once this happen, `passive` is cleared and both filters are
 * swapped.
 */
struct tw_bloomfilter_a2 {
  float density;

  struct tw_bloomfilter *active;
  struct tw_bloomfilter *passive;
};

/**
 * tw_bloomfilter_a2_new() - allocates a bloomfilter
 * @size:    number of bits the bloomfilter should hold
 * @k:       number of hash functions used
 * @density: threshold for rotation (between in (0, 1])
 *
 * Return: NULL if allocation failed, otherwise a pointer to the newly
 *         allocated `struct tw_bloomfilter_a2`.
 */
struct tw_bloomfilter_a2 *tw_bloomfilter_a2_new(uint64_t size, uint16_t k,
                                                float dentisy);

/**
 * tw_bloomfilter_a2_free() - free a bloomfilter
 * @bf: bloomfilter to free
 */
void tw_bloomfilter_a2_free(struct tw_bloomfilter_a2 *bf);

/**
 * tw_bloomfilter_a2_copy() - copy src bloomfilter into dst
 * @src: bloomfilter to copy from
 * @dst: bloomfilter to copy to
 *
 * Size of bloomfilter must be equals.
 *
 * Return: NULL if copy failed, otherwise a pointer to dst.
 */
struct tw_bloomfilter_a2 *
tw_bloomfilter_a2_copy(const struct tw_bloomfilter_a2 *src,
                       struct tw_bloomfilter_a2 *dst);

/**
 * tw_bloomfilter_a2_clone() - clone a bloomfilter into a newly allocated one
 * @bf: bloomfilter to clone
 *
 * Return: NULL if failed, otherwise a newly allocated bloomfilter initialized
 * from the requested bloomfilter. The caller is responsible to deallocate
 * with tw_bloomfilter_a2_free.
 */
struct tw_bloomfilter_a2 *
tw_bloomfilter_a2_clone(const struct tw_bloomfilter_a2 *bf);

/**
 * tw_bloomfilter_a2_set() - set an element in a bloomfilter
 * @bf:       bloomfilter affected
 * @key:      buffer of the key to add
 * @key_size: size of the buffer of the key to add
 */
void tw_bloomfilter_a2_set(struct tw_bloomfilter_a2 *bf, const void *key,
                           size_t key_size);

/**
 * tw_bloomfilter_a2_test() - test an element in a bloomfilter
 * @bf:   bloomfilter affected
 * @key:      buffer of the key to test
 * @key_size: size of the buffer of the key to test
 *
 * Return: false if the element is not in the bloomfilter, true otherwise.
 */
bool tw_bloomfilter_a2_test(const struct tw_bloomfilter_a2 *bf, const void *key,
                            size_t key_size);

/**
 * tw_bloomfilter_a2_empty() - verify if bloomfilter is empty
 * @bf: bloomfilter to verify
 *
 * Return: true if the bloomfilter is empty, false otherwise.
 */
bool tw_bloomfilter_a2_empty(const struct tw_bloomfilter_a2 *bf);

/**
 * tw_bloomfilter_a2_full() - verify if bloomfilter is full
 * @bf: bloomfilter to verify
 *
 * Return: true if both bloomfilters are full, false otherwise.
 */
bool tw_bloomfilter_a2_full(const struct tw_bloomfilter_a2 *bf);

/**
 * tw_bloomfilter_a2_count() - count the number of active bits
 * @bf: bloomfilter to count
 *
 * Return: number of active bits in both bloomfilters
 */
uint64_t tw_bloomfilter_a2_count(const struct tw_bloomfilter_a2 *bf);

/**
 * tw_bloomfilter_a2_density() - count the percentage of active bits
 * @bf: bloomfilter to count the density
 *
 * Return: the portion of active bits (count / size)
 */
float tw_bloomfilter_a2_density(const struct tw_bloomfilter_a2 *bf);

/**
 * tw_bloomfilter_a2_zero() - clear all bits in a bloomfilter
 * @bf: bloomfilter to empty
 *
 * Return: the bloomfilter cleared
 */
struct tw_bloomfilter_a2 *tw_bloomfilter_a2_zero(struct tw_bloomfilter_a2 *bf);

/**
 * tw_bloomfilter_a2_fill() - set all bits in a bloomfilter
 * @bf: bloomfilter to fill
 *
 * Return: the bloomfilter filled
 */
struct tw_bloomfilter_a2 *tw_bloomfilter_a2_fill(struct tw_bloomfilter_a2 *bf);

/**
 * tw_bloomfilter_a2_not() - inverse all bits and zeroes in the bloomfilter
 * @bf: bloomfilter to inverse
 *
 * Return: NULL if failed, otherwise the bloomfilter.
 */
struct tw_bloomfilter_a2 *tw_bloomfilter_a2_not(struct tw_bloomfilter_a2 *bf);

/**
 * tw_bloomfilter_a2_equal() - verify if bloomfilters are equal
 * @a: first bloomfilter to check
 * @b: second bloomfilter to check
 *
 * Return: true if equal, false otherwise
 *
 * In order to be comparable, bloomfilters must have the same size and the
 * same number of hash functions (k).
 */
bool tw_bloomfilter_a2_equal(const struct tw_bloomfilter_a2 *a,
                             const struct tw_bloomfilter_a2 *b);

/**
 * tw_bloomfilter_a2_union() - computer the union of bloomfilters
 * @src: source bloomfilter to union
 * @dst: destionation bloomfilter to union
 *
 * Return: NULL if failed, otherwise pointer to dst.
 *
 * Only works on bloomfilter of the same size and same number of hash
 * functions(k). This store the results in dst.
 */
struct tw_bloomfilter_a2 *
tw_bloomfilter_a2_union(const struct tw_bloomfilter_a2 *src,
                        struct tw_bloomfilter_a2 *dst);

/**
 * tw_bloomfilter_a2_intersection() - compute the intersection of bloomfilters
 * @src: source bloomfilter to intersection
 * @dst: destionation bloomfilter to intersection
 *
 * Return: NULL if failed, otherwise pointer to dst.
 *
 * Only works on bloomfilter of the same size and same number of hash
 * functions(k). This store the results in dst.
 */
struct tw_bloomfilter_a2 *
tw_bloomfilter_a2_intersection(const struct tw_bloomfilter_a2 *src,
                               struct tw_bloomfilter_a2 *dst);

/**
 * tw_bloomfilter_a2_xor() - compute the symetric difference of bloomfilters
 * @src: source bloomfilter to xor
 * @dst: destionation bloomfilter to xor
 *
 * Return: NULL if failed, otherwise pointer to dst.
 *
 * Only works on bloomfilter of the same size and same number of hash
 * functions(k). This store the results in dst.
 */
struct tw_bloomfilter_a2 *
tw_bloomfilter_a2_xor(const struct tw_bloomfilter_a2 *src,
                      struct tw_bloomfilter_a2 *dst);

#endif /* TWIDDLE_BLOOMFILTER_A2_H */
