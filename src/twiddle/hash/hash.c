#include <twiddle/hash/hash.h>

uint32_t
tw_hash_128_32(tw_uint128_t x) {
  return tw_hash_64_32(tw_hash_128_64(x));
}

uint32_t
tw_hash_64_32(uint64_t x) {
  return x;
}

uint64_t
tw_hash_128_64(tw_uint128_t x) {
  // Murmur-inspired hashing.
  const uint64_t k = 0x9ddfea08eb392d69ULL;
  uint64_t a = (x.l ^ x.h) * k;
  a ^= (a >> 47);
  uint64_t b = (x.h ^ a) * k;
  b ^= (b >> 47);
  b *= k;
  return b;
}
