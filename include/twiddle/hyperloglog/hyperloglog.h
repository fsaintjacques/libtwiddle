#ifndef TWIDDLE_HYPERLOGLOG_H
#define TWIDDLE_HYPERLOGLOG_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#define TW_HLL_ERROR_FOR_REG(reg) (1.04 / sqrt((double)(reg)))
#define TW_HLL_REG_FOR_ERROR(err) (1.0816 / ((err) * (err)))

#define TW_HLL_MIN_PRECISION 6
#define TW_HLL_MAX_PRECISION 18

/**
 * hyperloglog data structure
 *
 * (source https://en.wikipedia.org/wiki/HyperLogLog)
 *
 * The basis of the HyperLogLog algorithm is the observation that the
 * cardinality of a multiset of uniformly distributed random numbers can be
 * estimated by calculating the maximum number of leading zeros in the binary
 * representation of each number in the set. If the maximum number of leading
 * zeros observed is `n`, an estimate for the number of
 * distinct elements in the set is `2^{n}`.
 *
 * In the HyperLogLog algorithm, a hash function is applied to each element in
 * the original multiset, to obtain a multiset of uniformly distributed random
 * numbers with the same cardinality as the original multiset. The cardinality
 * of this randomly distributed set can then be estimated using the algorithm
 * above.
 *
 * The simple estimate of cardinality obtained using the algorithm above has
 * the disadvantage of a large variance. In the HyperLogLog algorithm, the
 * variance is minimised by splitting the multiset into numerous subsets,
 * calculating the maximum number of leading zeros in the numbers in each of
 * these subsets, and using a harmonic mean to combine these estimates for each
 * subset into an estimate of the cardinality of the whole set.
 *
 * For small cardinality, the algorithm fallback to the linear counting
 * algorithm, see [1] for a detailed analysis. This implementation's estimator
 * use the bias correction proposed in [2]. Refering to the original paper,
 * we're using `m` registers of fixed size 64 bits.
 *
 * [1] Flajolet, Philippe, et al. "Hyperloglog: the analysis of a near-optimal
 * cardinality estimation algorithm." DMTCS Proceedings 1 (2008).
 *
 * [2] Heule, Stefan, Marc Nunkesser, and Alexander Hall. "HyperLogLog in
 * practice: Algorithmic engineering of a state of the art cardinality
 * estimation algorithm." Proceedings of the 16th International Conference on
 * Extending Database Technology. ACM, 2013.
 */
struct tw_hyperloglog {
  /** the number of registers will be defined as 2^precision */
  uint8_t precision;
  /** allocated array containing the 8bit registers */
  uint8_t *registers;
};

/**
 * Allocate a `struct tw_hyperloglog`.
 *
 * @param precision power-of-2 exponent number of bucket hyperloglog should use,
 *                  must be greater or equal than `TW_HLL_MIN_PRECISION and
 *                  smaller or equal than `TW_HLL_MAX_PRECISION`
 *
 * @return `NULL` if allocation failed, otherwise a pointer to the newly
 *         allocated `struct tw_hyperloglog`.
 *
 * @note group:hyperloglog
 */
struct tw_hyperloglog *tw_hyperloglog_new(uint8_t precision);

/**
 * Free a `struct tw_hyperloglog`.
 *
 * @param hll to free
 *
 * @note group:hyperloglog
 */
void tw_hyperloglog_free(struct tw_hyperloglog *hll);

/**
 * Copy a source `struct tw_hyperloglog` into a specified destination.
 *
 * @param src non-null hyperloglog to copy from
 * @param dst non-null hyperloglog of the same precision has src to copy to
 *
 * @return `NULL` if copy failed, otherwise a pointer to dst
 *
 * @note group:hyperloglog
 */
struct tw_hyperloglog *tw_hyperloglog_copy(const struct tw_hyperloglog *src,
                                           struct tw_hyperloglog *dst);

/**
 * Clone a `struct tw_hyperloglog` into a newly allocated one.
 *
 * @param hll non-null hyperloglog to clone
 *
 * @return `NULL` if failed, otherwise a newly allocated hyperloglog initialized
 *         from the requested hyperloglog. The caller is responsible to
 *         deallocate with tw_hyperloglog_free
 *
 * @note group:hyperloglog
 */
struct tw_hyperloglog *tw_hyperloglog_clone(const struct tw_hyperloglog *hll);

/**
 * Add an element in a `struct tw_hyperloglog`.
 *
 * @param hll non-null hyperloglog to add the element to
 * @param key non-null buffer of the key to add
 * @param key_size positive integer size of the key to add
 *
 * @note group:hyperloglog
 */
void tw_hyperloglog_add(struct tw_hyperloglog *hll, const void *key,
                        size_t key_size);

/**
 * Estimate the number of elements in a `struct tw_hyperloglog`.
 *
 * @param hll non-null hyperloglog to estimate
 *
 * @return `0.0` if hll is NULL, otherwise the estimated number of elements
 *         in hll.
 *
 * @note group:hyperloglog
 */
double tw_hyperloglog_count(const struct tw_hyperloglog *hll);

/**
 * Verify if `struct tw_hyperloglog`s are equal.
 *
 * @param fst non-null first hyperloglog to check
 * @param snd non-null second hyperloglog to check
 *
 * @return `false` if any is null or not of the same precision, otherwise an
 *         indicator if `src' and `dst' are equal
 *
 * @note group:hyperloglog
 */
bool tw_hyperloglog_equal(const struct tw_hyperloglog *fst,
                          const struct tw_hyperloglog *snd);

/**
 * Merge a `struct tw_hyperloglog` in a specified destination.
 *
 * The merge operation is an elemwise max applied to the buckets.
 *
 * @param src non-null hyperloglog to merge from
 * @param dst non-null hyperloglog to merge to
 *
 * @return `NULL` if any is null or not of the same precision, otherwise a
 *         pointer to merged `dst'
 *
 * @note group:hyperloglog
 */
struct tw_hyperloglog *tw_hyperloglog_merge(const struct tw_hyperloglog *src,
                                            struct tw_hyperloglog *dst);
#endif /* TWIDDLE_HYPERLOGLOG_H */
