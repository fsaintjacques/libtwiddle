#ifndef TWIDDLE_BLOOMFILTER_A2_H
#define TWIDDLE_BLOOMFILTER_A2_H

#include <stdbool.h>
#include <stdint.h>

struct tw_bloomfilter;

/**
 * Aging bloomfilter with active buffers data structure
 *
 * The paper "Aging bloom filter with two active buffers for dynamic sets"
 * describe a method where 2 bloom filters are used to implement a FIFO.
 *
 * Elements are added to `active` until `density` (on active) is attained;
 * then `passive` is cleared and both filters are swapped.
 */
struct tw_bloomfilter_a2 {
  /** density threshold to trigger rotation */
  float density;
  /** pointer to active bloomfilter */
  struct tw_bloomfilter *active;
  /** pointer to passive bloomfilter */
  struct tw_bloomfilter *passive;
};

/**
 * Allocate a `struct tw_bloomfilter_a2`.
 *
 * @param size number of bits the bloomfilter should hold, between
 *             (0, TW_BITMAP_MAX_BITS].
 * @param k stricly positive number of hash functions used
 * @param density threshold for rotation within (0, 1]
 *
 * @return `NULL` if allocation failed, otherwise a pointer to the newly
 *         allocated `struct tw_bloomfilter`
 *
 * @note group:bloomfilter_a2
 */
struct tw_bloomfilter_a2 *tw_bloomfilter_a2_new(uint64_t size, uint16_t k,
                                                float dentisy);

/**
 * Free a `struct tw_bloomfilter_a2`.
 *
 * @param bf bloomfilter to free
 *
 * @note group:bloomfilter_a2
 */
void tw_bloomfilter_a2_free(struct tw_bloomfilter_a2 *bf);

/**
 * Copy a source `struct tw_bloomfilter_a2` into a specified destination.
 *
 * @param src non-null bloomfilter to copy from
 * @param dst non-null bloomfilter to copy to
 *
 * @return `NULL` if any filter is null or not of the same cardinality,
 *         otherwise a pointer to dst
 *
 * @note group:bloomfilter_a2
 */
struct tw_bloomfilter_a2 *
tw_bloomfilter_a2_copy(const struct tw_bloomfilter_a2 *src,
                       struct tw_bloomfilter_a2 *dst);

/**
 * Clone a `struct tw_bloomfilter_a2` into a newly allocated one.
 *
 * @param bf non-null bloomfilter to clone
 *
 * @return `NULL` if failed, otherwise a newly allocated bloomfilter initialized
 *         from the requested bloomfilter. The caller is responsible to
 *         deallocate with tw_bloomfilter_a2_free
 *
 * @note group:bloomfilter_a2
 */
struct tw_bloomfilter_a2 *
tw_bloomfilter_a2_clone(const struct tw_bloomfilter_a2 *bf);

/**
 * Set an element in a `struct tw_bloomfilter_a2`.
 *
 * @param bf non-null bloomfilter affected
 * @param key non-null buffer of the key to add
 * @param key_size stricly positive size of the buffer key to add
 *
 * @note group:bloomfilter_a2
 */
void tw_bloomfilter_a2_set(struct tw_bloomfilter_a2 *bf, const void *key,
                           size_t key_size);

/**
 * Verify if an element is present in a `struct tw_bloomfilter_a2`.
 *
 * @param bf non-null bloomfilter affected
 * @param key non-null buffer of the key to test
 * @param key_size stricly positive size of the buffer key to test
 *
 * @return `false` if preconditions are not met, otherwise indicator if the
 *         element is in the bloomfilter (with possibility of false positives)
 *
 * @note group:bloomfilter_a2
 */
bool tw_bloomfilter_a2_test(const struct tw_bloomfilter_a2 *bf, const void *key,
                            size_t key_size);

/**
 * Verify if a `struct tw_bloomfilter_a2` is empty.
 *
 * @param bf non-null bloomfilter to verify emptyness
 *
 * @return `false` if bf is null, otherwise indicator if the bloomfilter is
 *         empty.
 *
 * @note group:bloomfilter_a2
 */
bool tw_bloomfilter_a2_empty(const struct tw_bloomfilter_a2 *bf);

/**
 * Verify if a `struct tw_bloomfilter_a2` is full.
 *
 * @param bf non-null bloomfilter to verify fullness
 *
 * @return `false` if bf is null, otherwise indicator if the bloomfilter is
 *         full.
 *
 * @note group:bloomfilter_a2
 */
