#include <stdlib.h>

#include <check.h>

#include <twiddle/bloomfilter/bloomfilter.h>

#include "include/helpers.h"

START_TEST(test_bloomfilter_basic)
{
  DESCRIBE_TEST;

  const int32_t sizes[] = {32, 64, 128, 256, 512, 1024, 2048, 4096, 1 << 17};
  const int32_t ks[] = {1, 2, 3, 4, 5, 6, 7, 8, 17};
  const int32_t offsets[] = {-1, 0, 1};
  const char *values[] = {"herp", "derp", "ferp", "merp"};

  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const int32_t nbits = sizes[i] + offsets[j];
      const int32_t k = ks[i];
      struct tw_bloomfilter *bf = tw_bloomfilter_new(nbits, k);

      for (size_t j = 0; j < TW_ARRAY_SIZE(values); ++j) {
        const char *value = values[j];
        tw_bloomfilter_set(bf, strlen(value), value);
        ck_assert(tw_bloomfilter_test(bf, strlen(value), value));
      }

      /**
       * This is prone to failure and may be removed if causing problem.
       */
      const char *not_there = "oups!";
      ck_assert(!tw_bloomfilter_test(bf, strlen(not_there), not_there));

      tw_bloomfilter_free(bf);
    }
  }
}
END_TEST

START_TEST(test_bloomfilter_copy_and_clone)
{
  DESCRIBE_TEST;

  const int32_t sizes[] = {1024, 2048, 4096, 1 << 17};
  const int32_t ks[] = {6, 7, 8, 17};
  const int32_t offsets[] = {-1, 0, 1};

  const char *values[] = {"herp", "derp", "ferp", "merp"};

  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const int32_t nbits = sizes[i] + offsets[j];
      const int32_t k = ks[i];
      struct tw_bloomfilter *bf = tw_bloomfilter_new(nbits, k);

      for (size_t j = 0; j < TW_ARRAY_SIZE(values); ++j) {
        const char *value = values[j];
        tw_bloomfilter_set(bf, strlen(value), value);
      }

      struct tw_bloomfilter *copy = tw_bloomfilter_new(nbits, k);
      tw_bloomfilter_copy(bf, copy);
      struct tw_bloomfilter *clone = tw_bloomfilter_clone(copy);

      for (size_t j = 0; j < TW_ARRAY_SIZE(values); ++j) {
        const char *value = values[j];
        ck_assert(tw_bloomfilter_test(bf, strlen(value), value));
        ck_assert(tw_bloomfilter_test(copy, strlen(value), value));
        ck_assert(tw_bloomfilter_test(clone, strlen(value), value));
      }

      /**
       * This is prone to failure and may be removed if causing problem.
       */
      char *not_there = "oups!";
      ck_assert(!tw_bloomfilter_test(bf, strlen(not_there), not_there));

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

      tw_bloomfilter_set(src, strlen(values[0]), values[0]);
      tw_bloomfilter_set(src, strlen(values[1]), values[1]);
      tw_bloomfilter_set(src, strlen(values[2]), values[2]);

      tw_bloomfilter_set(dst, strlen(values[1]), values[1]);
      tw_bloomfilter_set(dst, strlen(values[2]), values[2]);
      tw_bloomfilter_set(dst, strlen(values[3]), values[3]);

      ck_assert(tw_bloomfilter_intersection(src, dst) != NULL);
      ck_assert(!tw_bloomfilter_test(dst, strlen(values[0]), values[0]));
      ck_assert(tw_bloomfilter_test(dst, strlen(values[1]), values[1]));
      ck_assert(tw_bloomfilter_test(dst, strlen(values[2]), values[2]));
      ck_assert(!tw_bloomfilter_test(dst, strlen(values[3]), values[3]));

      ck_assert(tw_bloomfilter_union(src, dst) != NULL);
      ck_assert(tw_bloomfilter_test(dst, strlen(values[0]), values[0]));
      ck_assert(tw_bloomfilter_test(dst, strlen(values[1]), values[1]));
      ck_assert(tw_bloomfilter_test(dst, strlen(values[2]), values[2]));
      ck_assert(!tw_bloomfilter_test(dst, strlen(values[3]), values[3]));
      ck_assert(tw_bloomfilter_equal(src, dst));

      tw_bloomfilter_free(src);
      tw_bloomfilter_free(dst);
    }
  }
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
  suite_add_tcase(s, tc);
  srunner_run_all(runner, CK_NORMAL);
  number_failed = srunner_ntests_failed(runner);
  srunner_free(runner);

  return number_failed;
}

int main(int argc, char *argv[])
{
  return (run_tests() == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
