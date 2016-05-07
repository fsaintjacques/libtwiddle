#include <stdlib.h>

#include <twiddle/bitmap/bitmap.h>
#include <twiddle/bitmap/bitmap_rle.h>

#include "../src/twiddle/macrology.h"
#include "test.h"

static const uint32_t sizes[] = {32,   64,   128,  256,  512,
                                 1024, 2048, 4096, 32768};
static const uint32_t offsets[] = {-1, 0, 1};

START_TEST(test_bitmap_rle_basic)
{
  DESCRIBE_TEST;
  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const uint32_t nbits = sizes[i] + offsets[j];
      struct tw_bitmap_rle *bitmap = tw_bitmap_rle_new(nbits);

      for (uint32_t pos = 0; pos < nbits; ++pos) {
        if (pos % 4) {
          tw_bitmap_rle_set(bitmap, pos);
        }
      }

      for (uint32_t pos = 0; pos < nbits; ++pos) {
        if (pos % 4) {
          ck_assert(tw_bitmap_rle_test(bitmap, pos));
        } else {
          ck_assert(!tw_bitmap_rle_test(bitmap, pos));
        }
      }

      tw_bitmap_rle_free(bitmap);
    }
  }
}
END_TEST

START_TEST(test_bitmap_rle_range)
{
  DESCRIBE_TEST;
  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const uint32_t nbits = sizes[i] + offsets[j];
      struct tw_bitmap_rle *bitmap = tw_bitmap_rle_new(nbits);

      struct tw_bitmap_rle_word word = {.pos = 0, .count = nbits / 2};
      tw_bitmap_rle_set_word(bitmap, &word);
      tw_bitmap_rle_set_range(bitmap, nbits / 2 + 1, nbits - 1);

      ck_assert(tw_bitmap_rle_test(bitmap, 0));
      ck_assert(tw_bitmap_rle_test(bitmap, nbits / 2 - 1));
      ck_assert(tw_bitmap_rle_test(bitmap, nbits / 2 + 1));
      ck_assert(tw_bitmap_rle_test(bitmap, nbits - 1));

      tw_bitmap_rle_free(bitmap);
    }
  }
}
END_TEST

START_TEST(test_bitmap_rle_copy_and_clone)
{
  DESCRIBE_TEST;
  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const uint32_t nbits = sizes[i] + offsets[j];
      struct tw_bitmap_rle *src = tw_bitmap_rle_new(nbits);
      struct tw_bitmap_rle *dst = tw_bitmap_rle_new(nbits);

      struct tw_bitmap_rle_word word = {.pos = 0, .count = nbits / 2};
      tw_bitmap_rle_set_word(src, &word);
      tw_bitmap_rle_set_range(src, nbits / 2 + 1, nbits - 1);

      ck_assert_ptr_ne(tw_bitmap_rle_copy(src, dst), NULL);

      /* free original to catch potential dangling pointers */
      tw_bitmap_rle_free(src);

      struct tw_bitmap_rle *tmp = tw_bitmap_rle_clone(dst);

      const uint32_t positions[] = {0, nbits / 2 - 1, nbits / 2 + 1, nbits - 1};
      for (size_t k = 0; k < TW_ARRAY_SIZE(positions); ++k) {
        const uint32_t pos = positions[k];
        ck_assert(tw_bitmap_rle_test(dst, pos));
        ck_assert(tw_bitmap_rle_test(tmp, pos));
      }

      tw_bitmap_rle_free(tmp);
      tw_bitmap_rle_free(dst);
    }
  }
}
END_TEST

/**
 * This test is not fully representative because tw_bitmap_rle should be
 * constructed in an iterative way and then immutable. Thus swapping from
 * fill/zero/fill breaks this assumption.
 */
