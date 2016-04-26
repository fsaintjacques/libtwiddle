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

int main()
{
  const size_t n_bits = 1 << 16;

  struct benchmark benchmarks[] = {
      BENCHMARK_FIXTURE(bitmap_xor, 4096, n_bits, bitmap_dual_setup,
                        bitmap_dual_teardown),
  };

  run_benchmarks(benchmarks, 1);

  return EXIT_SUCCESS;
}
