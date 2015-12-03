/**
 * Murmur3 was created by Austin Appleby, and the C port
 * and general tidying up was done by Peter Scott.
 */

#ifndef TWIDDLE_HASH_MURMUR3_H
#define TWIDDLE_HASH_MURMUR3_H

#include <stddef.h>
#include <stdint.h>

#include <twiddle/hash/hash.h>

uint64_t tw_murmur3_64(const uint64_t seed, const void *key,
                       const size_t key_len);

tw_uint128_t tw_murmur3_128(const uint64_t seed, const void *key,
                            const size_t key_len);

#endif /* TWIDDLE_HASH_MURMUR3_H */
