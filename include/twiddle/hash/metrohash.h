/**
 * MetroHash was created by J. Andrew Rogers, see
 * https://github.com/jandrewrogers/MetroHash
 */

#ifndef TWIDDLE_HASH_METROHASH_H
#define TWIDDLE_HASH_METROHASH_H

#include <stddef.h>
#include <stdint.h>

#include <twiddle/hash/hash.h>

uint64_t tw_metrohash_64(const uint64_t seed, const void *key,
                         const size_t key_len);

tw_uint128_t tw_metrohash_128(const uint64_t seed, const void *key,
                              const size_t key_len);

#endif /* TWIDDLE_HASH_METROHASH_H */
