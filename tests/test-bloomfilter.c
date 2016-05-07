#include <stdlib.h>

#include <twiddle/bloomfilter/bloomfilter.h>
#include <twiddle/internal/utils.h>

#include "test.h"

START_TEST(test_bloomfilter_basic)
{
  DESCRIBE_TEST;

  const uint32_t sizes[] = {32, 64, 128, 256, 512, 1024, 2048, 4096, 1 << 17};
  const uint32_t ks[] = {1, 2, 3, 4, 5, 6, 7, 8, 17};
  const uint32_t offsets[] = {-1, 0, 1};
  const char *values[] = {"herp", "derp", "ferp", "merp"};

  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const uint32_t nbits = sizes[i] + offsets[j];
      const uint32_t k = ks[i];
      struct tw_bloomfilter *bf = tw_bloomfilter_new(nbits, k);

      for (size_t l = 0; l < TW_ARRAY_SIZE(values); ++l) {
        const char *value = values[l];
        tw_bloomfilter_set(bf, value, strlen(value));
        ck_assert(tw_bloomfilter_test(bf, value, strlen(value)));
      }

      /**
       * This is prone to failure and may be removed if causing problem.
       */
      const char *not_there = "oups!";
      ck_assert(!tw_bloomfilter_test(bf, not_there, strlen(not_there)));

      tw_bloomfilter_free(bf);
    }
  }
}
END_TEST

START_TEST(test_bloomfilter_copy_and_clone)
{
  DESCRIBE_TEST;

  const uint32_t sizes[] = {1024, 2048, 4096, 1 << 17};
  const uint32_t ks[] = {6, 7, 8, 17};
  const uint32_t offsets[] = {-1, 0, 1};

  const char *values[] = {"herp", "derp", "ferp", "merp"};

  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const uint32_t nbits = sizes[i] + offsets[j];
      const uint32_t k = ks[i];
      struct tw_bloomfilter *bf = tw_bloomfilter_new(nbits, k);

      for (size_t l = 0; l < TW_ARRAY_SIZE(values); ++l) {
        const char *value = values[l];
        tw_bloomfilter_set(bf, value, strlen(value));
      }

      struct tw_bloomfilter *copy = tw_bloomfilter_new(nbits, k);
      tw_bloomfilter_copy(bf, copy);
      struct tw_bloomfilter *clone = tw_bloomfilter_clone(copy);

      for (size_t l = 0; l < TW_ARRAY_SIZE(values); ++l) {
        const char *value = values[l];
        ck_assert(tw_bloomfilter_test(bf, value, strlen(value)));
        ck_assert(tw_bloomfilter_test(copy, value, strlen(value)));
        ck_assert(tw_bloomfilter_test(clone, value, strlen(value)));
      }

      /**
       * This is prone to failure and may be removed if causing problem.
       */
      const char *not_there = "oups!";
      ck_assert(!tw_bloomfilter_test(bf, not_there, strlen(not_there)));

      /**
       * Quickly validate independance
       */
      tw_bloomfilter_zero(bf);
      ck_assert(tw_bloomfilter_empty(bf));
      ck_assert(!tw_bloomfilter_empty(copy));
      ck_assert(!tw_bloomfilter_empty(clone));

      tw_bloomfilter_zero(copy);
      ck_assert(tw_bloomfilter_empty(copy));
      ck_assert(!tw_bloomfilter_empty(clone));

      tw_bloomfilter_free(bf);
      tw_bloomfilter_free(copy);
      tw_bloomfilter_free(clone);
    }
  }
}
END_TEST

START_TEST(test_bloomfilter_set_operations)
{
  DESCRIBE_TEST;

  const int32_t sizes[] = {1024, 2048, 4096};
  const int32_t ks[] = {6, 7, 8};
  const int32_t offsets[] = {-1, 0, 1};
  const char *values[] = {"herp", "derp", "ferp", "merp"};

  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const int32_t nbits = sizes[i] + offsets[j];
      const int32_t k = ks[i];
      struct tw_bloomfilter *src = tw_bloomfilter_new(nbits, k);
      struct tw_bloomfilter *dst = tw_bloomfilter_new(nbits, k);

      tw_bloomfilter_set(src, values[0], strlen(values[0]));
      tw_bloomfilter_set(src, values[1], strlen(values[1]));
      tw_bloomfilter_set(src, values[2], strlen(values[2]));

      tw_bloomfilter_set(dst, values[1], strlen(values[1]));
      tw_bloomfilter_set(dst, values[2], strlen(values[2]));
      tw_bloomfilter_set(dst, values[3], strlen(values[3]));

      ck_assert_ptr_ne(tw_bloomfilter_intersection(src, dst), NULL);
      ck_assert(!tw_bloomfilter_test(dst, values[0], strlen(values[0])));
      ck_assert(tw_bloomfilter_test(dst, values[1], strlen(values[1])));
      ck_assert(tw_bloomfilter_test(dst, values[2], strlen(values[2])));
      ck_assert(!tw_bloomfilter_test(dst, values[3], strlen(values[3])));

      ck_assert_ptr_ne(tw_bloomfilter_union(src, dst), NULL);
      ck_assert(tw_bloomfilter_test(dst, values[0], strlen(values[0])));
      ck_assert(tw_bloomfilter_test(dst, values[1], strlen(values[1])));
      ck_assert(tw_bloomfilter_test(dst, values[2], strlen(values[2])));
      ck_assert(!tw_bloomfilter_test(dst, values[3], strlen(values[3])));
      ck_assert(tw_bloomfilter_equal(src, dst));

      tw_bloomfilter_free(src);
      tw_bloomfilter_free(dst);
    }
  }
}
END_TEST

