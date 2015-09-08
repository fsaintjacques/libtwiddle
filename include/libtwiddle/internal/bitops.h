#ifndef _ASM_X86_BITOPS_H
#define _ASM_X86_BITOPS_H

/*
 * Copyright 1992, Linus Torvalds.
 *
 * Note: inlines with more than a single statement should be marked
 * __always_inline to avoid problems with older gcc's inlining heuristics.
 */

#define BITS_PER_LONG      64
#define _BITOPS_LONG_SHIFT 6
#define BIT_64(n)          (1UL << (n))
#define BITOP_ADDR(x)      "+m" (*(volatile long *) (x))
#define ADDR               BITOP_ADDR(addr)

/**
 * __set_bit - Set a bit in memory
 * @nr: the bit to set
 * @addr: the address to start counting from
 *
 * Unlike set_bit(), this function is non-atomic and may be reordered.
 * If it's called on the same region of memory simultaneously, the effect
 * may be that only one operation succeeds.
 */
static inline void __set_bit(long nr, volatile unsigned long *addr)
{
  asm volatile("bts %1,%0" : ADDR : "Ir" (nr) : "memory");
}

static inline void __clear_bit(long nr, volatile unsigned long *addr)
{
  asm volatile("btr %1,%0" : ADDR : "Ir" (nr));
}

/**
 * __change_bit - Toggle a bit in memory
 * @nr: the bit to change
 * @addr: the address to start counting from
 *
 * Unlike change_bit(), this function is non-atomic and may be reordered.
 * If it's called on the same region of memory simultaneously, the effect
 * may be that only one operation succeeds.
 */
static inline void __change_bit(long nr, volatile unsigned long *addr)
{
  asm volatile("btc %1,%0" : ADDR : "Ir" (nr));
}

/**
 * __test_and_set_bit - Set a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation is non-atomic and can be reordered.
 * If two examples of this operation race, one can appear to succeed
 * but actually fail.  You must protect multiple accesses with a lock.
 */
static inline int __test_and_set_bit(long nr, volatile unsigned long *addr)
{
  int oldbit;

  asm("bts %2,%1\n\t"
      "sbb %0,%0"
      : "=r" (oldbit), ADDR
      : "Ir" (nr));
  return oldbit;
}

/**
 * __test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to clear
 * @addr: Address to count from
 *
 * This operation is non-atomic and can be reordered.
 * If two examples of this operation race, one can appear to succeed
 * but actually fail.  You must protect multiple accesses with a lock.
 *
 * Note: the operation is performed atomically with respect to
 * the local CPU, but not other CPUs. Portable code should not
 * rely on this behaviour.
 */
static inline int __test_and_clear_bit(long nr, volatile unsigned long *addr)
{
  int oldbit;

  asm volatile("btr %2,%1\n\t"
         "sbb %0,%0"
         : "=r" (oldbit), ADDR
         : "Ir" (nr));
  return oldbit;
}

/* WARNING: non atomic and it can be reordered! */
static inline int __test_and_change_bit(long nr, volatile unsigned long *addr)
{
  int oldbit;

  asm volatile("btc %2,%1\n\t"
         "sbb %0,%0"
         : "=r" (oldbit), ADDR
         : "Ir" (nr) : "memory");

  return oldbit;
}

static __always_inline int constant_test_bit(long nr, const volatile unsigned long *addr)
{
  return ((1UL << (nr & (BITS_PER_LONG-1))) &
    (addr[nr >> _BITOPS_LONG_SHIFT])) != 0;
}

static inline int variable_test_bit(long nr, volatile const unsigned long *addr)
{
  int oldbit;

  asm volatile("bt %2,%1\n\t"
         "sbb %0,%0"
         : "=r" (oldbit)
         : "m" (*(unsigned long *)addr), "Ir" (nr));

  return oldbit;
}

/**
 * __ffs - find first set bit in word
 * @word: The word to search
 *
 * Undefined if no bit exists, so code should check against 0 first.
 */
static inline unsigned long __ffs(unsigned long word)
{
  asm("rep; bsf %1,%0"
    : "=r" (word)
    : "rm" (word));
  return word;
}

/**
 * ffz - find first zero bit in word
 * @word: The word to search
 *
 * Undefined if no zero exists, so code should check against ~0UL first.
 */
static inline unsigned long ffz(unsigned long word)
{
  asm("rep; bsf %1,%0"
    : "=r" (word)
    : "r" (~word));
  return word;
}

/*
 * __fls: find last set bit in word
 * @word: The word to search
 *
 * Undefined if no set bit exists, so code should check against 0 first.
 */
static inline unsigned long __fls(unsigned long word)
{
  asm("bsr %1,%0"
      : "=r" (word)
      : "rm" (word));
  return word;
}

/**
 * ffs - find first set bit in word
 * @x: the word to search
 *
 * This is defined the same way as the libc and compiler builtin ffs
 * routines, therefore differs in spirit from the other bitops.
 *
 * ffs(value) returns 0 if value is 0 or the position of the first
 * set bit if value is nonzero. The first (least significant) bit
 * is at position 1.
 */
static inline int tw_ffs(int x)
{
  int r;

  /*
   * AMD64 says BSFL won't clobber the dest reg if x==0; Intel64 says the
   * dest reg is undefined if x==0, but their CPU architect says its
   * value is written to set it to the same as before, except that the
   * top 32 bits will be cleared.
   *
   * We cannot do this on 32 bits because at the very least some
   * 486 CPUs did not behave this way.
   */
  asm("bsfl %1,%0"
      : "=r" (r)
      : "rm" (x), "0" (-1));
  return r + 1;
}

/**
 * fls - find last set bit in word
 * @x: the word to search
 *
 * This is defined in a similar way as the libc and compiler builtin
 * ffs, but returns the position of the most significant set bit.
 *
 * fls(value) returns 0 if value is 0 or the position of the last
 * set bit if value is nonzero. The last (most significant) bit is
 * at position 32.
 */
static inline int tw_fls(int x)
{
  int r;

  /*
   * AMD64 says BSRL won't clobber the dest reg if x==0; Intel64 says the
   * dest reg is undefined if x==0, but their CPU architect says its
   * value is written to set it to the same as before, except that the
   * top 32 bits will be cleared.
   *
   * We cannot do this on 32 bits because at the very least some
   * 486 CPUs did not behave this way.
   */
  asm("bsrl %1,%0"
      : "=r" (r)
      : "rm" (x), "0" (-1));
  return r + 1;
}

/**
 * fls64 - find last set bit in a 64-bit word
 * @x: the word to search
 *
 * This is defined in a similar way as the libc and compiler builtin
 * ffsll, but returns the position of the most significant set bit.
 *
 * fls64(value) returns 0 if value is 0 or the position of the last
 * set bit if value is nonzero. The last (most significant) bit is
 * at position 64.
 */
static __always_inline int fls64(uint64_t x)
{
  int bitpos = -1;
  /*
   * AMD64 says BSRQ won't clobber the dest reg if x==0; Intel64 says the
   * dest reg is undefined if x==0, but their CPU architect says its
   * value is written to set it to the same as before.
   */
  asm("bsrq %1,%q0"
      : "+r" (bitpos)
      : "rm" (x));
  return bitpos + 1;
}

#endif /* _ASM_X86_BITOPS_H */
