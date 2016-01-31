#include <math.h>
#include <stdlib.h>

#include <check.h>

#include <twiddle/internal/utils.h>
#include <twiddle/hyperloglog/hyperloglog.h>
#include <twiddle/hyperloglog/hyperloglog_simd.h>

#include "include/helpers.h"

bool estimate_within_error(double estimate, double real, double error)
{
  const double diff = fabs(estimate - real);
  const double margin = real * 0.1;
  return diff < 10 * margin;
}

START_TEST(test_hyperloglog_basic)
{
  DESCRIBE_TEST;
  for (uint8_t p = TW_HLL_MIN_PRECISION; p <= TW_HLL_MAX_PRECISION; ++p) {
    const uint32_t n_registers = 1 << p;
    struct tw_hyperloglog *hll = tw_hyperloglog_new(p);
    ck_assert(hll != NULL);

    double n_elems = 0.0;

    /** test linear_count */
    for (size_t k = 0; k < n_registers; ++k) {
      if (k % 2) {
        tw_hyperloglog_add(hll, (void *)&k, sizeof(k));
        n_elems += 1.0;
      }
    }
    bool within_error = estimate_within_error(
        tw_hyperloglog_count(hll), n_elems, TW_HLL_ERROR_FOR_REG(n_registers));
    ck_assert(within_error);

    /** test loglog */
    n_elems = 0;
    for (size_t k = 0; k < 10 * n_registers; ++k) {
      if (k % 2) {
        tw_hyperloglog_add(hll, (void *)&k, sizeof(k));
        n_elems += 1.0;
      }
    }
    within_error = estimate_within_error(tw_hyperloglog_count(hll), n_elems,
                                         TW_HLL_ERROR_FOR_REG(n_registers));
    ck_assert_msg(within_error, "estimate %f not within bounds",
                  tw_hyperloglog_count(hll));
    tw_hyperloglog_free(hll);
  }
}
END_TEST

START_TEST(test_hyperloglog_copy_and_clone)
{
  DESCRIBE_TEST;
  for (uint8_t p = TW_HLL_MIN_PRECISION; p <= TW_HLL_MAX_PRECISION; ++p) {
    const int32_t n_registers = 1 << p;
    struct tw_hyperloglog *hll = tw_hyperloglog_new(p);
    struct tw_hyperloglog *copy = tw_hyperloglog_new(p);

    /** test linear_count */
    for (size_t k = 0; k < n_registers; ++k) {
      if (k % 2) {
        tw_hyperloglog_add(hll, (void *)&k, sizeof(k));
      }
    }

    ck_assert(tw_hyperloglog_copy(hll, copy) != NULL);
    ck_assert(tw_hyperloglog_count(hll) == tw_hyperloglog_count(copy));

    struct tw_hyperloglog *clone = tw_hyperloglog_clone(copy);
    ck_assert(clone != NULL);
    ck_assert(tw_hyperloglog_equal(hll, clone));
    ck_assert(tw_hyperloglog_count(hll) == tw_hyperloglog_count(clone));

    tw_hyperloglog_free(clone);
    tw_hyperloglog_free(copy);
    tw_hyperloglog_free(hll);
  }
}
END_TEST

START_TEST(test_hyperloglog_merge)
{
  DESCRIBE_TEST;
  for (uint8_t p = TW_HLL_MIN_PRECISION; p <= TW_HLL_MAX_PRECISION; ++p) {
    const uint32_t n_registers = 1 << p;
    struct tw_hyperloglog *src = tw_hyperloglog_new(p);
    struct tw_hyperloglog *dst = tw_hyperloglog_new(p);
    struct tw_hyperloglog *prev;

    const int times = 100;
    /** test linear_count */
    for (size_t k = 0; k < times * n_registers; ++k) {
      if (k % 2) {
        tw_hyperloglog_add(src, (void *)&k, sizeof(k));
      } else {
        tw_hyperloglog_add(dst, (void *)&k, sizeof(k));
      }
    }

    prev = tw_hyperloglog_clone(dst);

    ck_assert(tw_hyperloglog_merge(src, dst) != NULL);

    // merge should guarantee size increase
    ck_assert(tw_hyperloglog_count(src) <= tw_hyperloglog_count(dst));
    ck_assert(tw_hyperloglog_count(prev) <= tw_hyperloglog_count(dst));

    double estimate = tw_hyperloglog_count(dst);
    bool within_bound =
        estimate_within_error(estimate, (double)times * n_registers,
                              TW_HLL_ERROR_FOR_REG(n_registers));

    ck_assert_msg(within_bound, "%d not within bounds",
                  tw_hyperloglog_count(dst));

    tw_hyperloglog_free(dst);
    tw_hyperloglog_free(src);
  }
}
END_TEST

