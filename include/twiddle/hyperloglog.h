#ifndef TWIDDLE_HYPERLOGLOG_H
#define TWIDDLE_HYPERLOGLOG_H

#include <stdbool.h>
#include <stdint.h>

#define hll_t uint8_t
#define TW_BYTES_PER_HLL    sizeof(hll_t)
#define TW_BITS_PER_HLL     (TW_BYTES_PER_HLL * TW_BITS_IN_WORD)
#define TW_HLL_BITS_PER_REG 6
#define TW_HLL_REG_PER_HLL  (TW_BITS_PER_HLL / TW_HLL_BITS_PER_REG)

#define TW_HLL_ERROR_FOR_REG(reg)     (1.04  /sqrt((double) (reg)))
#define TW_HLL_REG_FOR_ERROR(err)     (1.0816/((err)*(err)))

#define tw_hyperloglog_info_copy(src, dst) \
  dst = (struct tw_hyperloglog_info) {.precision= src.precision, .hash_seed = src.hash_seed}
#define tw_hyperloglog_info_equal(a,b) \
  (a.precision == b.precision && a.hash_seed == b.hash_seed)

/**
 * struct tw_hyperloglog_info - hyperloglog header
 * @precision: number of registers will be defined as 2^precision.
 * @hash_seed: seed used by the hash function
 *
 * We use precision instead of n_registers because this is the definition used
 * in the original paper and makes it easier to follow the code.
 */
struct tw_hyperloglog_info {
  uint32_t precision;
  uint32_t hash_seed;
};

/**
 * struct tw_hyperloglog - hyperloglog data structure
 * @info:      header information
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
  struct tw_hyperloglog_info info;
  uint8_t registers[];
};

#define TW_HLL_DEFAULT_SEED 646086642U

/**
 * tw_hyperloglog_new() - allocates a hyperloglog data structure
 * @precision: precision hyperloglog should use
 *
 * Return: NULL if allocation failed, otherwise a pointer to the newly
 *         allocated `struct tw_hyperloglog`.
 */
struct tw_hyperloglog *
tw_hyperloglog_new(uint32_t precision);

/**
 * tw_hyperloglog_free() - free a hyperloglog
 * @hll: hyperloglog to free
 */
void
tw_hyperloglog_free(struct tw_hyperloglog *hll);

/**
 * tw_hyperloglog_copy() - copy src hyperloglog into dst
 * @src: hyperloglog to copy from
 * @dst: hyperloglog to copy to
 *
 * Precision of hyperloglogs must be equals.
 *
 * Return: NULL if copy failed, otherwise a pointer to dst.
 */
struct tw_hyperloglog *
tw_hyperloglog_copy(const struct tw_hyperloglog *src,
                          struct tw_hyperloglog *dst);

/**
 * tw_hyperloglog_clone() - clone a hyperloglog into a newly allocated one
 * @hll: hyperloglog to clone
 *
 * Return: NULL if failed, otherwise a newly allocated hyperloglog initialized
 * from the requested hyperloglog. The caller is responsible to deallocate
 * with tw_hyperloglog_free.
 */
struct tw_hyperloglog *
tw_hyperloglog_clone(const struct tw_hyperloglog *hll);

/**
 * tw_hyperloglog_add_hashed() - add an element in a hyperloglog structure
 * @hll:  hyperloglog affected
 * @hash: hashed value to add
 */
void
tw_hyperloglog_add_hashed(struct tw_hyperloglog *hll, uint64_t hash);

/**
 * tw_hyperloglog_add() - add an element in a hyperloglog structure
 * @hll:      hyperloglog affected
 * @key_size: size of the key to add
 * @key_buf:  buf to the key to add
 */
void
tw_hyperloglog_add(struct tw_hyperloglog *hll,
                   size_t key_size, const char* key_buf);

/**
 * tw_hyperloglog_count() - estimate the number of elements in hll
 * @hll: hyperloglog to estimate
 *
 * Return: estimated number of elements in hll
 */
double
tw_hyperloglog_count(const struct tw_hyperloglog *hll);
#endif /* TWIDDLE_hyperloglog_H */
