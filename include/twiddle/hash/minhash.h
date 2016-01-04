#ifndef TWIDDLE_HASH_MINHASH_H
#define TWIDDLE_HASH_MINHASH_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/**
 * struct tw_minhash_info - minhash header
 * @n_registers: number of registers
 * @hash_seed: seed used by the hash function
 */
struct tw_minhash_info {
  uint32_t n_registers;
  uint64_t hash_seed;
};

#define tw_minhash_info_init(n, h)                                             \
  (struct tw_minhash_info) { .n_registers = n, .hash_seed = h }
#define tw_minhash_info_equal(a, b)                                            \
  (a.n_registers == b.n_registers && a.hash_seed == b.hash_seed)

#define TW_BYTES_PER_MINHASH_REGISTER sizeof(uint64_t)

/**
 * struct tw_minhash - minhash data structure
 * @info:      minhash info header
 * @registers: registers holding computed values
 */
struct tw_minhash {
  struct tw_minhash_info info;
  uint64_t registers[];
};

#define TW_MINHASH_DEFAULT_SEED 18014475172444421775ULL

/**
 * tw_minhash_new() - allocates a minhash structure
 * @n_registers: number of 64bit registers the structure holds
 *
 * Return: NULL if allocation failed, otherwise a pointer to the newly
 *         allocated `struct tw_minhash`.
 */
struct tw_minhash *tw_minhash_new(uint32_t n_registers);

/**
 * tw_mihash_free() - free a minhash
 * @hash: hash to free
 */
void tw_minhash_free(struct tw_minhash *hash);

/**
 * tw_mihash_copy() - copy `src` minhash into `dst`
 * @src: minhash to copy from
 * @dst: minhash to copy to
 *
 * Size and seed of hashes must be equals.
 *
 * Return: NULL if copy failed, otherwise a pointer to dst.
 */
struct tw_minhash *tw_minhash_copy(const struct tw_minhash *src,
                                   struct tw_minhash *dst);

/**
 * tw_minhash_clone() - clone a minash into a new allocated minhash
 * @hash: minhash to clone
 *
 * Return: NULL if failed, otherwise a newly allocated minhash initialized from
 * the requests minhash. The caller is responsible to deallocated the minhash
 * with tw_minhash_free.
 */
struct tw_minhash *tw_minhash_clone(const struct tw_minhash *hash);

void tw_minhash_add(struct tw_minhash *hash, size_t key_size,
                    const char *key_buf);

/**
 * tw_minhash_estimate() - estimate the jaccard index between two minhash.
 * @a: first minhash
 * @b: second minhash
 *
 * Return: estimated jaccard index of `a` and `b`.
 */
float tw_minhash_estimate(const struct tw_minhash *a,
                          const struct tw_minhash *b);

/**
 * tw_minhash_equal() - verify if hashes are equal
 * @a: first minhash
 * @b: second minhash
 *
 * Return: true if equal, false otherwise
 *
 * Note that this function will always return false if size and/or seed are
 * not equal.
 */
bool tw_minhash_equal(const struct tw_minhash *a, const struct tw_minhash *b);

/**
 * tw_minhash_merge() - merge src into dst
 * @src: minhash to merge from
 * @dst: minhash to merge to
 *
 * Return: pointer to merged minhash structure dst, NULL if failed.
 *
 * Merged hashes must have the same size and seed. Otherwise
 * merging is refused.
 */
struct tw_minhash *tw_minhash_merge(const struct tw_minhash *src,
                                    struct tw_minhash *dst);

#endif /* TWIDDLE_HASH_MINHASH_H */
