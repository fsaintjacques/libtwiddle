#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include <check.h>

#include <twiddle/internal/utils.h>
#include <twiddle/hash/minhash.h>

#include "test.h"

static bool estimate_in_bounds(uint32_t n, float jaccard,
                               float jaccard_estimate)
{
  /* This is solely used to make test pass with reasonnable bounds */
  const float epsilon = 0.25;
  return fabs(jaccard - jaccard_estimate) < (1 + epsilon) * (1 / sqrt(n));
}

START_TEST(test_minhash_basic)
{
  DESCRIBE_TEST;

  const uint32_t sizes[] = {256, 512, 1024, 2048, 4096, 1 << 13};

  const uint8_t sample = 4;

  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    const uint32_t n_registers = sizes[i];
    struct tw_minhash *a = tw_minhash_new(n_registers);
    struct tw_minhash *b = tw_minhash_new(n_registers);

    const uint32_t n_items = n_registers * 4;
    uint32_t intersection = 0;
    for (size_t j = 0; j < n_items; ++j) {
      const size_t key_size = sizeof(j);
      const void *key = (void *)&j;

      tw_minhash_add(a, key, key_size);
      if (j % sample == 0) {
        tw_minhash_add(b, key, key_size);
        intersection++;
      }
    }

    const float jaccard = (float)intersection / (float)n_items;
    const float jaccard_estimate = tw_minhash_estimate(a, b);
    ck_assert(estimate_in_bounds(n_registers, jaccard, jaccard_estimate));

    tw_minhash_free(b);
    tw_minhash_free(a);
  }
}
END_TEST

START_TEST(test_minhash_copy_and_clone)
{
  DESCRIBE_TEST;

  const uint32_t sizes[] = {256, 512, 1024, 2048, 4096, 1 << 13};

  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    const uint32_t n_registers = sizes[i];
    struct tw_minhash *a = tw_minhash_new(n_registers);
    struct tw_minhash *b = tw_minhash_clone(a);

    ck_assert(tw_minhash_equal(a, b));

    const uint32_t n_items = n_registers / 2;
    for (size_t j = 0; j < n_items; ++j) {
      const size_t key_size = sizeof(j);
      const void *key = (void *)&j;
      tw_minhash_add(a, key, key_size);
    }

    ck_assert(!tw_minhash_equal(a, b));

    for (size_t j = 0; j < n_items; ++j) {
      const size_t key_size = sizeof(j);
      const void *key = (void *)&j;
      tw_minhash_add(b, key, key_size);
    }

    ck_assert(tw_minhash_equal(a, b));

    struct tw_minhash *c = tw_minhash_new(n_registers);

    ck_assert(!tw_minhash_equal(a, c));
    ck_assert_ptr_ne(tw_minhash_copy(b, c), NULL);
    ck_assert(tw_minhash_equal(a, c));

    tw_minhash_free(c);
    tw_minhash_free(b);
    tw_minhash_free(a);
  }
}
END_TEST

START_TEST(test_minhash_merge)
{
  DESCRIBE_TEST;

  const uint32_t sizes[] = {16,  32,   64,   128,  256,
                            512, 1024, 2048, 4096, 1 << 13};

  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    const uint32_t n_registers = sizes[i];
    struct tw_minhash *a = tw_minhash_new(n_registers);
    struct tw_minhash *b = tw_minhash_new(n_registers);
    struct tw_minhash *f = tw_minhash_new(n_registers);

    const uint32_t n_items = n_registers * 4;
    for (size_t j = 0; j < n_items; ++j) {
      const size_t key_size = sizeof(j);
      const void *key = (void *)&j;
      tw_minhash_add(((j % 2) ? a : b), key, key_size);
      tw_minhash_add(f, key, key_size);
    }

    ck_assert(estimate_in_bounds(n_registers, 0, tw_minhash_estimate(a, b)));
    ck_assert(estimate_in_bounds(n_registers, 0.5, tw_minhash_estimate(a, f)));
    ck_assert(estimate_in_bounds(n_registers, 0.5, tw_minhash_estimate(b, f)));

    struct tw_minhash *u = tw_minhash_clone(a);
    ck_assert_ptr_ne(tw_minhash_merge(b, u), NULL);

    for (size_t j = 0; j < n_registers; j++) {
      bool same_value =
          tw_max(a->registers[j], b->registers[j]) == u->registers[j];
      ck_assert(same_value);
    }

    ck_assert(estimate_in_bounds(n_registers, 0.5, tw_minhash_estimate(a, u)));
    ck_assert(estimate_in_bounds(n_registers, 0.5, tw_minhash_estimate(b, u)));
    ck_assert(estimate_in_bounds(n_registers, 1.0, tw_minhash_estimate(f, u)));

    tw_minhash_free(u);
    tw_minhash_free(f);
    tw_minhash_free(b);
    tw_minhash_free(a);
  }
}
END_TEST

int run_tests()
{
  int number_failed;

  Suite *s = suite_create("minhash");
  SRunner *runner = srunner_create(s);
  TCase *tc = tcase_create("basic");
  tcase_add_test(tc, test_minhash_basic);
  tcase_add_test(tc, test_minhash_copy_and_clone);
  tcase_add_test(tc, test_minhash_merge);
  /* added for travis slowness of clang */
  tcase_set_timeout(tc, 15);
  suite_add_tcase(s, tc);
  srunner_run_all(runner, CK_NORMAL);
  number_failed = srunner_ntests_failed(runner);
  srunner_free(runner);

  return number_failed;
}

int main() { return (run_tests() == 0) ? EXIT_SUCCESS : EXIT_FAILURE; }
