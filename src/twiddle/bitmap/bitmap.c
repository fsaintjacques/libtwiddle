#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>

#include <twiddle/bitmap/bitmap.h>
#include <twiddle/internal/simd.h>
#include <twiddle/internal/utils.h>

static inline void tw_bitmap_clear_extra_bits(struct tw_bitmap *bitmap)
{
  const uint64_t size = bitmap->size;
  const uint8_t rest = size % TW_BITS_PER_BITMAP;
  if (rest) {
    bitmap->data[BITMAP_POS(size)] ^= ~0UL << rest;
  }
}

struct tw_bitmap *tw_bitmap_new(uint64_t size)
{
  if (0 == size || size > TW_BITMAP_MAX_BITS) {
    return NULL;
  }

  struct tw_bitmap *bitmap = calloc(1, sizeof(struct tw_bitmap));

  if (!bitmap) {
    return NULL;
  }

  const size_t data_size =
      TW_ALLOC_TO_CACHELINE(TW_BITMAP_PER_BITS(size) * TW_BYTES_PER_BITMAP);

  if ((bitmap->data = malloc_aligned(TW_CACHELINE, data_size)) == NULL) {
    free(bitmap);
    return NULL;
  }

  memset(bitmap->data, 0, data_size);

  bitmap->size = data_size * TW_BITS_IN_WORD;
  return bitmap;
}

void tw_bitmap_free(struct tw_bitmap *bitmap)
{
  free(bitmap->data);
  free(bitmap);
}

struct tw_bitmap *tw_bitmap_copy(const struct tw_bitmap *src,
                                 struct tw_bitmap *dst)
{
  if (!src || !dst || dst->size != src->size) {
    return NULL;
  }

  dst->count = src->count;
  memcpy(dst->data, src->data,
         TW_BITMAP_PER_BITS(src->size) * TW_BYTES_PER_BITMAP);

  return dst;
}

struct tw_bitmap *tw_bitmap_clone(const struct tw_bitmap *bitmap)
{
  if (!bitmap) {
    return NULL;
  }

  struct tw_bitmap *new = tw_bitmap_new(bitmap->size);
  if (!new) {
    return NULL;
  }

  return tw_bitmap_copy(bitmap, new);
}

inline void tw_bitmap_set(struct tw_bitmap *bitmap, uint64_t pos)
{
  if (!bitmap || pos >= bitmap->size) {
    return;
  }

  const bitmap_t old_bitmap = bitmap->data[BITMAP_POS(pos)];
  const bitmap_t new_bitmap = old_bitmap | MASK(pos);
  const bool changed = (old_bitmap != new_bitmap);
  bitmap->count += changed;
  bitmap->data[BITMAP_POS(pos)] = new_bitmap;
  ;
}

inline void tw_bitmap_clear(struct tw_bitmap *bitmap, uint64_t pos)
{
  if (!bitmap || pos >= bitmap->size) {
    return;
  }

  const bitmap_t old_bitmap = bitmap->data[BITMAP_POS(pos)];
  const bitmap_t new_bitmap = old_bitmap & ~MASK(pos);
  const bool changed = (old_bitmap != new_bitmap);
  bitmap->count -= changed;
  bitmap->data[BITMAP_POS(pos)] = new_bitmap;
}

bool tw_bitmap_test(const struct tw_bitmap *bitmap, uint64_t pos)
{
  if (!bitmap || pos >= bitmap->size) {
    return false;
  }

  return !!(bitmap->data[BITMAP_POS(pos)] & MASK(pos));
}

bool tw_bitmap_test_and_set(struct tw_bitmap *bitmap, uint64_t pos)
{
  if (!bitmap || pos >= bitmap->size) {
    return false;
  }

  const bitmap_t old_bitmap = bitmap->data[BITMAP_POS(pos)];
  const bitmap_t new_bitmap = old_bitmap | MASK(pos);
  const bool changed = (old_bitmap != new_bitmap);
  bitmap->count += changed;
  bitmap->data[BITMAP_POS(pos)] = new_bitmap;
  return !changed;
}

bool tw_bitmap_test_and_clear(struct tw_bitmap *bitmap, uint64_t pos)
{
  if (!bitmap || pos >= bitmap->size) {
    return false;
  }

  const bitmap_t old_bitmap = bitmap->data[BITMAP_POS(pos)];
  const bitmap_t new_bitmap = old_bitmap & ~MASK(pos);
  const bool changed = (old_bitmap != new_bitmap);
  bitmap->count -= changed;
  bitmap->data[BITMAP_POS(pos)] = new_bitmap;
  return changed;
}

bool tw_bitmap_empty(const struct tw_bitmap *bitmap)
{
  if (!bitmap) {
    return false;
  }

  return bitmap->count == 0;
}

bool tw_bitmap_full(const struct tw_bitmap *bitmap)
{
  if (!bitmap) {
    return false;
  }

  return bitmap->size == bitmap->count;
}

