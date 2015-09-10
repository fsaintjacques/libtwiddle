#include <stdlib.h>

#include <check.h>

#include <libtwiddle/bloomfilter.h>

#include "include/helpers.h"

START_TEST(test_bloomfilter_basic)
{
  DESCRIBE_TEST;

  const int32_t sizes[] = {32, 64, 128, 256, 512, 1024, 2048, 4096, 1 << 17};
  const int32_t ks[]    = {1 , 2 , 3  , 4  , 5  , 6   , 7   , 8   , 17};
  const int32_t offsets[] = {-1, 0, 1};

  char *values[] = {"herp", "derp", "ferp", "merp"};

  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const int32_t nbits = sizes[i] + offsets[j];
      const int32_t k = ks[i];
      struct tw_bloomfilter *bf = tw_bloomfilter_new(nbits, k);

      for (size_t j = 0; j < TW_ARRAY_SIZE(values); ++j) {
        char *value = values[j];
        tw_bloomfilter_set(bf, strlen(value), value);
        ck_assert(tw_bloomfilter_test(bf, strlen(value), value));
      }

      /**
       * This is prone to failure and may be removed if causing problem.
       */
      char *not_there = "oups!";
      ck_assert(!tw_bloomfilter_test(bf, strlen(not_there), not_there));

      tw_bloomfilter_free(bf);
    }
  }

}
END_TEST

int run_tests() {
  int number_failed;

  Suite  *s = suite_create("bloomfilter");
  SRunner *runner = srunner_create(s);
  TCase *tc = tcase_create("basic");
  tcase_add_test(tc, test_bloomfilter_basic);
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
