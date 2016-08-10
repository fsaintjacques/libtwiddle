#ifndef TWIDDLE_HASH_MINHASH_H
#define TWIDDLE_HASH_MINHASH_H

#include <stdbool.h>
#include <stdint.h>

/**
 * minhash data structure
 *
 * This implementation forces use of 32bits buckets. It also uses max instead
 * of min, since it's homomorphic.
 */
struct tw_minhash {
  /** number of registers */
  uint32_t n_registers;
  /** registers holding computed values */
  uint32_t *registers;
};

/**
 * Allocate a `struct tw_minhash`.
 *
 * The allocation will be rounded up to the closest multiple of a
 * cacheline.
 *
 * @param n_registers stricly positive number of 32bit registers the structure
 *                    holds
 *
 * @return `NULL` if allocation failed, otherwise a pointer to the newly
 *         allocated `struct tw_minhash`.
 *
 * @note group:minhash
 */
struct tw_minhash *tw_minhash_new(uint32_t n_registers);

/**
 * Free a `struct tw_minhash`.
 *
 * @param hash to free
 *
 * @note group:minhash
 */
void tw_minhash_free(struct tw_minhash *hash);

/**
 * Copy a source `struct tw_minhash` into a specified destination.
 *
 * @param src non-null minhash to copy from
 * @param dst non-null minhash to copy to
 *
 * @return `NULL` if any hash is null or not of the same cardinality, otherwise
 *         a pointer to dst
 *
 * @note group:minhash
 */
struct tw_minhash *tw_minhash_copy(const struct tw_minhash *src,
                                   struct tw_minhash *dst);

/**
 * Clone a `struct tw_minhash` into a newly allocated one.
 *
 * @param hash non-null minhash to clone
 *
 * @return `NULL` if failed, otherwise a newly allocated minhash initialized
 *         from the requests minhash. The caller is responsible to deallocate
 *         the minhash with tw_minhash_free
 *
 * @note group:minhash
 */
struct tw_minhash *tw_minhash_clone(const struct tw_minhash *hash);

/**
 * Add an element into a `struct tw_minhash`.
 *
 * @param hash non-null minhash to add
 * @param key non-null buffer of the key to add
 * @param key_size stricly positive size of the buffer of the key to add
 *
 * @note group:minhash
 */
void tw_minhash_add(struct tw_minhash *hash, const void *key, size_t key_size);

/**
 * Estimate the jaccard index between two `struct tw_minhash`s.
 *
 * @param fst non-null first minhash
 * @param snd non-null second minhash
 *
 * @return `0.0` if any hash is null or hashes are not of the same cardinality,
 *         otherwise the estimated jaccard index between `fst` and `snd`
 *
 * @note group:minhash
 */
float tw_minhash_estimate(const struct tw_minhash *fst,
                          const struct tw_minhash *snd);

/**
 * Verify if `struct tw_minhash`s are equal.
 *
 * @param fst non-null first minhash
 * @param snd non-null second minhash
 *
 * @return `false` any hash is null or hashes are not of the same cardinality,
 *         otherwise indicator if hashes are equal
 *
 * @note group:minhash
 */
bool tw_minhash_equal(const struct tw_minhash *fst,
                      const struct tw_minhash *snd);

/**
 * Merge a `struct tw_minhash` in a specified destination.
 *
 * @param src non-null minhash to merge from
 * @param dst non-null minhash to merge to
 *
 * @return `NULL` if any hash is null or hashes are not of the same cardinality,
 *         otherwise pointer to dst with merged registers
 *
 * @note group:minhash
 */
struct tw_minhash *tw_minhash_merge(const struct tw_minhash *src,
                                    struct tw_minhash *dst);

#endif /* TWIDDLE_HASH_MINHASH_H */
