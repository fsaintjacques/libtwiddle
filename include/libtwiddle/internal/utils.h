#ifndef LIBTWIDDLE_INTERNAL_UTILS_H
#define LIBTWIDDLE_INTERNAL_UTILS_H

#define DIV_ROUND_UP(n,d)        (((n) + (d) - 1) / (d))

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define BITS_IN_WORD 8
#define MASK(x) (1 << (x % BITS_IN_WORD))
#define BYTE_POS(x) (x / BITS_IN_WORD)

#endif /* LIBTWIDDLE_INTERNAL_UTILS_H */
