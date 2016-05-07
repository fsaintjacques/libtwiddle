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
 * struct tw_hyperloglog - hyperloglog data structure
 * @precision: number of registers will be defined as 2^precision.
 * @registers: allocated array containing the 8bit registers
 *
 * This implementation does not use the 6-bit packing and/or mix dense/sparse
 * representation proposed in the [1].
 *
 * [1] Heule, Stefan, Marc Nunkesser, and Alexander Hall. "HyperLogLog in
 * practice: Algorithmic engineering of a state of the art cardinality
 * estimation algorithm." Proceedings of the 16th International Conference on
 * Extending Database Technology. ACM, 2013.
 */
struct tw_hyperloglog {
  uint8_t precision;
  uint8_t *registers;
};

/**
 * tw_hyperloglog_new() - allocates a hyperloglog data structure
 * @precision: precision hyperloglog should use
 *
 * `precision' must be within [TW_HLL_MIN_PRECISION, TW_HLL_MAX_PRECISION].
 *
 * Return: NULL if allocation failed, otherwise a pointer to the newly
 *         allocated `struct tw_hyperloglog`.
 */
struct tw_hyperloglog *tw_hyperloglog_new(uint8_t precision);

/**
 * tw_hyperloglog_free() - free a hyperloglog
 * @hll: hyperloglog to free
 */
void tw_hyperloglog_free(struct tw_hyperloglog *hll);

/**
 * tw_hyperloglog_copy() - copy src hyperloglog into dst
 * @src: hyperloglog to copy from
 * @dst: hyperloglog to copy to
 *
 * `src' and `dst' must be non-null and of same precision.
 *
 * Return: NULL if copy failed, otherwise a pointer to dst.
 */
struct tw_hyperloglog *tw_hyperloglog_copy(const struct tw_hyperloglog *src,
                                           struct tw_hyperloglog *dst);

/**
 * tw_hyperloglog_clone() - clone a hyperloglog into a newly allocated one
 * @hll: hyperloglog to clone
 *
 * `hll' must be non-null.
 *
 * Return: NULL if failed, otherwise a newly allocated hyperloglog initialized
 * from the requested hyperloglog. The caller is responsible to deallocate
 * with tw_hyperloglog_free.
 */
struct tw_hyperloglog *tw_hyperloglog_clone(const struct tw_hyperloglog *hll);

/**
 * tw_hyperloglog_add() - add an element in a hyperloglog structure
 * @hll:      hyperloglog affected
 * @key:      buffer of the key to add
 * @key_size: size of the key to add
 *
 * `hll' and `key' must be non-null, and key_size must be greater than 0.
 */
void tw_hyperloglog_add(struct tw_hyperloglog *hll, const void *key,
                        size_t key_size);

/**
 * tw_hyperloglog_count() - estimate the number of elements in hll
 * @hll: hyperloglog to estimate
 *
 * `hll' must be non-null.
 *
 * Return: 0.0 if hll is NULL, otherwise theestimated number of elements in hll.
 */
double tw_hyperloglog_count(const struct tw_hyperloglog *hll);

/**
 * tw_hyperloglog_equal() - verify if hyperloglog are equal
 * @fst: first hyperloglog to check
 * @snd: second hyperloglog to check
 *
 * `src' and `dst' must be non-null and of same precision.
 *
 * Return: false if pre-conditions are not met, otherwise an indicator if
 * `src' and `dst' are equal.
 */
bool tw_hyperloglog_equal(const struct tw_hyperloglog *fst,
                          const struct tw_hyperloglog *snd);

/**
 * tw_hyperloglog_merge() - merge src into dst
 * @src: hyperloglog to merge from
 * @dst: hyperloglog to merge to
 *
 * `src' and `dst' must be non-null and of same precision.
 *
 * Return: NULL if precondition are not met, otherwise a pointer to merged
 * `dst'.
 */
struct tw_hyperloglog *tw_hyperloglog_merge(const struct tw_hyperloglog *src,
                                            struct tw_hyperloglog *dst);
#endif /* TWIDDLE_HYPERLOGLOG_H */