bool tw_bloomfilter_a2_full(const struct tw_bloomfilter_a2 *bf);

/**
 * Count the number of active bits in a `struct tw_bloomfilter_a2`.
 *
 * @param bf non-null bloomfilter to count active bits
 *
 * @return `0` if bf is null, otherwise the number of active bits
 *
 * @note group:bloomfilter_a2
 */
uint64_t tw_bloomfilter_a2_count(const struct tw_bloomfilter_a2 *bf);

/**
 * Count the percentage of active bits in a `struct tw_bloomfilter_a2`.
 *
 * @param bf non-null bloomfilter to count the density
 *
 * @return `0.0` if bf is null, otherwise the portion of active bits
 *         expressed as (count / size).
 *
 * @note group:bloomfilter_a2
 */
float tw_bloomfilter_a2_density(const struct tw_bloomfilter_a2 *bf);

/**
 * Zero all bits in a `struct tw_bloomfilter_a2`.
 *
 * @param bf non-null bloomfilter to zero
 *
 * @return `NULL` if bf is null, otherwise a pointer to bf on successful
 *         operation
 *
 * @note group:bloomfilter_a2
 */
struct tw_bloomfilter_a2 *tw_bloomfilter_a2_zero(struct tw_bloomfilter_a2 *bf);

/**
 * Fill all bits in a `struct tw_bloomfilter_a2`.
 *
 * @param bf non-null bloomfilter to fill
 *
 * @return `NULL` if bf is null, otherwise a pointer to bf on successful
 *         operation
 *
 * @note group:bloomfilter_a2
 */
struct tw_bloomfilter_a2 *tw_bloomfilter_a2_fill(struct tw_bloomfilter_a2 *bf);

/**
 * Inverse all bits and zeroes in a `struct tw_bloomfilter_a2`.
 *
 * @param bf non-null bloomfilter to inverse
 *
 * @return `NULL` if failed, otherwise the bloomfilter
 *
 * @note group:bloomfilter_a2
 */
struct tw_bloomfilter_a2 *tw_bloomfilter_a2_not(struct tw_bloomfilter_a2 *bf);

/**
 * Verify if `struct tw_bloomfilter_a2`s are equal.
 *
 * @param fst first non-null bloomfilter to check
 * @param snd second non-null bloomfilter to check
 *
 * @return `false` any bloomfilter is null or hashes are not of the same
 *         cardinality, otherwise indicator if filters are equal
 *
 * @note group:bloomfilter_a2
 */
bool tw_bloomfilter_a2_equal(const struct tw_bloomfilter_a2 *fst,
                             const struct tw_bloomfilter_a2 *snd);

/**
 * Compute the union of `struct tw_bloomfilter_a2`s.
 *
 * @param src non-null bloomfilter to union from
 * @param dst non-null bloomfilter to union to
 *
 * @return: `NULL` if failed, otherwise pointer to dst
 *
 * @note group:bloomfilter_a2
 */
struct tw_bloomfilter_a2 *
tw_bloomfilter_a2_union(const struct tw_bloomfilter_a2 *src,
                        struct tw_bloomfilter_a2 *dst);

/**
 * Compute the intersection of `struct tw_bloomfilter_a2`s.
 *
 * @param src non-null bloomfilter to intersect from
 * @param dst non-null bloomfilter to intersect to
 *
 * @return: `NULL` if failed, otherwise pointer to dst
 *
 * @note group:bloomfilter_a2
 */
struct tw_bloomfilter_a2 *
tw_bloomfilter_a2_intersection(const struct tw_bloomfilter_a2 *src,
                               struct tw_bloomfilter_a2 *dst);

/**
 * Compute the symetric difference of `struct tw_bloomfilter_a2`s.
 *
 * @param src non-null bloomfilter to xor from
 * @param dst non-null bloomfilter to xor to
 *
 * @return: `NULL` if failed, otherwise pointer to dst
 *
 * @note group:bloomfilter_a2
 */
struct tw_bloomfilter_a2 *
tw_bloomfilter_a2_xor(const struct tw_bloomfilter_a2 *src,
                      struct tw_bloomfilter_a2 *dst);

#endif /* TWIDDLE_BLOOMFILTER_A2_H */
