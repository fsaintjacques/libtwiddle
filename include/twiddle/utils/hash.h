#ifndef TWIDDLE_UTILS_HASH_H
#define TWIDDLE_UTILS_HASH_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint64_t h;
  uint64_t l;
} tw_uint128_t;

uint64_t tw_hash_128_64(tw_uint128_t hash);

uint64_t tw_metrohash_64(const uint64_t seed, const void *key,
                         const size_t key_len);

tw_uint128_t tw_metrohash_128(const uint64_t seed, const void *key,
                              const size_t key_len);

uint64_t tw_murmur3_64(const uint64_t seed, const void *key,
                       const size_t key_len);

tw_uint128_t tw_murmur3_128(const uint64_t seed, const void *key,
                            const size_t key_len);

#endif /* TWIDDLE_HASH_HASH_H */
