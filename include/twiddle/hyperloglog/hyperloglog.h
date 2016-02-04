#ifndef TWIDDLE_HYPERLOGLOG_H
#define TWIDDLE_HYPERLOGLOG_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define TW_BYTES_PER_HLL_REGISTER sizeof(uint8_t)
#define TW_BITS_PER_HLL_REGISTER (TW_BYTES_PER_HLL * TW_BITS_IN_WORD)
#define TW_HLL_BITS_PER_REG 6
#define TW_HLL_REG_PER_HLL (TW_BITS_PER_HLL / TW_HLL_BITS_PER_REG)

#define TW_HLL_ERROR_FOR_REG(reg) (1.04 / sqrt((double)(reg)))
#define TW_HLL_REG_FOR_ERROR(err) (1.0816 / ((err) * (err)))

// minimum to be aligned with a cacheline.
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

#define TW_HLL_DEFAULT_SEED 646086642ULL

/**
 * tw_hyperloglog_new() - allocates a hyperloglog data structure
 * @precision: precision hyperloglog should use
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
 * Precision of hyperloglogs must be equals.
 *
 * Return: NULL if copy failed, otherwise a pointer to dst.
 */
struct tw_hyperloglog *tw_hyperloglog_copy(const struct tw_hyperloglog *src,
                                           struct tw_hyperloglog *dst);

/**
 * tw_hyperloglog_clone() - clone a hyperloglog into a newly allocated one
 * @hll: hyperloglog to clone
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
 */
void tw_hyperloglog_add(struct tw_hyperloglog *hll, const void *key,
                        size_t key_size);

/**
 * tw_hyperloglog_count() - estimate the number of elements in hll
 * @hll: hyperloglog to estimate
 *
 * Return: estimated number of elements in hll
 */
double tw_hyperloglog_count(const struct tw_hyperloglog *hll);

/**
 * tw_hyperloglog_equal() - verify if hyperloglog are equal
 * @a: first hyperloglog to check
 * @b: second hyperloglog to check
 *
 * Return: true if equal, false otherwise
 *
 * In order to be comparable, hyperloglogs must have the same precision.
 */
bool tw_hyperloglog_equal(const struct tw_hyperloglog *a,
                          const struct tw_hyperloglog *b);

/**
 * tw_hyperloglog_merge() - merge src into dst
 * @src: hyperloglog to merge from
 * @dst: hyperloglog to merge to
 *
 * Return: pointer to merged hyperloglog structure dst, NULL if failed.
 *
 * Merged hll must have the same header (precision, hash_seed). Otherwise
 * merging is refused.
 */
struct tw_hyperloglog *tw_hyperloglog_merge(const struct tw_hyperloglog *src,
                                            struct tw_hyperloglog *dst);
#endif /* TWIDDLE_HYPERLOGLOG_H */