START_TEST(test_bitmap_rle_zero_and_fill)
{
  DESCRIBE_TEST;
  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const uint32_t nbits = sizes[i] + offsets[j];
      struct tw_bitmap_rle *bitmap = tw_bitmap_rle_new(nbits);

      ck_assert(tw_bitmap_rle_empty(bitmap));
      ck_assert(tw_almost_equal(tw_bitmap_rle_density(bitmap), 0.0));
      ck_assert(!tw_bitmap_rle_full(bitmap));

      tw_bitmap_rle_fill(bitmap);

      const uint32_t positions[] = {0, nbits / 2 - 1, nbits / 2 + 1, nbits - 1};
      for (size_t k = 0; k < TW_ARRAY_SIZE(positions); ++k) {
        ck_assert(tw_bitmap_rle_test(bitmap, positions[k]));
      }

      ck_assert(tw_bitmap_rle_full(bitmap));
      ck_assert(tw_almost_equal(tw_bitmap_rle_density(bitmap), 1.0));
      ck_assert(!tw_bitmap_rle_empty(bitmap));

      tw_bitmap_rle_zero(bitmap);

      for (size_t k = 0; k < TW_ARRAY_SIZE(positions); ++k) {
        ck_assert(!tw_bitmap_rle_test(bitmap, positions[k]));
      }

      ck_assert(tw_bitmap_rle_empty(bitmap));
      ck_assert(!tw_bitmap_rle_full(bitmap));

      tw_bitmap_rle_set_range(bitmap, nbits / 2 + 1, nbits - 1);
      ck_assert(tw_bitmap_rle_test(bitmap, nbits - 1));
      ck_assert(!tw_bitmap_rle_empty(bitmap));
      ck_assert(!tw_bitmap_rle_full(bitmap));

      tw_bitmap_rle_free(bitmap);
    }
  }
}
END_TEST

START_TEST(test_bitmap_rle_find_first)
{
  DESCRIBE_TEST;
  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const uint32_t nbits = sizes[i] + offsets[j];
      struct tw_bitmap_rle *bitmap = tw_bitmap_rle_new(nbits);

      ck_assert_int64_t_eq(tw_bitmap_rle_find_first_zero(bitmap), 0);
      ck_assert_int64_t_eq(tw_bitmap_rle_find_first_bit(bitmap), -1);

      tw_bitmap_rle_set(bitmap, 0);
      ck_assert_int64_t_eq(tw_bitmap_rle_find_first_zero(bitmap), 1);
      ck_assert_int64_t_eq(tw_bitmap_rle_find_first_bit(bitmap), 0);

      tw_bitmap_rle_set(bitmap, 1);
      ck_assert_int64_t_eq(tw_bitmap_rle_find_first_zero(bitmap), 2);
      ck_assert_int64_t_eq(tw_bitmap_rle_find_first_bit(bitmap), 0);

      tw_bitmap_rle_set(bitmap, 3);
      ck_assert_int64_t_eq(tw_bitmap_rle_find_first_zero(bitmap), 2);
      ck_assert_int64_t_eq(tw_bitmap_rle_find_first_bit(bitmap), 0);

      tw_bitmap_rle_fill(bitmap);
      ck_assert_int64_t_eq(tw_bitmap_rle_find_first_zero(bitmap), -1);
      ck_assert_int64_t_eq(tw_bitmap_rle_find_first_bit(bitmap), 0);

      tw_bitmap_rle_zero(bitmap);
      ck_assert_int64_t_eq(tw_bitmap_rle_find_first_zero(bitmap), 0);
      ck_assert_int64_t_eq(tw_bitmap_rle_find_first_bit(bitmap), -1);

      tw_bitmap_rle_set(bitmap, nbits - 1);
      ck_assert_int64_t_eq(tw_bitmap_rle_find_first_zero(bitmap), 0);
      ck_assert_int64_t_eq(tw_bitmap_rle_find_first_bit(bitmap), nbits - 1);

      tw_bitmap_rle_free(bitmap);
    }
  }
}
END_TEST

START_TEST(test_bitmap_rle_equal)
{
  DESCRIBE_TEST;
  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const uint32_t nbits = sizes[i] + offsets[j];
      struct tw_bitmap_rle *a = tw_bitmap_rle_new(nbits);
      struct tw_bitmap_rle *b = tw_bitmap_rle_clone(a);

      ck_assert(tw_bitmap_rle_equal(a, b));

      int lim = 16;
      for (int k = 0; k <= lim; ++k) {
        tw_bitmap_rle_set(a, k);
      }
      tw_bitmap_rle_set_range(b, 0, lim);

      ck_assert(tw_bitmap_rle_equal(a, b));

      tw_bitmap_rle_free(b);
      tw_bitmap_rle_free(a);
    }
  }
}
END_TEST

