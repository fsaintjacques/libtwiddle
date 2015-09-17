#include <stdlib.h>

#include <check.h>

#include <libtwiddle/bitmap_rle.h>
#include <libtwiddle/internal/utils.h>

#include "include/helpers.h"

START_TEST(test_bitmap_rle_basic)
{
  DESCRIBE_TEST;

  const int32_t sizes[] = {32, 64, 128, 256, 512, 1024, 2048, 4096, 1 << 16};
  const int32_t offsets[] = {-1, 0, 1};

  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const int32_t nbits = sizes[i] + offsets[j];
      struct tw_bitmap_rle *bitmap = tw_bitmap_rle_new(nbits);

      for (uint32_t pos = 0; pos < nbits; ++pos) {
        if (pos % 4)
          tw_bitmap_rle_set(bitmap, pos);
      }

      for (uint32_t pos = 0; pos < nbits; ++pos) {
        if (pos % 4)
          ck_assert(tw_bitmap_rle_test(bitmap, pos));
        else
          ck_assert(!tw_bitmap_rle_test(bitmap, pos));
      }

      tw_bitmap_rle_free(bitmap);
    }
  }

}
END_TEST

int run_tests() {
  int number_failed;

  Suite  *s = suite_create("bitmap-rle");
  SRunner *runner = srunner_create(s);
  TCase *tc = tcase_create("basic");
  tcase_add_test(tc, test_bitmap_rle_basic);
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
