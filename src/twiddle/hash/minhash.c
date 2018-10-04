#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>

#include <twiddle/hash/minhash.h>
#include <twiddle/utils/hash.h>

#include "../macrology.h"

#define TW_BYTES_PER_MINHASH_REGISTER sizeof(uint32_t)

#define TW_MINHASH_DEFAULT_SEED 18014475172444421775ULL

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

  if ((hash->registers = malloc_aligned(TW_CACHELINE, data_size)) == NULL) {
    free(hash);
    return NULL;
  }

  memset(hash->registers, 0, data_size);

  hash->n_registers = n_registers;
  return hash;
}

void tw_minhash_free(struct tw_minhash *hash)
{
  if (!hash) {
    return;
  }

  free(hash->registers);
  free(hash);
}

struct tw_minhash *tw_minhash_copy(const struct tw_minhash *src,
                                   struct tw_minhash *dst)
{
  if (!src || !dst || src->n_registers != dst->n_registers) {
    return NULL;
  }

  memcpy(dst->registers, src->registers,
         src->n_registers * TW_BYTES_PER_MINHASH_REGISTER);

  return dst;
}

struct tw_minhash *tw_minhash_clone(const struct tw_minhash *hash)
{
  if (!hash) {
    return NULL;
  }

  struct tw_minhash *copy = tw_minhash_new(hash->n_registers);
  if (!copy) {
    return NULL;
  }

  return tw_minhash_copy(hash, copy);
}

void tw_minhash_add(struct tw_minhash *hash, const void *key, size_t key_size)
{
  if (!hash || !key || !key_size) {
    return;
  }

  const uint64_t hashed =
      tw_metrohash_64(TW_MINHASH_DEFAULT_SEED, key, key_size);

  const uint32_t a = (uint32_t) hashed;
  const uint32_t b = (uint32_t) (hashed >> 32);

  const uint32_t n_registers = hash->n_registers;

#define MINH_ADD_LOOP(simd_t, simd_load, simd_add, simd_max, simd_store, simd_set1, vec_elts) \
  uint32_t ib[vec_elts];                                                            \
  for(size_t i = 0; i < vec_elts; i++)                                              \
    ib[i]  = i * b;                                                                 \
  simd_t acc = simd_add(simd_set1(a), simd_load((simd_t *)ib));		            \
  simd_t inc = simd_set1(vec_elts * b);                                             \
  simd_t acc2 = simd_add(acc,inc);                                                  \
  inc = simd_add(inc,inc);                                                          \
  const size_t n_vectors =                                                          \
    n_registers * sizeof(uint32_t) / sizeof(simd_t);                                \
                                                                                    \
  for(size_t i=0; i < n_vectors; i+=2) {                                            \
    simd_t *dst =  (simd_t *)hash->registers + i;                                   \
    simd_t *dst2 =  (simd_t *)hash->registers + i + 1;                              \
    *dst = simd_max(acc, simd_load(dst));                                           \
    *dst2 = simd_max(acc2, simd_load(dst2));                                        \
    acc = simd_add(acc, inc);                                                       \
    acc2 = simd_add(acc2, inc);                                                     \
  }

#ifdef USE_AVX2
  MINH_ADD_LOOP( __m256i, _mm256_loadu_si256, _mm256_add_epi32, _mm256_max_epu32, 
		 _mm256_storeu_si256, _mm256_set1_epi32, sizeof(__m256i)/sizeof(uint32_t) )
#elif defined USE_AVX
  MINH_ADD_LOOP( __m128i, _mm_load_si128, _mm_add_epi32, _mm_max_epu32, 
		 _mm_store_si128, _mm_set1_epi32, sizeof(__m128i)/sizeof(uint32_t) )
#else
  for (size_t i = 0; i < n_registers; ++i) {
    const uint32_t hashed_i = a + i * b;
    hash->registers[i] = tw_max(hash->registers[i], hashed_i);
  }
#endif

#undef MINH_ADD_LOOP
}

float tw_minhash_estimate(const struct tw_minhash *a,
                          const struct tw_minhash *b)
{
  if (!a || !b || a->n_registers != b->n_registers) {
    return 0.0f;
  }

  const uint32_t n_registers = a->n_registers;

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

#undef MINH_EST_LOOP

  return (float)n_registers_eq / (float)n_registers;
}

bool tw_minhash_equal(const struct tw_minhash *a, const struct tw_minhash *b)
{
  if (!a || !b || a->n_registers != b->n_registers) {
    return false;
  }

  const uint32_t n_registers = a->n_registers;

#define MINH_EQ_LOOP(simd_t, simd_load, simd_equal)                            \
  const size_t n_vectors =                                                     \
      n_registers * TW_BYTES_PER_MINHASH_REGISTER / sizeof(simd_t);            \
  for (size_t i = 0; i < n_vectors; ++i) {                                     \
    simd_t *a_addr = (simd_t *)a->registers + i,                               \
           *b_addr = (simd_t *)b->registers + i;                               \
    if (!simd_equal(simd_load(a_addr), simd_load(b_addr))) {                   \
      return false;                                                            \
    }                                                                          \
  }

#ifdef USE_AVX2
  MINH_EQ_LOOP(__m256i, _mm256_load_si256, tw_mm256_equal)
#elif defined USE_AVX
  MINH_EQ_LOOP(__m128i, _mm_load_si128, tw_mm_equal)
#else
  for (size_t i = 0; i < n_registers; ++i) {
    if (a->registers[i] != b->registers[i]) {
      return false;
    }
  }
#endif

#undef MINH_EQ_LOOP

  return true;
}

struct tw_minhash *tw_minhash_merge(const struct tw_minhash *src,
                                    struct tw_minhash *dst)
{
  if (!src || !dst || src->n_registers != dst->n_registers) {
    return NULL;
  }

  const uint32_t n_registers = src->n_registers;

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

#undef MINH_MAX_LOOP

  return dst;
}
