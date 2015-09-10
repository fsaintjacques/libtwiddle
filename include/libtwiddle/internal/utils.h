#ifndef LIBTWIDDLE_INTERNAL_UTILS_H
#define LIBTWIDDLE_INTERNAL_UTILS_H

#define TW_CACHELINE 64

#define TW_DIV_ROUND_UP(n,d)        (((n) + (d) - 1) / (d))

#define TW_ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define TW_BITS_IN_WORD 8
#define TW_BIT_POS(x)  (1 << (x % TW_BITS_IN_WORD))
#define TW_BYTE_POS(x) (x / TW_BITS_IN_WORD)

#define tw_likely(x)       __builtin_expect((x),1)
#define tw_unlikely(x)     __builtin_expect((x),0)

#define tw_ffsl(x) __builtin_ffsl(x)
#define tw_ffzl(x) __builtin_ffsl(~x)

#endif /* LIBTWIDDLE_INTERNAL_UTILS_H */