START_TEST(test_bitmap_rle_not)
{
  DESCRIBE_TEST;
  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const int32_t nbits = sizes[i] + offsets[j];
      struct tw_bitmap_rle *bitmap = tw_bitmap_rle_new(nbits);
      struct tw_bitmap_rle *dst = tw_bitmap_rle_clone(bitmap);

      /* basic negation */
      dst = tw_bitmap_rle_not(bitmap, dst);
      ck_assert(tw_bitmap_rle_full(dst));
      dst = tw_bitmap_rle_not(tw_bitmap_rle_fill(bitmap), dst);
      ck_assert(tw_bitmap_rle_empty(dst));

      /* first and last word overflows */
      tw_bitmap_rle_zero(bitmap);
      tw_bitmap_rle_set_range(bitmap, 1, nbits - 2);
      dst = tw_bitmap_rle_not(bitmap, dst);
      ck_assert(tw_bitmap_rle_test(dst, 0));
      ck_assert(!tw_bitmap_rle_test(dst, 1));
      ck_assert(!tw_bitmap_rle_test(dst, nbits - 2));
      ck_assert(tw_bitmap_rle_test(dst, nbits - 1));

      /* last overflows */
      tw_bitmap_rle_zero(bitmap);
      tw_bitmap_rle_set_range(bitmap, 0, nbits - 2);
      dst = tw_bitmap_rle_not(bitmap, dst);
      ck_assert(!tw_bitmap_rle_test(dst, 0));
      ck_assert(!tw_bitmap_rle_test(dst, 1));
      ck_assert(!tw_bitmap_rle_test(dst, nbits - 2));
      ck_assert(tw_bitmap_rle_test(dst, nbits - 1));

      /* first overflows */
      tw_bitmap_rle_zero(bitmap);
      tw_bitmap_rle_set_range(bitmap, 1, nbits - 1);
      dst = tw_bitmap_rle_not(bitmap, dst);
      ck_assert(tw_bitmap_rle_test(dst, 0));
      ck_assert(!tw_bitmap_rle_test(dst, 1));
      ck_assert(!tw_bitmap_rle_test(dst, nbits - 2));
      ck_assert(!tw_bitmap_rle_test(dst, nbits - 1));

      /* none overflows */
      tw_bitmap_rle_zero(bitmap);
      tw_bitmap_rle_set(bitmap, 0);
      tw_bitmap_rle_set(bitmap, nbits - 1);
      dst = tw_bitmap_rle_not(bitmap, dst);
      ck_assert(!tw_bitmap_rle_test(dst, 0));
      ck_assert(tw_bitmap_rle_test(dst, 1));
      ck_assert(tw_bitmap_rle_test(dst, nbits - 2));
      ck_assert(!tw_bitmap_rle_test(dst, nbits - 1));

      /* not(not(a)) == a */
      tw_bitmap_rle_zero(bitmap);
      int blocks = nbits / 16;
      for (int k = 1; k < blocks; ++k) {
        tw_bitmap_rle_set(bitmap, k * 16);
      }
      struct tw_bitmap_rle *tmp = tw_bitmap_rle_new(nbits);
      tmp = tw_bitmap_rle_not(bitmap, tmp);
      dst = tw_bitmap_rle_not(tmp, dst);
      ck_assert(tw_bitmap_rle_equal(bitmap, dst));

      tw_bitmap_rle_free(tmp);
      tw_bitmap_rle_free(dst);
      tw_bitmap_rle_free(bitmap);
    }
  }
}
END_TEST

