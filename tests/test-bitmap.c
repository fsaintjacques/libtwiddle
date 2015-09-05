#include <stdlib.h>

#include <check.h>

#include <libtwiddle/bitmap.h>
#include <libtwiddle/internal/utils.h>

#include "helpers.h"

static void
validate_bitmap(struct tw_bitmap *bitmap, uint32_t nbits)
{

  ck_assert_msg(tw_bitmap_empty(bitmap), "A new bitmap should be empty");

  for (uint32_t pos = 0; pos < nbits; ++pos) {
    ck_assert_msg(!tw_bitmap_test(bitmap, pos),
                  "Unexpected bit from freshly initialized bitmap at pos: %d",
                  pos);
    tw_bitmap_set(bitmap, pos);
  }

  for (uint32_t pos = 0; pos < nbits; ++pos) {
    ck_assert_msg(tw_bitmap_test(bitmap, (nbits - 1) - pos),
                  "Unexpected zero at pos: %d", (nbits - 1) - pos);
  }

  ck_assert_msg(tw_bitmap_full(bitmap), "A full bitmap should be full");

  for (uint32_t pos = 0; pos < nbits; ++pos) {
    ck_assert_msg(tw_bitmap_test(bitmap, pos),
                  "Unexpected zero from freshly populated bitmap at pos: %d",
                  pos);
    tw_bitmap_clear(bitmap, pos);
    ck_assert_msg(!tw_bitmap_test(bitmap, pos),
                  "Unexpected cleared bit at pos: %d", pos);
  }

  ck_assert_msg(tw_bitmap_empty(bitmap), "A cleared bitmap should be empty");

}

START_TEST(test_bitmap_basic)
{
  DESCRIBE_TEST;

  const int32_t sizes[] = {32, 64, 128, 256, 512, 1024, 2048, 4096, 1 << 17};
  const int32_t offsets[] = {-1, 0, 1};

  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const int32_t nbits = sizes[i] + offsets[j];
      struct tw_bitmap *bitmap = tw_bitmap_new(nbits);
      validate_bitmap(bitmap, nbits);
      tw_bitmap_free(bitmap);
    }
  }

}
END_TEST

START_TEST(test_bitmap_zero_and_fill)
{
  DESCRIBE_TEST;

  const int32_t sizes[] = {32, 64, 128, 256, 512, 1024, 2048, 4096};
  const int32_t offsets[] = {-1, 0, 1};

  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const int32_t nbits = sizes[i] + offsets[j];
      struct tw_bitmap *bitmap = tw_bitmap_new(nbits);

      ck_assert(tw_bitmap_empty(bitmap));
      ck_assert(!tw_bitmap_full(bitmap));

      tw_bitmap_fill(bitmap);

      for (uint32_t pos = 0; pos < nbits; ++pos)
        ck_assert(tw_bitmap_test(bitmap, pos));

      ck_assert(tw_bitmap_full(bitmap));
      ck_assert(!tw_bitmap_empty(bitmap));

      tw_bitmap_zero(bitmap);

      for (uint32_t pos = 0; pos < nbits; ++pos)
        ck_assert(!tw_bitmap_test(bitmap, pos));

      ck_assert(tw_bitmap_empty(bitmap));
      ck_assert(!tw_bitmap_full(bitmap));

      tw_bitmap_free(bitmap);
    }
  }


}
END_TEST

int run_tests() {
  int number_failed;

  Suite  *s = suite_create("bitmap");
  SRunner *runner = srunner_create(s);
  TCase *tc = tcase_create("basic");
  tcase_add_test(tc, test_bitmap_basic);
  tcase_add_test(tc, test_bitmap_zero_and_fill);
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
