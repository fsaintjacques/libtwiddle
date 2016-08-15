#include <signal.h>
#include <stdlib.h>

#include <twiddle/bitmap/bitmap.h>

#include "../src/twiddle/macrology.h"
#include "test.h"

static void validate_bitmap(struct tw_bitmap *bitmap, uint32_t nbits)
{

  ck_assert_msg(tw_bitmap_empty(bitmap), "A new bitmap should be empty");

  for (uint32_t pos = 0; pos < nbits; ++pos) {
    ck_assert(!tw_bitmap_test(bitmap, pos));
    tw_bitmap_set(bitmap, pos);
  }

  for (uint32_t pos = 0; pos < nbits; ++pos) {
    ck_assert(tw_bitmap_test(bitmap, (nbits - 1) - pos));
  }

  ck_assert(tw_bitmap_full(bitmap));

  for (uint32_t pos = 0; pos < nbits; ++pos) {
    ck_assert(tw_bitmap_test(bitmap, pos));
    tw_bitmap_clear(bitmap, pos);
    ck_assert(!tw_bitmap_test(bitmap, pos));
  }

  ck_assert(tw_bitmap_empty(bitmap));
}

START_TEST(test_bitmap_basic)
{
  DESCRIBE_TEST;

  const int32_t sizes[] = {512, 1024, 2048, 4096, 8192, 1 << 17};

  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    const int32_t nbits = sizes[i];
    struct tw_bitmap *bitmap = tw_bitmap_new(nbits);
    validate_bitmap(bitmap, nbits);
    tw_bitmap_free(bitmap);
  }
}
END_TEST

START_TEST(test_bitmap_copy_and_clone)
{
  DESCRIBE_TEST;

  const uint32_t sizes[] = {512, 1024, 2048, 4096};

  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    const uint32_t nbits = sizes[i];
    struct tw_bitmap *src = tw_bitmap_new(nbits);
    struct tw_bitmap *dst = tw_bitmap_new(nbits);

    for (uint32_t k = 0; k < nbits; ++k) {
      if (k % 2) {
        tw_bitmap_set(src, k);
      }
    }

    ck_assert_ptr_ne(tw_bitmap_copy(src, dst), NULL);

    struct tw_bitmap *tmp = tw_bitmap_clone(src);

    for (uint32_t k = 0; k < nbits; ++k) {
      if (k % 2) {
        ck_assert(tw_bitmap_test(dst, k));
        ck_assert(tw_bitmap_test(tmp, k));
      }
    }

    tw_bitmap_free(tmp);
    tw_bitmap_free(src);
    tw_bitmap_free(dst);
  }
}
END_TEST

START_TEST(test_bitmap_zero_and_fill)
{
  DESCRIBE_TEST;

  const uint32_t sizes[] = {512, 1024, 2048, 4096};

  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    const uint32_t nbits = sizes[i];
    struct tw_bitmap *bitmap = tw_bitmap_new(nbits);

    ck_assert(tw_bitmap_empty(bitmap));
    ck_assert(tw_almost_equal(tw_bitmap_density(bitmap), 0.0));
    ck_assert(!tw_bitmap_full(bitmap));

    tw_bitmap_fill(bitmap);

    for (uint32_t pos = 0; pos < nbits; ++pos) {
      ck_assert(tw_bitmap_test(bitmap, pos));
    }

    ck_assert(tw_bitmap_full(bitmap));
    ck_assert(tw_almost_equal(tw_bitmap_density(bitmap), 1.0));
    ck_assert(!tw_bitmap_empty(bitmap));

    tw_bitmap_zero(bitmap);

    for (uint32_t pos = 0; pos < nbits; ++pos) {
      ck_assert(!tw_bitmap_test(bitmap, pos));
    }

    ck_assert(tw_bitmap_empty(bitmap));
    ck_assert(!tw_bitmap_full(bitmap));

    tw_bitmap_free(bitmap);
  }
}
END_TEST

