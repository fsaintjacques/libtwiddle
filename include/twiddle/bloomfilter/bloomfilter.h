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
 * bloomfilter data structure
 *
 * This bloomfilter is static and does not support automatic resizing. The
 * underlaying storage is `struct tw_bitmap`.
 */
struct tw_bloomfilter {
  /** number of hash functions */
  uint16_t k;
  /** bitmap holding the bits */
  struct tw_bitmap *bitmap;
};

/**
 * Allocate a `struct tw_bloomfilter`.
 *
 * @param size number of bits the bloomfilter should hold, between
 *             (0, TW_BITMAP_MAX_BITS].
 * @param k stricly positive number of hash functions used
 *
 * @return `NULL` if allocation failed, otherwise a pointer to the newly
 *         allocated `struct tw_bloomfilter`
 *
 * @note group:bloomfilter
 */
struct tw_bloomfilter *tw_bloomfilter_new(uint64_t size, uint16_t k);

/**
 * Free a `struct tw_bloomfilter`.
 *
 * @param bf bloomfilter to free
 *
 * @note group:bloomfilter
 */
void tw_bloomfilter_free(struct tw_bloomfilter *bf);

/**
 * Copy a source `struct tw_bloomfilter` into a specified destination.
 *
 * @param src non-null bloomfilter to copy from
 * @param dst non-null bloomfilter to copy to
 *
 * @return `NULL` if any filter is null or not of the same cardinality,
 *         otherwise a pointer to dst
 *
 * @note group:bloomfilter
 */
struct tw_bloomfilter *tw_bloomfilter_copy(const struct tw_bloomfilter *src,
                                           struct tw_bloomfilter *dst);

/**
 * Clone a `struct tw_bloomfilter` into a newly allocated one.
 *
 * @param bf non-null bloomfilter to clone
 *
 * @return `NULL` if failed, otherwise a newly allocated bloomfilter initialized
 *         from the requested bloomfilter. The caller is responsible to
 *         deallocate with tw_bloomfilter_free
 *
 * @note group:bloomfilter
 */
struct tw_bloomfilter *tw_bloomfilter_clone(const struct tw_bloomfilter *bf);

/**
 * Set an element in a `struct tw_bloomfilter`.
 *
 * @param bf non-null bloomfilter affected
 * @param key non-null buffer of the key to add
 * @param key_size stricly positive size of the buffer key to add
 *
 * @note group:bloomfilter
 */
void tw_bloomfilter_set(struct tw_bloomfilter *bf, const void *key,
                        size_t key_size);

/**
 * Verify if an element is present in a `struct tw_bloomfilter`.
 *
 * @param bf non-null bloomfilter affected
 * @param key non-null buffer of the key to test
 * @param key_size stricly positive size of the buffer key to test
 *
 * @return `false` if preconditions are not met, otherwise indicator if the
 *         element is in the bloomfilter (with possibility of false positives)
 *
 * @note group:bloomfilter
 */
bool tw_bloomfilter_test(const struct tw_bloomfilter *bf, const void *key,
                         size_t key_size);
/**
 * Verify if a `struct tw_bloomfilter` is empty.
 *
 * @param bf non-null bloomfilter to verify emptyness
 *
 * @return `false` if bf is null, otherwise indicator if the bloomfilter is
 *         empty.
 *
 * @note group:bloomfilter
 */
bool tw_bloomfilter_empty(const struct tw_bloomfilter *bf);

/**
 * Verify if a `struct tw_bloomfilter` is full.
 *
 * @param bf non-null bloomfilter to verify fullness
 *
 * @return `false` if bf is null, otherwise indicator if the bloomfilter is
 *         full.
 *
 * @note group:bloomfilter
 */
bool tw_bloomfilter_full(const struct tw_bloomfilter *bf);

/**
 * Count the number of active bits in a `struct tw_bloomfilter`.
 *
 * @param bf non-null bloomfilter to count active bits
 *
 * @return `0` if bf is null, otherwise the number of active bits
 *
 * @note group:bloomfilter
 */
uint64_t tw_bloomfilter_count(const struct tw_bloomfilter *bf);

/**
 * Count the percentage of active bits in a `struct tw_bloomfilter`.
 *
 * @param bf non-null bloomfilter to count the density
 *
 * @return `0.0` if bf is null, otherwise the portion of active bits
 *         expressed as (count / size).
 *
 * @note group:bloomfilter
 */
float tw_bloomfilter_density(const struct tw_bloomfilter *bf);

/**
 * Zero all bits in a `struct tw_bloomfilter`.
 *
 * @param bf non-null bloomfilter to zero
 *
 * @return `NULL` if bf is null, otherwise a pointer to bf on successful
 *         operation
 *
 * @note group:bloomfilter
 */
struct tw_bloomfilter *tw_bloomfilter_zero(struct tw_bloomfilter *bf);

/**
 * Fill all bits in a `struct tw_bloomfilter`.
 *
 * @param bf non-null bloomfilter to fill
 *
 * @return `NULL` if bf is null, otherwise a pointer to bf on successful
 *         operation
 *
 * @note group:bloomfilter
 */
struct tw_bloomfilter *tw_bloomfilter_fill(struct tw_bloomfilter *bf);

/**
 * Inverse all bits and zeroes in a `struct tw_bloomfilter`.
 *
 * @param bf non-null bloomfilter to inverse
 *
 * @return `NULL` if failed, otherwise the bloomfilter
 *
 * @note group:bloomfilter
 */
struct tw_bloomfilter *tw_bloomfilter_not(struct tw_bloomfilter *bf);

/**
 * Verify if `struct tw_bloomfilter`s are equal.
 *
 * @param fst first non-null bloomfilter to check
 * @param snd second non-null bloomfilter to check
 *
 * @return `false` any bloomfilter is null or hashes are not of the same
 *         cardinality, otherwise indicator if filters are equal
 *
 * @note group:bloomfilter
 */
bool tw_bloomfilter_equal(const struct tw_bloomfilter *a,
                          const struct tw_bloomfilter *b);

/**
 * Compute the union of `struct tw_bloomfilter`s.
 *
 * @param src non-null bloomfilter to union from
 * @param dst non-null bloomfilter to union to
 *
 * @return: `NULL` if failed, otherwise pointer to dst
 *
 * @note group:bloomfilter
 */
struct tw_bloomfilter *tw_bloomfilter_union(const struct tw_bloomfilter *src,
                                            struct tw_bloomfilter *dst);

/**
 * Compute the intersection of `struct tw_bloomfilter`s.
 *
 * @param src non-null bloomfilter to intersect from
 * @param dst non-null bloomfilter to intersect to
 *
 * @return: `NULL` if failed, otherwise pointer to dst
 *
 * @note group:bloomfilter
 */
struct tw_bloomfilter *
tw_bloomfilter_intersection(const struct tw_bloomfilter *src,
                            struct tw_bloomfilter *dst);

/**
 * Compute the symetric difference of `struct tw_bloomfilter`s.
 *
 * @param src non-null bloomfilter to xor from
 * @param dst non-null bloomfilter to xor to
 *
 * @return: `NULL` if failed, otherwise pointer to dst
 *
 * @note group:bloomfilter
 */
struct tw_bloomfilter *tw_bloomfilter_xor(const struct tw_bloomfilter *src,
                                          struct tw_bloomfilter *dst);

#endif /* TWIDDLE_BLOOMFILTER_H */
