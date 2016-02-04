#include <x86intrin.h>

#define tw_simd_equal(a, b, simd_cmpeq, simd_maskmove, mask)                   \
  ((int)mask == simd_maskmove(simd_cmpeq(a, b)))

#define tw_mm256_equal(a, b)                                                   \
  tw_simd_equal(a, b, _mm256_cmpeq_epi8, _mm256_movemask_epi8, 0xFFFFFFFF)

#define tw_mm_equal(a, b)                                                      \
  tw_simd_equal(a, b, _mm_cmpeq_epi8, _mm_movemask_epi8, 0xFFFF)
