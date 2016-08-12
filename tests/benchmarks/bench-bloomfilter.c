#include <stdlib.h>

#include <twiddle/bitmap/bitmap.h>
#include <twiddle/bloomfilter/bloomfilter.h>

#include "benchmark.h"

void bloomfilter_setup(struct benchmark *b)
{
  const size_t size = b->size * 8;
  const uint16_t k = 10;

  b->opaque = tw_bloomfilter_new(size, k);
  assert(b->opaque);

  for (size_t i = 0; i < size; ++i) {
    if (i % 3) {
      tw_bloomfilter_set(b->opaque, &i, sizeof(i));
    }
  }
}

void bloomfilter_teardown(struct benchmark *b)
{
  struct tw_bloomfilter *bf = (struct tw_bloomfilter *)b->opaque;
  tw_bloomfilter_free(bf);
  b->opaque = NULL;
}

void bloomfilter_set(void *opaque)
{
  struct tw_bloomfilter *bf = (struct tw_bloomfilter *)opaque;

  const size_t n_rounds = (bf->bitmap->size) / (8 * 128);
  for (size_t i = 0; i < n_rounds; ++i) {
    tw_bloomfilter_set(bf, &i, sizeof(i));
  }
}

void bloomfilter_test(void *opaque)
{
  struct tw_bloomfilter *bf = (struct tw_bloomfilter *)opaque;

  const size_t n_rounds = (bf->bitmap->size) / (8 * 128);
  for (size_t i = 0; i < n_rounds; ++i) {
    tw_bloomfilter_test(bf, &i, sizeof(i));
  }
}

int main(int argc, char *argv[])
{

  if (argc != 3) {
    fprintf(stderr, "usage: %s <repeat> <size>\n", argv[0]);
    return EXIT_FAILURE;
  }

  const size_t repeat = strtol(argv[1], NULL, 10);
  const size_t size = strtol(argv[2], NULL, 10);
  (void)size;

  struct benchmark benchmarks[] = {
      BENCHMARK_FIXTURE(bloomfilter_set, repeat, size, bloomfilter_setup,
                        bloomfilter_teardown),
      BENCHMARK_FIXTURE(bloomfilter_test, repeat, size, bloomfilter_setup,
                        bloomfilter_teardown),
  };

  run_benchmarks(benchmarks, sizeof(benchmarks) / sizeof(benchmarks[0]));

  return EXIT_SUCCESS;
}
