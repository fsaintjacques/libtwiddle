#ifndef TWIDDLE_HYPERBITBIT_H
#define TWIDDLE_HYPERBITBIT_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * struct tw_hyperbitbit - hyperbitbit data structure
 */
struct tw_hyperbitbit {
  uint8_t current_rank;
  uint64_t rank_bitmap;
  uint64_t next_rank_bitmap;
};

/**
 * tw_hyperbitbit_new() - allocates a hyperbitbit data structure
 *
 * Return: NULL if allocation failed, otherwise a pointer to the newly
 *         allocated `struct tw_hyperbitbit`.
 */
struct tw_hyperbitbit *tw_hyperbitbit_new();

/**
 * tw_hyperbitbit_free() - free a hyperbitbit
 * @hbb: hyperbitbit to free
 */
void tw_hyperbitbit_free(struct tw_hyperbitbit *hbb);

/**
 * tw_hyperbitbit_copy() - copy src hyperbitbit into dst
 * @src: hyperbitbit to copy from
 * @dst: hyperbitbit to copy to
 *
 * `src' and `dst' must be non-null.
 *
 * Return: NULL if copy failed, otherwise a pointer to dst.
 */
struct tw_hyperbitbit *tw_hyperbitbit_copy(const struct tw_hyperbitbit *src,
                                           struct tw_hyperbitbit *dst);

/**
 * tw_hyperbitbit_clone() - clone a hyperbitbit into a newly allocated one
 * @hbb: hyperbitbit to clone
 *
 * `hbb' must be non-null.
 *
 * Return: NULL if failed, otherwise a newly allocated hyperbitbit initialized
 * from the requested hyperbitbit. The caller is responsible to deallocate
 * with tw_hyperbitbit_free.
 */
struct tw_hyperbitbit *tw_hyperbitbit_clone(const struct tw_hyperbitbit *hbb);

/**
 * tw_hyperbitbit_add() - add an element in a hyperbitbit structure
 * @hbb:      hyperbitbit affected
 * @key:      buffer of the key to add
 * @key_size: size of the key to add
 *
 * `hll' and `key' must be non-null, and key_size must be greater than 0.
 */
void tw_hyperbitbit_add(struct tw_hyperbitbit *hbb, const void *key,
                        size_t key_size);

/**
 * tw_hyperbitbit_count() - estimate the number of elements in hbb
 * @hbb: hyperbitbit to estimate
 *
 * `hbb' must be non-null.
 *
 * Return: 0.0 if hbb is NULL, otherwise theestimated number of elements in hbb.
 */
double tw_hyperbitbit_count(const struct tw_hyperbitbit *hbb);

/**
 * tw_hyperbitbit_equal() - verify if hyperbitbit are equal
 * @fst: first hyperbitbit to check
 * @snd: second hyperbitbit to check
 *
 * `src' and `dst' must be non-null.
 *
 * Return: false if pre-conditions are not met, otherwise an indicator if
 * `src' and `dst' are equal.
 */
bool tw_hyperbitbit_equal(const struct tw_hyperbitbit *fst,
                          const struct tw_hyperbitbit *snd);

/**
 * tw_hyperbitbit_merge() - merge src into dst
 * @src: hyperbitbit to merge from
 * @dst: hyperbitbit to merge to
 *
 * `src' and `dst' must be non-null.
 *
 * Return: NULL if precondition are not met, otherwise a pointer to merged
 * `dst'.
 */
struct tw_hyperbitbit *tw_hyperbitbit_merge(const struct tw_hyperbitbit *src,
                                            struct tw_hyperbitbit *dst);
#endif /* TWIDDLE_HYPERBITBIT_H */
