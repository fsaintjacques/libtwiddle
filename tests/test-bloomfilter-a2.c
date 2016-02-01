#include <stdlib.h>

#include <check.h>

#include <twiddle/bloomfilter/bloomfilter.h>
#include <twiddle/bloomfilter/bloomfilter_a2.h>

#include "include/helpers.h"

START_TEST(test_bloomfilter_a2_basic)
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
      struct tw_bloomfilter_a2 *bf = tw_bloomfilter_a2_new(nbits, k, 0.25);

      for (size_t l = 0; l < TW_ARRAY_SIZE(values); ++l) {
        const char *value = values[l];
        tw_bloomfilter_a2_set(bf, value, strlen(value));
        ck_assert(tw_bloomfilter_a2_test(bf, value, strlen(value)));
      }

      /**
       * This is prone to failure and may be removed if causing problem.
       */
      const char *not_there = "oups!";
      ck_assert(!tw_bloomfilter_a2_test(bf, not_there, strlen(not_there)));

      tw_bloomfilter_a2_free(bf);
    }
  }
}
END_TEST

START_TEST(test_bloomfilter_a2_copy_and_clone)
{
  DESCRIBE_TEST;

  const uint32_t sizes[] = {1024, 2048, 4096, 1 << 17};
  const uint32_t ks[] = {6, 7, 8, 17};
  const uint32_t offsets[] = {-1, 0, 1};

  const char *values[] = {"herp", "derp", "ferp", "merp"};

  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const int32_t nbits = sizes[i] + offsets[j];
      const int32_t k = ks[i];
      struct tw_bloomfilter_a2 *bf = tw_bloomfilter_a2_new(nbits, k, 0.25);

      for (size_t l = 0; l < TW_ARRAY_SIZE(values); ++l) {
        const char *value = values[l];
        tw_bloomfilter_a2_set(bf, value, strlen(value));
      }

      struct tw_bloomfilter_a2 *copy = tw_bloomfilter_a2_new(nbits, k, 0.25);
      tw_bloomfilter_a2_copy(bf, copy);
      struct tw_bloomfilter_a2 *clone = tw_bloomfilter_a2_clone(copy);

      for (size_t l = 0; l < TW_ARRAY_SIZE(values); ++l) {
        const char *value = values[l];
        ck_assert(tw_bloomfilter_a2_test(bf, value, strlen(value)));
        ck_assert(tw_bloomfilter_a2_test(copy, value, strlen(value)));
        ck_assert(tw_bloomfilter_a2_test(clone, value, strlen(value)));
      }

      /**
       * This is prone to failure and may be removed if causing problem.
       */
      const char *not_there = "oups!";
      ck_assert(!tw_bloomfilter_a2_test(bf, not_there, strlen(not_there)));

      /**
       * Quickly validate independance
       */
      tw_bloomfilter_a2_zero(bf);
      ck_assert(tw_bloomfilter_a2_empty(bf));
      ck_assert(!tw_bloomfilter_a2_empty(copy));
      ck_assert(!tw_bloomfilter_a2_empty(clone));

      tw_bloomfilter_a2_zero(copy);
      ck_assert(tw_bloomfilter_a2_empty(copy));
      ck_assert(!tw_bloomfilter_a2_empty(clone));

      tw_bloomfilter_a2_free(bf);
      tw_bloomfilter_a2_free(copy);
      tw_bloomfilter_a2_free(clone);
    }
  }
}
END_TEST

