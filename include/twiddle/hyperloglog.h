#ifndef TWIDDLE_HYPERLOGLOG_H
#define TWIDDLE_HYPERLOGLOG_H

#include <stdbool.h>
#include <stdint.h>

#define hll_t uint8_t
#define TW_BYTES_PER_HLL    sizeof(hll_t)
#define TW_BITS_PER_HLL     (TW_BYTES_PER_HLL * TW_BITS_IN_WORD)
#define TW_HLL_BITS_PER_REG 6
#define TW_HLL_REG_PER_HLL  (TW_BITS_PER_HLL / TW_HLL_BITS_PER_REG)

#define TW_HLL_ERROR_FOR_REG(reg)     (1.04  /sqrt((double) reg))
#define TW_HLL_REG_FOR_ERROR(err)     (1.0816/(err*err))

struct tw_hyperloglog {
  uint32_t size;
  uint32_t hash_seed;
  uint8_t registers[];
};

#define TW_HLL_DEFAULT_SEED 646086642L

/**
 * tw_hyperloglog_new() - allocates a hyperloglog data structure
 * @size: number of 8-bit counters the hyperloglog should hold
 *
 * Return: NULL if allocation failed, otherwise a pointer to the newly
 *         allocated `struct tw_hyperloglog`.
 */
struct tw_hyperloglog *
tw_hyperloglog_new(uint32_t size);

/**
 * tw_hyperloglog_free() - free a hyperloglog
 * @bf: hyperloglog to free
 */
void
tw_hyperloglog_free(struct tw_hyperloglog *hll);

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
 * tw_hyperloglog_estimate() - estimate the number of elements in hll
 * @hll: hyperloglog to estimate
 *
 * Return: estimated number of elements in hll
 */
double
tw_hyperloglog_estimate(const struct tw_hyperloglog *bf);
#endif /* TWIDDLE_hyperloglog_H */
