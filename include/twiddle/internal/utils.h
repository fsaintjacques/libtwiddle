#ifndef TWIDDLE_INTERNAL_UTILS_H
#define TWIDDLE_INTERNAL_UTILS_H

/* Number of bytes per cache line */
#ifndef TW_CACHELINE
#define TW_CACHELINE 64
#endif

/* Number of bytes per simd vector */
#ifndef TW_VECTOR_SIZE
#ifdef USE_AVX512
#define TW_VECTOR_SIZE 64
#elif USE_AVX2
#define TW_VECTOR_SIZE 32
#else
#define TW_VECTOR_SIZE 16
#endif
#endif

/* Number of simd vector per cache line */
#define TW_VECTOR_PER_CACHE_LINE (TW_CACHELINE / TW_VECTOR_SIZE)

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

#define tw_ffsll(x) __builtin_ffsll(x)
#define tw_ffzll(x) __builtin_ffsll(~x)
#define tw_popcountl(x) __builtin_popcountl(x)

#define tw_min(a, b) ((a < b) ? a : b)
#define tw_max(a, b) ((a < b) ? b : a)

#endif /* TWIDDLE_INTERNAL_UTILS_H */