START_TEST(test_bitmap_rle_union)
{
  DESCRIBE_TEST;
  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const int32_t nbits = sizes[i] + offsets[j];
      struct tw_bitmap_rle *a = tw_bitmap_rle_new(nbits);
      struct tw_bitmap_rle *b = tw_bitmap_rle_new(nbits);
      struct tw_bitmap_rle *c = tw_bitmap_rle_new(nbits);

      ck_assert_ptr_ne(tw_bitmap_rle_union(a, b, c), NULL);
      ck_assert(tw_bitmap_rle_empty(a));
      ck_assert(tw_bitmap_rle_empty(b));
      ck_assert(tw_bitmap_rle_empty(c));

      tw_bitmap_rle_fill(a);
      ck_assert_ptr_ne(tw_bitmap_rle_union(a, b, c), NULL);
      ck_assert(tw_bitmap_rle_full(a));
      ck_assert(tw_bitmap_rle_empty(b));
      ck_assert(tw_bitmap_rle_full(c));

      tw_bitmap_rle_zero(a);
      tw_bitmap_rle_zero(b);
      tw_bitmap_rle_zero(c);

      tw_bitmap_rle_set_range(a, 0, nbits / 2 + 1);
      tw_bitmap_rle_set_range(b, nbits / 2 - 1, nbits - 1);
      ck_assert_ptr_ne(tw_bitmap_rle_union(a, b, c), NULL);
      ck_assert(!tw_bitmap_rle_empty(a) && !tw_bitmap_rle_full(a));
      ck_assert(!tw_bitmap_rle_empty(b) && !tw_bitmap_rle_full(b));
      ck_assert(tw_bitmap_rle_full(c));

      tw_bitmap_rle_zero(a);
      tw_bitmap_rle_zero(b);
      tw_bitmap_rle_zero(c);

      tw_bitmap_rle_free(c);
      tw_bitmap_rle_free(b);
      tw_bitmap_rle_free(a);
    }
  }
}
END_TEST

START_TEST(test_bitmap_rle_union_advanced)
{
  DESCRIBE_TEST;
  const uint32_t nbits = 512;
  struct tw_bitmap_rle *a = tw_bitmap_rle_new(nbits);
  struct tw_bitmap_rle *b = tw_bitmap_rle_new(nbits);
  struct tw_bitmap_rle *c = tw_bitmap_rle_new(nbits);
  struct tw_bitmap_rle *expected = tw_bitmap_rle_new(nbits);

  tw_bitmap_rle_set_range(a, 0, 255);
  tw_bitmap_rle_set_range(b, 0, 7);
  tw_bitmap_rle_set_range(b, 9, 15);
  tw_bitmap_rle_set_range(b, 17, 31);
  tw_bitmap_rle_set_range(b, 255, 325);
  tw_bitmap_rle_set_range(a, 327, 410);
  tw_bitmap_rle_set_range(b, 409, 500);
  tw_bitmap_rle_set_range(a, 510, 511);
  ck_assert_ptr_ne(tw_bitmap_rle_union(a, b, c), NULL);

  tw_bitmap_rle_set_range(expected, 0, 325);
  tw_bitmap_rle_set_range(expected, 327, 500);
  tw_bitmap_rle_set_range(expected, 510, 511);
  ck_assert(tw_bitmap_rle_equal(expected, c));

  tw_bitmap_rle_zero(a);
  tw_bitmap_rle_zero(b);
  tw_bitmap_rle_zero(c);
  tw_bitmap_rle_zero(expected);

  /** Test drainage */
  tw_bitmap_rle_set_range(a, 0, 4);
  tw_bitmap_rle_set_range(b, 6, 8);
  tw_bitmap_rle_set_range(b, 10, 16);
  ck_assert_ptr_ne(tw_bitmap_rle_union(a, b, c), NULL);

  tw_bitmap_rle_set_range(expected, 0, 4);
  tw_bitmap_rle_set_range(expected, 6, 8);
  tw_bitmap_rle_set_range(expected, 10, 16);
  ck_assert(tw_bitmap_rle_equal(expected, c));

  tw_bitmap_rle_free(expected);
  tw_bitmap_rle_free(c);
  tw_bitmap_rle_free(b);
  tw_bitmap_rle_free(a);
}
END_TEST

