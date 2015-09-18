#include <stdlib.h>

#include <check.h>

#include <libtwiddle/bitmap_rle.h>
#include <libtwiddle/internal/utils.h>

#include "include/helpers.h"

static const int32_t sizes[] = {32, 64, 128, 256, 512, 1024, 2048, 4096, 32768};
static const int32_t offsets[] = {-1, 0, 1};

START_TEST(test_bitmap_rle_basic)
{
  DESCRIBE_TEST;
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

START_TEST(test_bitmap_rle_range)
{
  DESCRIBE_TEST;
  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const int32_t nbits = sizes[i] + offsets[j];
      struct tw_bitmap_rle *bitmap = tw_bitmap_rle_new(nbits);

      struct tw_bitmap_rle_word word = {.pos = 0, .count = nbits/2 };
      tw_bitmap_rle_set_word(bitmap, &word);
      tw_bitmap_rle_set_range(bitmap, nbits/2 + 1, nbits - 1);

      ck_assert(tw_bitmap_rle_test(bitmap, 0));
      ck_assert(tw_bitmap_rle_test(bitmap, nbits/2 - 1));
      ck_assert(tw_bitmap_rle_test(bitmap, nbits/2 + 1));
      ck_assert(tw_bitmap_rle_test(bitmap, nbits - 1));

      tw_bitmap_rle_free(bitmap);
    }
  }
}
END_TEST

START_TEST(test_bitmap_rle_copy_and_clone)
{
  DESCRIBE_TEST;
  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const int32_t nbits = sizes[i] + offsets[j];
      struct tw_bitmap_rle *src = tw_bitmap_rle_new(nbits);
      struct tw_bitmap_rle *dst = tw_bitmap_rle_new(nbits);

      struct tw_bitmap_rle_word word = {.pos = 0, .count = nbits/2 };
      tw_bitmap_rle_set_word(src, &word);
      tw_bitmap_rle_set_range(src, nbits/2 + 1, nbits - 1);

      ck_assert(tw_bitmap_rle_copy(src, dst) != NULL);

      /* free original to catch potential dangling pointers */
      tw_bitmap_rle_free(src);

      struct tw_bitmap_rle *tmp = tw_bitmap_rle_clone(dst);

      const uint32_t positions[] = {0, nbits/2 - 1, nbits/2 + 1, nbits - 1};
      for (size_t k = 0; k < TW_ARRAY_SIZE(positions); ++k) {
        const uint32_t pos = positions[k];
        ck_assert(tw_bitmap_rle_test(dst, pos));
        ck_assert(tw_bitmap_rle_test(tmp, pos));
      }

      tw_bitmap_rle_free(tmp);
      tw_bitmap_rle_free(dst);
    }
  }
}
END_TEST

/**
 * This test is not fully representative because tw_bitmap_rle should be
 * constructed in an iterative way and then immutable. Thus swapping from
 * fill/zero/fill breaks this assumption.
 */
START_TEST(test_bitmap_rle_zero_and_fill)
{
  DESCRIBE_TEST;
  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const int32_t nbits = sizes[i] + offsets[j];
      struct tw_bitmap_rle *bitmap= tw_bitmap_rle_new(nbits);

      ck_assert(tw_bitmap_rle_empty(bitmap));
      ck_assert(tw_bitmap_rle_density(bitmap) == 0.0);
      ck_assert(!tw_bitmap_rle_full(bitmap));

      tw_bitmap_rle_fill(bitmap);

      const uint32_t positions[] = {0, nbits/2 - 1, nbits/2 + 1, nbits - 1};
      for (size_t k = 0; k < TW_ARRAY_SIZE(positions); ++k) {
        ck_assert(tw_bitmap_rle_test(bitmap, positions[k]));
      }

      ck_assert(tw_bitmap_rle_full(bitmap));
      ck_assert(tw_bitmap_rle_density(bitmap) == 1.0);
      ck_assert(!tw_bitmap_rle_empty(bitmap));

      tw_bitmap_rle_zero(bitmap);

      for (size_t k = 0; k < TW_ARRAY_SIZE(positions); ++k) {
        ck_assert(!tw_bitmap_rle_test(bitmap, positions[k]));
      }

      ck_assert(tw_bitmap_rle_empty(bitmap));
      ck_assert(!tw_bitmap_rle_full(bitmap));

      tw_bitmap_rle_set_range(bitmap, nbits/2 + 1, nbits - 1);
      ck_assert(tw_bitmap_rle_test(bitmap, nbits - 1));
      ck_assert(!tw_bitmap_rle_empty(bitmap));
      ck_assert(!tw_bitmap_rle_full(bitmap));

      tw_bitmap_rle_free(bitmap);
    }
  }
}
END_TEST

START_TEST(test_bitmap_rle_find_first)
{
  DESCRIBE_TEST;
  for (size_t i = 0; i < TW_ARRAY_SIZE(sizes); ++i) {
    for (size_t j = 0; j < TW_ARRAY_SIZE(offsets); ++j) {
      const int32_t nbits = sizes[i] + offsets[j];
      struct tw_bitmap_rle *bitmap = tw_bitmap_rle_new(nbits);

      ck_assert(tw_bitmap_rle_find_first_zero(bitmap) == 0);
      ck_assert(tw_bitmap_rle_find_first_bit(bitmap) == -1);

      tw_bitmap_rle_set(bitmap, 0);
      ck_assert(tw_bitmap_rle_find_first_zero(bitmap) == 1);
      ck_assert(tw_bitmap_rle_find_first_bit(bitmap) == 0);

      tw_bitmap_rle_set(bitmap, 1);
      ck_assert(tw_bitmap_rle_find_first_zero(bitmap) == 2);
      ck_assert(tw_bitmap_rle_find_first_bit(bitmap) == 0);

      tw_bitmap_rle_set(bitmap, 3);
      ck_assert(tw_bitmap_rle_find_first_zero(bitmap) == 2);
      ck_assert(tw_bitmap_rle_find_first_bit(bitmap) == 0);

      tw_bitmap_rle_fill(bitmap);
      ck_assert(tw_bitmap_rle_find_first_zero(bitmap) == -1);
      ck_assert(tw_bitmap_rle_find_first_bit(bitmap) == 0);

      tw_bitmap_rle_zero(bitmap);
      ck_assert(tw_bitmap_rle_find_first_zero(bitmap) == 0);
      ck_assert(tw_bitmap_rle_find_first_bit(bitmap) == -1);

      tw_bitmap_rle_set(bitmap, nbits - 1);
      ck_assert(tw_bitmap_rle_find_first_zero(bitmap) == 0);
      ck_assert(tw_bitmap_rle_find_first_bit(bitmap) == nbits - 1);

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
  tcase_add_test(tc, test_bitmap_rle_range);
  tcase_add_test(tc, test_bitmap_rle_copy_and_clone);
  tcase_add_test(tc, test_bitmap_rle_zero_and_fill);
  tcase_add_test(tc, test_bitmap_rle_find_first);
  tcase_set_timeout(tc, 15);
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
