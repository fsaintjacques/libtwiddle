#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>

#include <twiddle/internal/utils.h>
#include <twiddle/hash/minhash.h>
#include <twiddle/hash/metrohash.h>

struct tw_minhash *tw_minhash_new(uint32_t n_registers)
{
  if (n_registers == 0) {
    return NULL;
  }

  struct tw_minhash *hash = calloc(1, sizeof(struct tw_minhash));
  if (!hash) {
    return NULL;
  }

  const size_t data_size =
      TW_ALLOC_TO_CACHELINE(n_registers * TW_BYTES_PER_MINHASH_REGISTER);

  if (posix_memalign((void *)&(hash->registers), TW_CACHELINE, data_size)) {
    free(hash);
    return NULL;
  }

  memset(hash->registers, 0, data_size);

  hash->n_registers = n_registers;
  return hash;
}

void tw_minhash_free(struct tw_minhash *hash)
{
  free(hash->registers);
  free(hash);
}

struct tw_minhash *tw_minhash_copy(const struct tw_minhash *src,
                                   struct tw_minhash *dst)
{
  assert(src && dst);

  if (src->n_registers != dst->n_registers) {
    return NULL;
  }

  memcpy(dst->registers, src->registers,
         src->n_registers * TW_BYTES_PER_MINHASH_REGISTER);

  return dst;
}

struct tw_minhash *tw_minhash_clone(const struct tw_minhash *hash)
{
  assert(hash);

  struct tw_minhash *copy = tw_minhash_new(hash->n_registers);
  if (!copy) {
    return NULL;
  }

  return tw_minhash_copy(hash, copy);
}

void tw_minhash_add(struct tw_minhash *hash, const void *key, size_t key_size)
{
  assert(hash && key && key_size > 0);

  const uint64_t hashed =
      tw_metrohash_64(TW_MINHASH_DEFAULT_SEED, key, key_size);

  const uint32_t n_registers = hash->n_registers;
  for (size_t i = 0; i < n_registers; ++i) {
    const uint32_t hashed_i = ((uint32_t)hashed + i * (uint32_t)(hashed >> 32));
    hash->registers[i] = tw_max(hash->registers[i], hashed_i);
  }
}

float tw_minhash_estimate(const struct tw_minhash *a,
                          const struct tw_minhash *b)
{
  assert(a && b);

  const uint32_t n_registers = a->n_registers;

  if (n_registers != b->n_registers) {
    return 0.0;
  }

  uint32_t n_registers_eq = 0;

#define MINH_EST_LOOP(simd_t, simd_load, simd_cmpeq, simd_maskmove, eq_mask)   \
  const size_t n_vectors =                                                     \
      n_registers * TW_BYTES_PER_MINHASH_REGISTER / sizeof(simd_t);            \
  for (size_t i = 0; i < n_vectors; ++i) {                                     \
    simd_t *a_addr = (simd_t *)a->registers + i,                               \
           *b_addr = (simd_t *)b->registers + i;                               \
    const simd_t v_cmp = simd_cmpeq(simd_load(a_addr), simd_load(b_addr));     \
    const int h_cmp = simd_maskmove(v_cmp);                                    \
    n_registers_eq += __builtin_popcount(h_cmp & eq_mask);                     \
  }

#ifdef USE_AVX2
  MINH_EST_LOOP(__m256i, _mm256_load_si256, _mm256_cmpeq_epi32,
                _mm256_movemask_epi8, 0x11111111)
#elif defined USE_AVX
  MINH_EST_LOOP(__m128i, _mm_load_si128, _mm_cmpeq_epi32, _mm_movemask_epi8,
                0x1111)
#else
  for (size_t i = 0; i < n_registers; ++i) {
    n_registers_eq += (a->registers[i] == b->registers[i]);
  }
#endif

  return (float)n_registers_eq / (float)n_registers;
}

bool tw_minhash_equal(const struct tw_minhash *a, const struct tw_minhash *b)
{
  assert(a && b);

  const uint32_t n_registers = a->n_registers;

  if (n_registers != b->n_registers) {
    return false;
  }

#define MINH_EQ_LOOP(simd_t, simd_load, simd_cmpeq, simd_maskmove, eq_mask)    \
  const size_t n_vectors =                                                     \
      n_registers * TW_BYTES_PER_MINHASH_REGISTER / sizeof(simd_t);            \
  for (size_t i = 0; i < n_vectors; ++i) {                                     \
    simd_t *a_addr = (simd_t *)a->registers + i,                               \
           *b_addr = (simd_t *)b->registers + i;                               \
    const simd_t v_cmp = simd_cmpeq(simd_load(a_addr), simd_load(b_addr));     \
    const int h_cmp = simd_maskmove(v_cmp);                                    \
    if (h_cmp != (int)eq_mask) {                                               \
      return false;                                                            \
    }                                                                          \
  }

#ifdef USE_AVX2
  MINH_EQ_LOOP(__m256i, _mm256_load_si256, _mm256_cmpeq_epi32,
               _mm256_movemask_epi8, 0xFFFFFFFF)
#elif defined USE_AVX
  MINH_EQ_LOOP(__m128i, _mm_load_si128, _mm_cmpeq_epi32, _mm_movemask_epi8,
               0xFFFF)
#else
  for (size_t i = 0; i < n_registers; ++i) {
    if (a->registers[i] != b->registers[i]) {
      return false;
    }
  }
#endif

  return true;
}

struct tw_minhash *tw_minhash_merge(const struct tw_minhash *src,
                                    struct tw_minhash *dst)
{
  assert(src && dst);

  const uint32_t n_registers = src->n_registers;

  if (n_registers != dst->n_registers) {
    return NULL;
  }

#define MINH_MAX_LOOP(simd_t, simd_load, simd_max, simd_store)                 \
  const size_t n_vectors =                                                     \
      n_registers * TW_BYTES_PER_MINHASH_REGISTER / sizeof(simd_t);            \
  for (size_t i = 0; i < n_vectors; ++i) {                                     \
    simd_t *src_vec = (simd_t *)src->registers + i,                            \
           *dst_vec = (simd_t *)dst->registers + i;                            \
    const simd_t res = simd_max(simd_load(src_vec), simd_load(dst_vec));       \
    simd_store(dst_vec, res);                                                  \
  }

#ifdef USE_AVX512
  MINH_MAX_LOOP(__m512i, _mm512_load_si512, _mm512_max_epu32,
                _mm512_store_si512)
#elif defined USE_AVX2
  MINH_MAX_LOOP(__m256i, _mm256_load_si256, _mm256_max_epu32,
                _mm256_store_si256)
#elif defined USE_AVX
  MINH_MAX_LOOP(__m128i, _mm_load_si128, _mm_max_epu32, _mm_store_si128)
#else
  for (size_t i = 0; i < n_registers; ++i) {
    dst->registers[i] = tw_max(dst->registers[i], src->registers[i]);
  }
#endif

  return dst;
}