START_TEST(test_bitmap_find_first)
{
  DESCRIBE_TEST;

  const uint32_t sizes[] = {1024, 2048, 4096};

  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    const uint32_t nbits = sizes[i];
    struct tw_bitmap *bitmap = tw_bitmap_new(nbits);

    ck_assert_int64_t_eq(tw_bitmap_find_first_zero(bitmap), 0);
    ck_assert_int64_t_eq(tw_bitmap_find_first_bit(bitmap), -1);

    tw_bitmap_fill(bitmap);

    ck_assert_int64_t_eq(tw_bitmap_find_first_zero(bitmap), -1);
    ck_assert_int64_t_eq(tw_bitmap_find_first_bit(bitmap), 0);

    for (uint32_t pos = 0; pos < nbits - 1; ++pos) {
      tw_bitmap_clear(bitmap, pos);
      ck_assert_int64_t_eq(tw_bitmap_find_first_zero(bitmap), 0);
      ck_assert_int64_t_eq(tw_bitmap_find_first_bit(bitmap), pos + 1);
    }

    // clear last bit
    tw_bitmap_clear(bitmap, nbits - 1);
    ck_assert_int64_t_eq(tw_bitmap_find_first_zero(bitmap), 0);
    ck_assert_int64_t_eq(tw_bitmap_find_first_bit(bitmap), -1);

    for (uint32_t pos = 0; pos < nbits - 1; ++pos) {
      tw_bitmap_set(bitmap, pos);
      ck_assert_int64_t_eq(tw_bitmap_find_first_zero(bitmap), pos + 1);
      ck_assert_int64_t_eq(tw_bitmap_find_first_bit(bitmap), 0);
    }

    // set last bit
    tw_bitmap_set(bitmap, nbits - 1);
    ck_assert_int64_t_eq(tw_bitmap_find_first_zero(bitmap), -1);
    ck_assert_int64_t_eq(tw_bitmap_find_first_bit(bitmap), 0);

    tw_bitmap_free(bitmap);
  }
}
END_TEST

START_TEST(test_bitmap_report)
{
  DESCRIBE_TEST;
  struct tw_bitmap *bitmap = tw_bitmap_new(512);

  ck_assert(tw_bitmap_empty(bitmap));
  ck_assert(!tw_bitmap_full(bitmap));
  ck_assert(tw_almost_equal(tw_bitmap_density(bitmap), 0.0));

  uint64_t i = 0;
  for (; i < 128; i++) {
    tw_bitmap_set(bitmap, i);
  }
  ck_assert(tw_almost_equal(tw_bitmap_density(bitmap), 0.25));

  for (; i < 256; i++) {
    tw_bitmap_set(bitmap, i);
  }
  ck_assert(tw_almost_equal(tw_bitmap_density(bitmap), 0.50));

  tw_bitmap_set(bitmap, 2);
  for (; i < 384; i++) {
    tw_bitmap_set(bitmap, i);
  }
  ck_assert(tw_almost_equal(tw_bitmap_density(bitmap), 0.75));

  for (; i < 512; i++) {
    tw_bitmap_set(bitmap, i);
  }
  ck_assert(!tw_bitmap_empty(bitmap));
  ck_assert(tw_bitmap_full(bitmap));
  ck_assert(tw_almost_equal(tw_bitmap_density(bitmap), 1.0));

  tw_bitmap_free(bitmap);
}
END_TEST

START_TEST(test_bitmap_set_operations)
{
  DESCRIBE_TEST;
  const uint32_t sizes[] = {32, 64, 128, 256, 512, 1024, 2048, 4096, 1 << 17};
  const uint32_t offsets[] = {-1, 0, 1};

  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const uint32_t nbits = sizes[i] + offsets[j];
      struct tw_bitmap *src = tw_bitmap_new(nbits);
      struct tw_bitmap *dst = tw_bitmap_new(nbits);

      tw_bitmap_fill(src);
      tw_bitmap_not(src);
      ck_assert(tw_bitmap_empty(src));

      tw_bitmap_not(src);
      ck_assert(tw_bitmap_full(src));

      /* remove first and last bits */
      tw_bitmap_clear(src, 0);
      tw_bitmap_clear(src, nbits - 1);

      tw_bitmap_union(src, dst);
      ck_assert(tw_bitmap_equal(src, dst));
      /* X ^ X = X */
      ck_assert(tw_bitmap_equal(src, tw_bitmap_intersection(src, dst)));
      /* X xor X = 0 */
      ck_assert(tw_bitmap_empty(tw_bitmap_xor(dst, dst)));
      /* X | ~X = U */
      ck_assert(tw_bitmap_full(tw_bitmap_union(src, tw_bitmap_not(dst))));

      /* dst = src */
      tw_bitmap_intersection(src, dst);
      /* src.count = dst.count */
      tw_bitmap_clear(src, nbits / 2);
      tw_bitmap_clear(dst, nbits / 2 + 1);
      /* differs by one bit */
      ck_assert(!tw_bitmap_equal(src, dst));

      tw_bitmap_zero(src);
      tw_bitmap_fill(dst);

      ck_assert(tw_bitmap_equal(src, tw_bitmap_intersection(src, dst)));

      tw_bitmap_free(dst);
      tw_bitmap_free(src);
    }
  }
}
END_TEST