START_TEST(test_bitmap_rle_intersection)
{
  DESCRIBE_TEST;
  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const int32_t nbits = sizes[i] + offsets[j];
      struct tw_bitmap_rle *a = tw_bitmap_rle_new(nbits);
      struct tw_bitmap_rle *b = tw_bitmap_rle_new(nbits);
      struct tw_bitmap_rle *c = tw_bitmap_rle_new(nbits);

      ck_assert_ptr_ne(tw_bitmap_rle_intersection(a, b, c), NULL);
      ck_assert(tw_bitmap_rle_empty(a));
      ck_assert(tw_bitmap_rle_empty(b));
      ck_assert(tw_bitmap_rle_empty(c));

      tw_bitmap_rle_fill(a);
      tw_bitmap_rle_zero(b);
      ck_assert_ptr_ne(tw_bitmap_rle_intersection(a, b, c), NULL);
      ck_assert(tw_bitmap_rle_full(a));
      ck_assert(tw_bitmap_rle_empty(b));
      ck_assert(tw_bitmap_rle_empty(c));

      tw_bitmap_rle_zero(a);
      tw_bitmap_rle_zero(b);
      tw_bitmap_rle_zero(c);

      tw_bitmap_rle_fill(a);
      tw_bitmap_rle_fill(b);
      ck_assert_ptr_ne(tw_bitmap_rle_intersection(a, b, c), NULL);
      ck_assert(tw_bitmap_rle_full(a));
      ck_assert(tw_bitmap_rle_full(b));
      ck_assert(tw_bitmap_rle_full(c));

      tw_bitmap_rle_zero(a);
      tw_bitmap_rle_zero(b);
      tw_bitmap_rle_zero(c);

      tw_bitmap_rle_set_range(a, 0, 4);
      tw_bitmap_rle_set_range(b, 4, nbits - 1);
      ck_assert_ptr_ne(tw_bitmap_rle_intersection(a, b, c), NULL);
      ck_assert(!tw_bitmap_rle_empty(a) && !tw_bitmap_rle_full(a));
      ck_assert(!tw_bitmap_rle_empty(b) && !tw_bitmap_rle_full(b));
      ck_assert(!tw_bitmap_rle_empty(c) && !tw_bitmap_rle_full(c));
      ck_assert(tw_bitmap_rle_test(c, 4));

      tw_bitmap_rle_zero(a);
      tw_bitmap_rle_zero(b);
      tw_bitmap_rle_zero(c);

      tw_bitmap_rle_free(c);
      tw_bitmap_rle_free(b);
      tw_bitmap_rle_free(a);
    }
  }
}
END_TEST

START_TEST(test_bitmap_rle_intersection_advanced)
{
  DESCRIBE_TEST;
  const uint32_t nbits = 512;
  struct tw_bitmap_rle *a = tw_bitmap_rle_new(nbits);
  struct tw_bitmap_rle *b = tw_bitmap_rle_new(nbits);
  struct tw_bitmap_rle *c = tw_bitmap_rle_new(nbits);
  struct tw_bitmap_rle *expected = tw_bitmap_rle_new(nbits);

  tw_bitmap_rle_set_range(a, 0, 127);
  tw_bitmap_rle_set_range(a, 255, 325);
  tw_bitmap_rle_set_range(a, 409, 511);
  tw_bitmap_rle_set_range(b, 0, 8);
  tw_bitmap_rle_set_range(b, 10, 15);
  tw_bitmap_rle_set_range(b, 17, 128);
  tw_bitmap_rle_set_range(b, 255, 500);
  ck_assert_ptr_ne(tw_bitmap_rle_intersection(a, b, c), NULL);

  tw_bitmap_rle_set_range(expected, 0, 8);
  tw_bitmap_rle_set_range(expected, 10, 15);
  tw_bitmap_rle_set_range(expected, 17, 127);
  tw_bitmap_rle_set_range(expected, 255, 325);
  tw_bitmap_rle_set_range(expected, 409, 500);
  ck_assert(tw_bitmap_rle_equal(expected, c));

  tw_bitmap_rle_zero(a);
  tw_bitmap_rle_zero(b);
  tw_bitmap_rle_zero(c);
  tw_bitmap_rle_zero(expected);

  tw_bitmap_rle_free(expected);
  tw_bitmap_rle_free(c);
  tw_bitmap_rle_free(b);
  tw_bitmap_rle_free(a);
}
END_TEST

