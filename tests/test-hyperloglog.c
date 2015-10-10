#include <math.h>
#include <stdlib.h>

#include <check.h>

#include <twiddle/internal/utils.h>
#include <twiddle/hyperloglog.h>

#include "include/helpers.h"

bool
estimate_within_error(double estimate, double real, double error)
{
  const double diff = abs(estimate - real);
  const double margin = real * error;
  return diff < 3 * margin;
}

START_TEST(test_hyperloglog_basic)
{
  DESCRIBE_TEST;

  const int32_t sizes[] = {128, 256, 512, 1024, 2048, 4096, 1 << 17};
  const int32_t offsets[] = {-1, 0, 1};

  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const int32_t n_buckets = sizes[i] + offsets[j];
      struct tw_hyperloglog *hll = tw_hyperloglog_new(n_buckets);

      double n_elems = 0.0;

      /** test linear_count */
      for (size_t k = 0; k < n_buckets; ++k) {
        if (k % 2) {
          tw_hyperloglog_add(hll, sizeof(k), (const char *) &k);
          n_elems += 1.0;
        }
      }
      bool within_error = estimate_within_error(tw_hyperloglog_estimate(hll),
                                                n_elems,
                                                TW_HLL_ERROR_FOR_REG(n_buckets));
      ck_assert(within_error);

      /** test loglog */
      n_elems = 0;
      for (size_t k = 0; k < 10 * n_buckets; ++k) {
        if (k % 2) {
          tw_hyperloglog_add(hll, sizeof(k), (const char *) &k);
          n_elems += 1.0;
        }
      }
      within_error = estimate_within_error(tw_hyperloglog_estimate(hll),
                                           n_elems,
                                           TW_HLL_ERROR_FOR_REG(n_buckets));
      ck_assert(within_error);
      tw_hyperloglog_free(hll);
    }
  }

}
END_TEST

int run_tests() {
  int number_failed;

  Suite  *s = suite_create("hyperloglog");
  SRunner *runner = srunner_create(s);
  TCase *tc = tcase_create("basic");
  tcase_add_test(tc, test_hyperloglog_basic);
  suite_add_tcase(s, tc);
  srunner_run_all(runner, CK_NORMAL);
  number_failed = srunner_ntests_failed(runner);
  srunner_free(runner);

  return number_failed;
}

int
main(int argc, char* argv[])
{
  return (run_tests() == 0)? EXIT_SUCCESS: EXIT_FAILURE;
}