START_TEST(test_bitmap_errors)
{
  DESCRIBE_TEST;

  const size_t a_size = 1 << 16, b_size = (1 << 16) + 1;

  struct tw_bitmap *a = tw_bitmap_new(a_size);
  struct tw_bitmap *b = tw_bitmap_new(b_size);

  ck_assert_ptr_eq(tw_bitmap_new(0), NULL);
  ck_assert_ptr_eq(tw_bitmap_new(TW_BITMAP_MAX_BITS), NULL);

  ck_assert_ptr_eq(tw_bitmap_clone(NULL), NULL);

  /* This should not raise a segfault. */
  tw_bitmap_set(a, a_size);
  tw_bitmap_set(a, a_size + 1);
  tw_bitmap_clear(a, a_size);
  tw_bitmap_clear(a, a_size + 1);
  ck_assert(!tw_bitmap_test(a, a_size));
  ck_assert(!tw_bitmap_test(a, a_size + 1));

  ck_assert(!tw_bitmap_test_and_set(NULL, a_size));
  ck_assert(!tw_bitmap_test_and_set(a, a_size));
  ck_assert(!tw_bitmap_test_and_set(a, a_size + 1));

  ck_assert(!tw_bitmap_test_and_clear(NULL, a_size));
  ck_assert(!tw_bitmap_test_and_clear(a, a_size));
  ck_assert(!tw_bitmap_test_and_clear(a, a_size + 1));

  ck_assert(!tw_bitmap_empty(NULL));
  ck_assert(!tw_bitmap_full(NULL));
  ck_assert_int_eq(tw_bitmap_count(NULL), 0);
  ck_assert_ptr_eq(tw_bitmap_zero(NULL), NULL);
  ck_assert_ptr_eq(tw_bitmap_fill(NULL), NULL);
  ck_assert_int_eq(tw_bitmap_find_first_zero(NULL), -1);
  ck_assert_int_eq(tw_bitmap_find_first_bit(NULL), -1);

  ck_assert_ptr_eq(tw_bitmap_not(NULL), NULL);
  ck_assert(!tw_bitmap_equal(a, NULL));
  ck_assert(!tw_bitmap_equal(NULL, a));
  ck_assert_ptr_eq(tw_bitmap_union(a, NULL), NULL);
  ck_assert_ptr_eq(tw_bitmap_union(NULL, a), NULL);
  ck_assert_ptr_eq(tw_bitmap_union(a, b), NULL);
  ck_assert_ptr_eq(tw_bitmap_intersection(a, NULL), NULL);
  ck_assert_ptr_eq(tw_bitmap_intersection(NULL, a), NULL);
  ck_assert_ptr_eq(tw_bitmap_intersection(a, b), NULL);
  ck_assert_ptr_eq(tw_bitmap_xor(a, NULL), NULL);
  ck_assert_ptr_eq(tw_bitmap_xor(NULL, a), NULL);
  ck_assert_ptr_eq(tw_bitmap_xor(a, b), NULL);

  tw_bitmap_free(b);
  tw_bitmap_free(a);
}
END_TEST

int run_tests()
{
  int number_failed;

  Suite *s = suite_create("bitmap");
  SRunner *runner = srunner_create(s);

  TCase *tc = tcase_create("basic");
  tcase_add_test(tc, test_bitmap_basic);
  tcase_add_test(tc, test_bitmap_report);
  tcase_add_test(tc, test_bitmap_copy_and_clone);
  tcase_add_test(tc, test_bitmap_zero_and_fill);
  tcase_add_test(tc, test_bitmap_find_first);
  tcase_add_test(tc, test_bitmap_set_operations);
  tcase_add_test(tc, test_bitmap_errors);
  suite_add_tcase(s, tc);

  srunner_run_all(runner, CK_NORMAL);
  number_failed = srunner_ntests_failed(runner);
  srunner_free(runner);

  return number_failed;
}

int main() { return (run_tests() == 0) ? EXIT_SUCCESS : EXIT_FAILURE; }