uint64_t tw_bitmap_count(const struct tw_bitmap *bitmap)
{
  if (!bitmap) {
    return 0;
  }

  return bitmap->count;
}

float tw_bitmap_density(const struct tw_bitmap *bitmap)
{
  if (!bitmap) {
    return 0.0f;
  }

  return bitmap->count / (float)bitmap->size;
}

struct tw_bitmap *tw_bitmap_zero(struct tw_bitmap *bitmap)
{
  if (!bitmap) {
    return NULL;
  }

  memset(bitmap->data, 0,
         TW_BITMAP_PER_BITS(bitmap->size) * TW_BYTES_PER_BITMAP);

  bitmap->count = 0U;

  return bitmap;
}

struct tw_bitmap *tw_bitmap_fill(struct tw_bitmap *bitmap)
{
  if (!bitmap) {
    return NULL;
  }

  memset(bitmap->data, 0xFF,
         TW_BITMAP_PER_BITS(bitmap->size) * TW_BYTES_PER_BITMAP);

  tw_bitmap_clear_extra_bits(bitmap);

  bitmap->count = bitmap->size;

  return bitmap;
}

int64_t tw_bitmap_find_first_zero(const struct tw_bitmap *bitmap)
{
  if (!bitmap) {
    return -1;
  }

  /**
   * This check is required since we allocate memory on multiple of bitmap_t.
   * Thus if bitmap.size is not aligned on 64 bits, a full bitmap
   * would incorrectly report nbits+1 as the position of the first zero.
   */
  if (tw_unlikely(tw_bitmap_full(bitmap))) {
    return -1;
  }

  for (size_t i = 0; i < TW_BITMAP_PER_BITS(bitmap->size); ++i) {
    const int pos = __builtin_ffsll(~bitmap->data[i]);
    if (pos) {
      return (i * TW_BITS_PER_BITMAP) + (pos - 1);
    }
  }

  return -1;
}

int64_t tw_bitmap_find_first_bit(const struct tw_bitmap *bitmap)
{
  if (!bitmap) {
    return -1;
  }

  /**
   * This check is required since we allocate memory on multiple of bitmap_t
   * Thus if bitmap.size is not aligned on 64 bits, a full bitmap
   * filled with tw_bitmap_fill and then cleared manually with
   * tw_bitmap_clear,
   * could report the first bit to be greater than nbits.
   */
  if (tw_unlikely(tw_bitmap_empty(bitmap))) {
    return -1;
  }

  for (size_t i = 0; i < TW_BITMAP_PER_BITS(bitmap->size); ++i) {
    const int pos = __builtin_ffsll(bitmap->data[i]);
    if (pos) {
      return (i * TW_BITS_PER_BITMAP) + (pos - 1);
    }
  }

  return -1;
}

#define VECTORS_IN_BITS(simd_t, n_bits)                                        \
  (n_bits / (sizeof(simd_t) * TW_BITS_IN_WORD))

#define BITMAP_NOT_LOOP(simd_t, simd_set1, simd_load, simd_xor, simd_store)    \
  const simd_t mask = simd_set1(~0);                                           \
  for (size_t i = 0; i < VECTORS_IN_BITS(simd_t, size); ++i) {                 \
    simd_t *addr = (simd_t *)bitmap->data + i;                                 \
    const simd_t res = simd_xor(simd_load(addr), mask);                        \
    simd_store(addr, res);                                                     \
  }

struct tw_bitmap *tw_bitmap_not(struct tw_bitmap *bitmap)
{
  if (!bitmap) {
    return NULL;
  }

  const uint64_t size = bitmap->size;

#ifdef USE_AVX512
  BITMAP_NOT_LOOP(__m512i, _mm512_set1_epi8, _mm512_load_si512,
                  _mm512_xor_si512, _mm512_store_si512)
#elif defined USE_AVX2
  BITMAP_NOT_LOOP(__m256i, _mm256_set1_epi8, _mm256_load_si256,
                  _mm256_xor_si256, _mm256_store_si256)
#elif defined USE_AVX
  BITMAP_NOT_LOOP(__m128i, _mm_set1_epi8, _mm_load_si128, _mm_xor_si128,
                  _mm_store_si128)
#else
  for (size_t i = 0; i < TW_BITMAP_PER_BITS(size); ++i) {
    bitmap->data[i] ^= ~0UL;
  }
#endif

  bitmap->count = bitmap->size - bitmap->count;

  return bitmap;
}

