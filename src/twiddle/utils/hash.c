#include <twiddle/utils/hash.h>

uint64_t tw_hash_128_64(tw_uint128_t x)
{
  // Murmur-inspired hashing.
  const uint64_t k = 0x9ddfea08eb392d69ULL;
  uint64_t a = (x.l ^ x.h) * k;
  a ^= (a >> 47);
  uint64_t b = (x.h ^ a) * k;
  b ^= (b >> 47);
  b *= k;
  return b;
}
