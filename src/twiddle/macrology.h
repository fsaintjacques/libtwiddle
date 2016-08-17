#ifndef TWIDDLE_INTERNAL_UTILS_H
#define TWIDDLE_INTERNAL_UTILS_H

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <x86intrin.h>

#ifndef static_assert
#define static_assert _Static_assert
#endif

/* Number of bytes per cache line */
#ifndef TW_CACHELINE
#define TW_CACHELINE 64
#endif

#define TW_DIV_ROUND_UP(n, d) (((n) + (d)-1) / (d))

#define TW_ALLOC_TO_CACHELINE(size)                                            \
  ((TW_DIV_ROUND_UP((size), TW_CACHELINE) * TW_CACHELINE))

#define TW_ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define TW_BITOP_ADDR(x) "+m"(*(volatile long *)(x))

#define TW_BITS_IN_WORD 8
#define TW_BIT_POS(x) (1 << ((x) % TW_BITS_IN_WORD))
#define TW_BYTE_POS(x) ((x) / TW_BITS_IN_WORD)

#define tw_likely(x) __builtin_expect((x), 1)
#define tw_unlikely(x) __builtin_expect((x), 0)

/* use with care, it evaluates twice a & b */
#define tw_min(a, b) (((a) < (b)) ? (a) : (b))
#define tw_max(a, b) (((a) < (b)) ? (b) : (a))

#define tw_almost_equal(a, b) (fabs((a) - (b)) < FLT_EPSILON)

#ifdef _ISOC11_SOURCE
#define malloc_aligned aligned_alloc
#else
#if _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600
static inline void *__aligned_alloc(size_t align, size_t size)
{
  void *ptr = NULL;
  if (posix_memalign(&ptr, align, size)) {
    return NULL;
  }

  return ptr;
}
#define malloc_aligned __aligned_alloc
#else
static_assert(false, "you're in hell.");
#endif
#endif

#define tw_simd_equal(a, b, simd_cmpeq, simd_maskmove, mask)                   \
  ((int)mask == simd_maskmove(simd_cmpeq((a), (b))))

#define tw_mm256_equal(a, b)                                                   \
  tw_simd_equal((a), (b), _mm256_cmpeq_epi8, _mm256_movemask_epi8, 0xFFFFFFFF)

#define tw_mm_equal(a, b)                                                      \
  tw_simd_equal((a), (b), _mm_cmpeq_epi8, _mm_movemask_epi8, 0xFFFF)

#endif /* TWIDDLE_INTERNAL_UTILS_H */
