#ifndef TWIDDLE_UTILS_PROJECTION_H
#define TWIDDLE_UTILS_PROJECTION_H

inline uint32_t tw_projection_mod_32(uint32_t a, uint32_t b) { return a % b; }

inline uint64_t tw_projection_mod_64(uint64_t a, uint64_t b) { return a % b; }

inline uint32_t tw_projection_mul_32(uint32_t a, uint32_t b)
{
  return ((uint64_t)a * (uint64_t)b) >> 32;
}

inline uint64_t tw_projection_mul_64(uint64_t a, uint64_t b)
{
  return ((__uint128_t)a * (__uint128_t)b) >> 64;
}

#endif /* TWIDDLE_UTILS_PROJECTION_H */