START_TEST(test_hyperloglog_simd)
{
  DESCRIBE_TEST;
  for (uint8_t p = TW_HLL_MIN_PRECISION; p <= TW_HLL_MAX_PRECISION; ++p) {
    const uint32_t n_registers = 1 << p;
    struct tw_hyperloglog *hll = tw_hyperloglog_new(p);
    ck_assert(hll != NULL);

    double n_elems = 0.0;

    /** test linear_count */
    for (size_t k = 0; k < n_registers; ++k) {
      if (k % 2) {
        tw_hyperloglog_add(hll, (void *)&k, sizeof(k));
        n_elems += 1.0;
      }
    }
    bool within_error = estimate_within_error(
        tw_hyperloglog_count(hll), n_elems, TW_HLL_ERROR_FOR_REG(n_registers));
    ck_assert(within_error);

    /* Verify that the SIMD implementation computes the same
     * n_zeros/inverse_sum than the naive (correct) version */
    uint32_t n_zeros_1 = 0, n_zeros_2 = 0;
    float sum_1 = 0.0, sum_2 = 0.0;
    hyperloglog_count_port(hll->registers, n_registers, &sum_1, &n_zeros_1);
#ifdef USE_AVX2
    hyperloglog_count_avx2(hll->registers, n_registers, &sum_2, &n_zeros_2);
#elif USE_AVX
    hyperloglog_count_avx(hll->registers, n_registers, &sum_2, &n_zeros_2);
#else
    hyperloglog_count_port(hll->registers, n_registers, &sum_2, &n_zeros_2);
#endif
    ck_assert(n_zeros_1 == n_zeros_2);
    /* float sums is _not_ associative, thus it might differ a bit when using
     * SIMD operations */
    ck_assert(fabs(sum_1 - sum_2) < sum_1 * 0.00001);

    /** test loglog */
    n_elems = 0;
    for (size_t k = 0; k < 10 * n_registers; ++k) {
      if (k % 2) {
        tw_hyperloglog_add(hll, (void *)&k, sizeof(k));
        n_elems += 1.0;
      }
    }
    within_error = estimate_within_error(tw_hyperloglog_count(hll), n_elems,
                                         TW_HLL_ERROR_FOR_REG(n_registers));
    ck_assert_msg(within_error, "estimate %f not within bounds",
                  tw_hyperloglog_count(hll));

    n_zeros_1 = 0, n_zeros_2 = 0;
    sum_1 = 0.0, sum_2 = 0.0;
    hyperloglog_count_port(hll->registers, n_registers, &sum_1, &n_zeros_1);
#ifdef USE_AVX2
    hyperloglog_count_avx2(hll->registers, n_registers, &sum_2, &n_zeros_2);
#elif USE_AVX
    hyperloglog_count_avx(hll->registers, n_registers, &sum_2, &n_zeros_2);
#else
    hyperloglog_count_port(hll->registers, n_registers, &sum_2, &n_zeros_2);
#endif
    ck_assert(n_zeros_1 == n_zeros_2);
    /* float sums is _not_ associative, thus it might differ a bit when using
     * SIMD operations */
    ck_assert(fabs(sum_1 - sum_2) < sum_1 * 0.0001);

    tw_hyperloglog_free(hll);
  }
}
END_TEST

int run_tests()
{
  int number_failed;

  Suite *s = suite_create("hyperloglog");
  SRunner *runner = srunner_create(s);
  TCase *tc = tcase_create("basic");
  tcase_add_test(tc, test_hyperloglog_basic);
  tcase_add_test(tc, test_hyperloglog_copy_and_clone);
  tcase_add_test(tc, test_hyperloglog_merge);
  tcase_add_test(tc, test_hyperloglog_simd);
  tcase_set_timeout(tc, 15);
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
