#ifndef TWIDDLE_HASH_INTERNAL_H
#define TWIDDLE_HASH_INTERNAL_H

#define BIG_CONSTANT(x) (x##LLU)

static inline uint32_t rotl32(uint32_t x, int8_t r)
{
  return (x << r) | (x >> (32 - r));
}

static inline uint32_t rotr32(uint32_t x, int8_t r)
{
  return (x >> r) | (x << (32 - r));
}

static inline uint64_t rotl64(uint64_t x, int8_t r)
{
  return (x << r) | (x >> (64 - r));
}

static inline uint64_t rotr64(uint64_t x, int8_t r)
{
  return (x >> r) | (x << (64 - r));
}

static inline uint64_t cread_u64(const void *const ptr)
{
  return *(uint64_t *)ptr;
}

static inline uint64_t cread_u32(const void *const ptr)
{
  return *(uint32_t *)ptr;
}

static inline uint64_t cread_u16(const void *const ptr)
{
  return *(uint16_t *)ptr;
}

static inline uint64_t cread_u8(const void *const ptr)
{
  return *(uint8_t *)ptr;
}

#endif /* TWIDDLE_HASH_INTERNAL_H */