START_TEST(test_bitmap_rle_errors)
{
  DESCRIBE_TEST;

  const size_t a_size = 1 << 16, b_size = (1 << 16) + 1;

  struct tw_bitmap_rle *a = tw_bitmap_rle_new(a_size);
  struct tw_bitmap_rle *b = tw_bitmap_rle_new(b_size);

  ck_assert_ptr_eq(tw_bitmap_rle_new(0), NULL);
  ck_assert_ptr_eq(tw_bitmap_rle_new(TW_BITMAP_MAX_BITS + 1), NULL);

  ck_assert_ptr_eq(tw_bitmap_rle_copy(a, NULL), NULL);
  ck_assert_ptr_eq(tw_bitmap_rle_copy(NULL, a), NULL);
  ck_assert_ptr_eq(tw_bitmap_rle_clone(NULL), NULL);

  /* This should not raise a segfault. */
  tw_bitmap_rle_set(NULL, a_size);
  tw_bitmap_rle_set(a, a_size);
  tw_bitmap_rle_set(a, a_size + 1);
  ck_assert(!tw_bitmap_rle_test(NULL, a_size));
  ck_assert(!tw_bitmap_rle_test(a, a_size));
  ck_assert(!tw_bitmap_rle_test(a, a_size + 1));

  ck_assert(!tw_bitmap_rle_empty(NULL));
  ck_assert(!tw_bitmap_rle_full(NULL));
  ck_assert_int_eq(tw_bitmap_rle_count(NULL), 0);
  ck_assert_ptr_eq(tw_bitmap_rle_zero(NULL), NULL);
  ck_assert_ptr_eq(tw_bitmap_rle_fill(NULL), NULL);
  ck_assert_int_eq(tw_bitmap_rle_find_first_zero(NULL), -1);
  ck_assert_int_eq(tw_bitmap_rle_find_first_bit(NULL), -1);

  ck_assert_ptr_eq(tw_bitmap_rle_not(NULL, NULL), NULL);
  ck_assert(!tw_bitmap_rle_equal(NULL, NULL));
  ck_assert(!tw_bitmap_rle_equal(a, NULL));
  ck_assert(!tw_bitmap_rle_equal(NULL, a));
  ck_assert_ptr_eq(tw_bitmap_rle_union(a, NULL, NULL), NULL);
  ck_assert_ptr_eq(tw_bitmap_rle_union(NULL, a, NULL), NULL);
  ck_assert_ptr_eq(tw_bitmap_rle_union(a, b, NULL), NULL);
  ck_assert_ptr_eq(tw_bitmap_rle_intersection(a, NULL, NULL), NULL);
  ck_assert_ptr_eq(tw_bitmap_rle_intersection(NULL, a, NULL), NULL);
  ck_assert_ptr_eq(tw_bitmap_rle_intersection(a, b, NULL), NULL);

  tw_bitmap_rle_free(b);
  tw_bitmap_rle_free(a);
}
END_TEST

int run_tests()
{
  int number_failed;

  Suite *s = suite_create("bitmap-rle");
  SRunner *runner = srunner_create(s);

  TCase *basic = tcase_create("basic");
  tcase_add_test(basic, test_bitmap_rle_basic);
  tcase_add_test(basic, test_bitmap_rle_range);
  tcase_add_test(basic, test_bitmap_rle_copy_and_clone);
  tcase_add_test(basic, test_bitmap_rle_zero_and_fill);
  tcase_add_test(basic, test_bitmap_rle_find_first);
  tcase_add_test(basic, test_bitmap_rle_errors);
  tcase_set_timeout(basic, 15);
  suite_add_tcase(s, basic);

  TCase *ops = tcase_create("set-ops");
  tcase_add_test(ops, test_bitmap_rle_equal);
  tcase_add_test(ops, test_bitmap_rle_not);
  tcase_add_test(ops, test_bitmap_rle_union);
  tcase_add_test(ops, test_bitmap_rle_union_advanced);
  tcase_add_test(ops, test_bitmap_rle_intersection);
  tcase_add_test(ops, test_bitmap_rle_intersection_advanced);
  suite_add_tcase(s, ops);

  srunner_run_all(runner, CK_NORMAL);
  number_failed = srunner_ntests_failed(runner);
  srunner_free(runner);

  return number_failed;
}

int main() { return (run_tests() == 0) ? EXIT_SUCCESS : EXIT_FAILURE; }
