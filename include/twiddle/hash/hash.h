#ifndef TWIDDLE_HASH_HASH_H
#define TWIDDLE_HASH_HASH_H

#include <stdint.h>

typedef struct {
  uint64_t h;
  uint64_t l;
} tw_uint128_t;

uint64_t tw_hash_128_64(tw_uint128_t hash);

#endif /* TWIDDLE_HASH_HASH_H */
