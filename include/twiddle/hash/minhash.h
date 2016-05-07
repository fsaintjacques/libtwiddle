#ifndef TWIDDLE_HASH_MINHASH_H
#define TWIDDLE_HASH_MINHASH_H

#include <stdbool.h>
#include <stdint.h>

/**
 * struct tw_minhash - minhash data structure
 * @n_registers: number of registers
 * @registers: registers holding computed values
 */
struct tw_minhash {
  uint32_t n_registers;
  uint32_t *registers;
};

/**
 * tw_minhash_new() - allocates a minhash structure
 * @n_registers: number of 64bit registers the structure holds
 *
 * `n_registers' must be greater than 0.
 *
 * Return: NULL if allocation failed, otherwise a pointer to the newly
 * allocated `struct tw_minhash`.
 *
 * Note: The allocation will be rounded up to the closest multiple of a
 * cacheline.
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
 * `src' and `dst' must be non-null and must have the same register size.
 *
 * Return: NULL if pre-conditions are not met or copy failed, otherwise a
 * pointer to dst.
 */
struct tw_minhash *tw_minhash_copy(const struct tw_minhash *src,
                                   struct tw_minhash *dst);

/**
 * tw_minhash_clone() - clone a minash into a new allocated minhash
 * @hash: minhash to clone
 *
 * `hash' must be non-null.
 *
 * Return: NULL if failed, otherwise a newly allocated minhash initialized from
 * the requests minhash. The caller is responsible to deallocated the minhash
 * with tw_minhash_free.
 */
struct tw_minhash *tw_minhash_clone(const struct tw_minhash *hash);

/**
 * tw_minhash_add() - add an element into a minhash structure
 * @hash:     minhash to add
 * @key:      buffer of the key to add
 * @key_size: size of the buffer of the key to add
 *
 * `hash' and `key' must be non-null, `key_size' must be greater than 0.
 */
void tw_minhash_add(struct tw_minhash *hash, const void *key, size_t key_size);

/**
 * tw_minhash_estimate() - estimate the jaccard index between two minhash.
 * @fst: first minhash
 * @snd: second minhash
 *
 * `fst' and `snd' must be non-null and of equal size.
 *
 * Return: 0.0 if pre-conditions are not met, otherwise the estimated jaccard
 * index of `fst` and `snd`.
 */
float tw_minhash_estimate(const struct tw_minhash *fst,
                          const struct tw_minhash *snd);

/**
 * tw_minhash_equal() - verify if hashes are equal
 * @fst: first minhash
 * @snd: second minhash
 *
 * `fst' and `snd' must be non-null and of equal size.
 *
 * Return: false if pre-conditions are not met or not equal, otherwise
 * indicator if hashes are equal.
 */
bool tw_minhash_equal(const struct tw_minhash *fst,
                      const struct tw_minhash *snd);

/**
 * tw_minhash_merge() - merge src into dst
 * @src: minhash to merge from
 * @dst: minhash to merge to
 *
 * `src' and `dst' must be non-null and of equal size.
 *
 * Return: NULL if pre-conditions are not met, otherwise pointer to dst with
 * merged registers.
 */
struct tw_minhash *tw_minhash_merge(const struct tw_minhash *src,
                                    struct tw_minhash *dst);

#endif /* TWIDDLE_HASH_MINHASH_H */
