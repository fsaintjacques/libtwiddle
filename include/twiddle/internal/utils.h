#ifndef TWIDDLE_INTERNAL_UTILS_H
#define TWIDDLE_INTERNAL_UTILS_H

#define TW_CACHELINE 64

#define TW_DIV_ROUND_UP(n, d) (((n) + (d)-1) / (d))

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