START_TEST(test_bloomfilter_a2_set_operations)
{
  DESCRIBE_TEST;

  const uint32_t sizes[] = {1024, 2048, 4096};
  const uint32_t ks[] = {6, 7, 8};
  const uint32_t offsets[] = {-1, 0, 1};
  const char *values[] = {"herp", "derp", "ferp", "merp"};

  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const uint32_t nbits = sizes[i] + offsets[j];
      const uint32_t k = ks[i];
      struct tw_bloomfilter_a2 *src = tw_bloomfilter_a2_new(nbits, k, 0.25);
      struct tw_bloomfilter_a2 *dst = tw_bloomfilter_a2_new(nbits, k, 0.25);

      tw_bloomfilter_a2_set(src, values[0], strlen(values[0]));
      tw_bloomfilter_a2_set(src, values[1], strlen(values[1]));
      tw_bloomfilter_a2_set(src, values[2], strlen(values[2]));

      tw_bloomfilter_a2_set(dst, values[1], strlen(values[1]));
      tw_bloomfilter_a2_set(dst, values[2], strlen(values[2]));
      tw_bloomfilter_a2_set(dst, values[3], strlen(values[3]));

      ck_assert(tw_bloomfilter_a2_intersection(src, dst) != NULL);
      ck_assert(!tw_bloomfilter_a2_test(dst, values[0], strlen(values[0])));
      ck_assert(tw_bloomfilter_a2_test(dst, values[1], strlen(values[1])));
      ck_assert(tw_bloomfilter_a2_test(dst, values[2], strlen(values[2])));
      ck_assert(!tw_bloomfilter_a2_test(dst, values[3], strlen(values[3])));

      ck_assert(tw_bloomfilter_a2_union(src, dst) != NULL);
      ck_assert(tw_bloomfilter_a2_test(dst, values[0], strlen(values[0])));
      ck_assert(tw_bloomfilter_a2_test(dst, values[1], strlen(values[1])));
      ck_assert(tw_bloomfilter_a2_test(dst, values[2], strlen(values[2])));
      ck_assert(!tw_bloomfilter_a2_test(dst, values[3], strlen(values[3])));
      ck_assert(tw_bloomfilter_a2_equal(src, dst));

      tw_bloomfilter_a2_free(src);
      tw_bloomfilter_a2_free(dst);
    }
  }
}
END_TEST

START_TEST(test_bloomfilter_a2_test_rotation)
{
  DESCRIBE_TEST;
  const uint64_t size = 1 << 17;
  const uint16_t k = 10;
  const float density = 0.25;

  struct tw_bloomfilter_a2 *bf = tw_bloomfilter_a2_new(size, k, density);
  ck_assert(bf != NULL);
  ck_assert(tw_bloomfilter_a2_empty(bf));

  const size_t rotations = 10;
  size_t i = 0;

  for (size_t j = 0; j < rotations; ++j) {
    const size_t start = i;
    const struct tw_bloomfilter *active = bf->active;
    const struct tw_bloomfilter *passive = bf->passive;

    // add elements until density is reached
    while (tw_bloomfilter_density(active) < density) {
      tw_bloomfilter_a2_set(bf, (void *)&i, sizeof(i));
      ++i;
    }

    ck_assert(bf->active == active);
    ck_assert(bf->passive == passive);

    // trigger rotation
    tw_bloomfilter_a2_set(bf, (void *)&i, sizeof(i));
    ++i;

    ck_assert(bf->active == passive);
    ck_assert(bf->passive == active);
    ck_assert(tw_bloomfilter_density(active) >= density);
    ck_assert(tw_bloomfilter_density(passive) < density);
    ck_assert(!tw_bloomfilter_a2_empty(bf));

    // last batch should still be in bf after rotation
    for (size_t l = start; l < i; l++) {
      ck_assert(tw_bloomfilter_a2_test(bf, (void *)&l, sizeof(l)));
    }
  }
}
END_TEST

int run_tests()
{
  int number_failed;

  Suite *s = suite_create("bloomfilter_a2");
  SRunner *runner = srunner_create(s);
  TCase *tc = tcase_create("basic");
  tcase_add_test(tc, test_bloomfilter_a2_basic);
  tcase_add_test(tc, test_bloomfilter_a2_copy_and_clone);
  tcase_add_test(tc, test_bloomfilter_a2_set_operations);
  tcase_add_test(tc, test_bloomfilter_a2_test_rotation);
  suite_add_tcase(s, tc);
  srunner_run_all(runner, CK_NORMAL);
  number_failed = srunner_ntests_failed(runner);
  srunner_free(runner);

  return number_failed;
}

int main() { return (run_tests() == 0) ? EXIT_SUCCESS : EXIT_FAILURE; }
