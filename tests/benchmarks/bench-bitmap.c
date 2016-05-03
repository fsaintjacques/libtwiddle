#include <twiddle/bitmap/bitmap.h>

#include "benchmark.h"

struct dual_bitmap {
  struct tw_bitmap *a;
  struct tw_bitmap *b;
};

void bitmap_dual_setup(struct benchmark *b)
{
  const size_t size = b->size * 8;

  b->opaque = malloc(sizeof(struct dual_bitmap));
  struct dual_bitmap *dual = (struct dual_bitmap *)b->opaque;
  assert(dual);

  dual->a = tw_bitmap_new(size);
  assert(dual->a);
  dual->b = tw_bitmap_new(size);
  assert(dual->b);

  for (size_t i = 0; i < size; ++i) {
    if (i % 5) {
      tw_bitmap_set(dual->a, i);
      tw_bitmap_set(dual->b, i);
    }
  }
}

void bitmap_dual_teardown(struct benchmark *b)
{
  struct dual_bitmap *dual = (struct dual_bitmap *)b->opaque;
  tw_bitmap_free(dual->b);
  tw_bitmap_free(dual->a);
  free(dual);
  b->opaque = NULL;
}

void bitmap_xor(void *opaque)
{
  struct dual_bitmap *dual = (struct dual_bitmap *)opaque;

  tw_bitmap_xor(dual->a, dual->b);
}

void bitmap_equal(void *opaque)
{
  struct dual_bitmap *dual = (struct dual_bitmap *)opaque;

  bool res = tw_bitmap_equal(dual->a, dual->b);
  (void)res;
}

int main(int argc, char *argv[])
{

  if (argc != 3) {
    fprintf(stderr, "usage: %s <repeat> <size>\n", argv[0]);
    return EXIT_FAILURE;
  }

  const size_t repeat = strtol(argv[1], NULL, 10);
  const size_t size = strtol(argv[2], NULL, 10);

  struct benchmark benchmarks[] = {
      BENCHMARK_FIXTURE(bitmap_equal, repeat, size, bitmap_dual_setup,
                        bitmap_dual_teardown),
      BENCHMARK_FIXTURE(bitmap_xor, repeat, size, bitmap_dual_setup,
                        bitmap_dual_teardown),
  };

  run_benchmarks(benchmarks, sizeof(benchmarks) / sizeof(benchmarks[0]));

  return EXIT_SUCCESS;
}