bool tw_bitmap_equal(const struct tw_bitmap *fst, const struct tw_bitmap *snd)
{
  if (!fst || !snd) {
    return false;
  }

  if (fst->size != snd->size || fst->count != snd->count) {
    return false;
  }

  const uint64_t size = fst->size;

#define BITMAP_EQ_LOOP(simd_t, simd_load, simd_equal)                          \
  for (size_t i = 0; i < VECTORS_IN_BITS(simd_t, size); ++i) {                 \
    simd_t *fst_addr = (simd_t *)fst->data + i,                                \
           *snd_addr = (simd_t *)snd->data + i;                                \
    if (!simd_equal(simd_load(fst_addr), simd_load(snd_addr))) {               \
      return false;                                                            \
    }                                                                          \
  }

/* AVX512 does not have movemask_epi8 equivalent, fallback to AVX2 */
#ifdef USE_AVX2
  BITMAP_EQ_LOOP(__m256i, _mm256_load_si256, tw_mm256_equal)
#elif defined USE_AVX
  BITMAP_EQ_LOOP(__m128i, _mm_load_si128, tw_mm_equal)
#else
  for (size_t i = 0; i < TW_BITMAP_PER_BITS(size); ++i) {
    if (fst->data[i] != snd->data[i]) {
      return false;
    }
  }
#endif

#undef BITMAP_EQ_LOOP

  return true;
}

#define BITMAP_OP_LOOP(simd_t, simd_load, simd_op, simd_store)                 \
  const size_t uint64_t_per_simd_t = sizeof(simd_t) / sizeof(uint64_t);        \
  for (size_t i = 0; i < VECTORS_IN_BITS(simd_t, size); ++i) {                 \
    simd_t *src_vec = (simd_t *)src->data + i,                                 \
           *dst_vec = (simd_t *)dst->data + i;                                 \
    const simd_t res = simd_op(simd_load(src_vec), simd_load(dst_vec));        \
    simd_store(dst_vec, res);                                                  \
    for (size_t j = 0; j < uint64_t_per_simd_t; j++) {                         \
      count += __builtin_popcountl(dst->data[i * uint64_t_per_simd_t + j]);    \
    }                                                                          \
  }

struct tw_bitmap *tw_bitmap_union(const struct tw_bitmap *src,
                                  struct tw_bitmap *dst)
{
  if (!src || !dst || src->size != dst->size) {
    return NULL;
  }

  const uint64_t size = src->size;

  uint64_t count = 0;
#ifdef USE_AVX512
  BITMAP_OP_LOOP(__m512i, _mm512_load_si512, _mm512_or_si512,
                 _mm512_store_si512)
#elif defined USE_AVX2
  BITMAP_OP_LOOP(__m256i, _mm256_load_si256, _mm256_or_si256,
                 _mm256_store_si256)
#elif defined USE_AVX
  BITMAP_OP_LOOP(__m128i, _mm_load_si128, _mm_or_si128, _mm_store_si128)
#else
  for (size_t i = 0; i < TW_BITMAP_PER_BITS(size); ++i) {
    dst->data[i] |= src->data[i];
    count += __builtin_popcountl(dst->data[i]);
  }
#endif

  dst->count = count;

  return dst;
}

struct tw_bitmap *tw_bitmap_intersection(const struct tw_bitmap *src,
                                         struct tw_bitmap *dst)
{
  if (!src || !dst || src->size != dst->size) {
    return NULL;
  }

  const uint64_t size = src->size;

  uint64_t count = 0;
#ifdef USE_AVX512
  BITMAP_OP_LOOP(__m512i, _mm512_load_si512, _mm512_and_si512,
                 _mm512_store_si512)
#elif defined USE_AVX2
  BITMAP_OP_LOOP(__m256i, _mm256_load_si256, _mm256_and_si256,
                 _mm256_store_si256)
#elif defined USE_AVX
  BITMAP_OP_LOOP(__m128i, _mm_load_si128, _mm_and_si128, _mm_store_si128)
#else
  for (size_t i = 0; i < TW_BITMAP_PER_BITS(size); ++i) {
    dst->data[i] &= src->data[i];
    count += __builtin_popcountl(dst->data[i]);
  }
#endif

  dst->count = count;

  return dst;
}

struct tw_bitmap *tw_bitmap_xor(const struct tw_bitmap *src,
                                struct tw_bitmap *dst)
{
  if (!src || !dst || src->size != dst->size) {
    return NULL;
  }

  const uint64_t size = src->size;

  uint64_t count = 0;
#ifdef USE_AVX512
  BITMAP_OP_LOOP(__m512i, _mm512_load_si512, _mm512_xor_si512,
                 _mm512_store_si512)
#elif defined USE_AVX2
  BITMAP_OP_LOOP(__m256i, _mm256_load_si256, _mm256_xor_si256,
                 _mm256_store_si256)
#elif defined USE_AVX
  BITMAP_OP_LOOP(__m128i, _mm_load_si128, _mm_xor_si128, _mm_store_si128)
#else
  for (size_t i = 0; i < TW_BITMAP_PER_BITS(size); ++i) {
    dst->data[i] ^= src->data[i];
    count += __builtin_popcountl(dst->data[i]);
  }
#endif

  dst->count = count;

  return dst;
}