START_TEST(test_bloomfilter_errors)
{
  DESCRIBE_TEST;

  uint8_t k = 8;
  uint64_t size = 1 << 18;

  struct tw_bloomfilter *a = tw_bloomfilter_new(size, k),
                        *b = tw_bloomfilter_new(size + 1, k),
                        *c = tw_bloomfilter_new(size, k + 1);

  ck_assert_ptr_eq(tw_bloomfilter_clone(NULL), NULL);
  ck_assert_ptr_eq(tw_bloomfilter_copy(a, NULL), NULL);
  ck_assert_ptr_eq(tw_bloomfilter_copy(NULL, NULL), NULL);
  ck_assert_ptr_eq(tw_bloomfilter_copy(a, b), NULL);
  ck_assert_ptr_eq(tw_bloomfilter_copy(a, c), c);

  tw_bloomfilter_set(NULL, NULL, 0);
  tw_bloomfilter_set(a, NULL, 1);
  tw_bloomfilter_set(a, &k, 0);

  tw_bloomfilter_fill(a);

  ck_assert(!tw_bloomfilter_test(NULL, NULL, 0));
  ck_assert(!tw_bloomfilter_test(a, NULL, 1));
  ck_assert(!tw_bloomfilter_test(a, &k, 0));

  ck_assert(!tw_bloomfilter_empty(NULL));
  ck_assert(!tw_bloomfilter_full(NULL));
  ck_assert_int_eq(tw_bloomfilter_count(NULL), 0);
  ck_assert_ptr_eq(tw_bloomfilter_zero(NULL), NULL);
  ck_assert_ptr_eq(tw_bloomfilter_fill(NULL), NULL);
  ck_assert_ptr_eq(tw_bloomfilter_not(NULL), NULL);

  ck_assert(!tw_bloomfilter_equal(NULL, NULL));
  ck_assert(!tw_bloomfilter_equal(a, NULL));
  ck_assert(!tw_bloomfilter_equal(a, b));
  ck_assert(!tw_bloomfilter_equal(a, c));

  ck_assert_ptr_eq(tw_bloomfilter_union(NULL, NULL), NULL);
  ck_assert_ptr_eq(tw_bloomfilter_union(a, NULL), NULL);
  ck_assert_ptr_eq(tw_bloomfilter_union(NULL, b), NULL);

  ck_assert_ptr_eq(tw_bloomfilter_intersection(NULL, NULL), NULL);
  ck_assert_ptr_eq(tw_bloomfilter_intersection(a, NULL), NULL);
  ck_assert_ptr_eq(tw_bloomfilter_intersection(NULL, b), NULL);

  ck_assert_ptr_eq(tw_bloomfilter_xor(NULL, NULL), NULL);
  ck_assert_ptr_eq(tw_bloomfilter_xor(a, NULL), NULL);
  ck_assert_ptr_eq(tw_bloomfilter_xor(NULL, b), NULL);

  tw_bloomfilter_density(NULL);

  tw_bloomfilter_free(NULL);
  tw_bloomfilter_free(c);
  tw_bloomfilter_free(b);
  tw_bloomfilter_free(a);
}
END_TEST

int run_tests()
{
  int number_failed;

  Suite *s = suite_create("bloomfilter");
  SRunner *runner = srunner_create(s);
  TCase *tc = tcase_create("basic");
  tcase_add_test(tc, test_bloomfilter_basic);
  tcase_add_test(tc, test_bloomfilter_copy_and_clone);
  tcase_add_test(tc, test_bloomfilter_set_operations);
  tcase_add_test(tc, test_bloomfilter_errors);
  suite_add_tcase(s, tc);
  srunner_run_all(runner, CK_NORMAL);
  number_failed = srunner_ntests_failed(runner);
  srunner_free(runner);

  return number_failed;
}

int main() { return (run_tests() == 0) ? EXIT_SUCCESS : EXIT_FAILURE; }
