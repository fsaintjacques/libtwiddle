#ifndef TWIDDLE_INTERNAL_UTILS_H
#define TWIDDLE_INTERNAL_UTILS_H

#include <stdlib.h>
#include <float.h>
#include <math.h>

/* Number of bytes per cache line */
#ifndef TW_CACHELINE
#define TW_CACHELINE 64
#endif

#define TW_DIV_ROUND_UP(n, d) (((n) + (d)-1) / (d))

#define TW_ALLOC_TO_CACHELINE(size)                                            \
  ((TW_DIV_ROUND_UP(size, TW_CACHELINE) * TW_CACHELINE))

#define TW_ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define TW_BITOP_ADDR(x) "+m"(*(volatile long *)(x))

#define TW_BITS_IN_WORD 8
#define TW_BIT_POS(x) (1 << (x % TW_BITS_IN_WORD))
#define TW_BYTE_POS(x) (x / TW_BITS_IN_WORD)

#define tw_likely(x) __builtin_expect((x), 1)
#define tw_unlikely(x) __builtin_expect((x), 0)

#define tw_min(a, b) ((a < b) ? a : b)
#define tw_max(a, b) ((a < b) ? b : a)

#define tw_almost_equal(a, b) (fabs(a - b) < FLT_EPSILON)


/* OSX doesn't provide `aligned_alloc` like C11 mandates (we're in 2016). */
#if defined(__APPLE__)
static inline void * __aligned_alloc(size_t align, size_t size)
{
  void *ptr = NULL;
  if (posix_memalign(&ptr, align, size)) {
    return NULL;
  }

  return ptr;
}
#define malloc_aligned __aligned_alloc
#else
#define malloc_aligned aligned_alloc
#endif

#endif /* TWIDDLE_INTERNAL_UTILS_H */
