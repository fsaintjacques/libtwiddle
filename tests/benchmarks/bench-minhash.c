#include <stdlib.h>

#include <twiddle/hash/minhash.h>

#include "benchmark.h"

void minhash_setup(struct benchmark *b)
{

  b->opaque = (void *)tw_minhash_new(b->size);
}

void minhash_teardown(struct benchmark *b)
{
  tw_minhash_free(b->opaque);
  b->opaque = NULL;
}

void minhash_add(void *opaque)
{
  struct tw_minhash *h = (struct tw_minhash *)opaque;

  for (size_t i = 0; i < 10000; i++)
    tw_minhash_add(h, &i, sizeof(i));
}

void minhash_merge(void *opaque)
{
  struct tw_minhash *h = (struct tw_minhash *)opaque;

  for (size_t i = 0; i < 10000; i++)
    tw_minhash_merge(h, h);
}

void minhash_est(void *opaque)
{
  struct tw_minhash *h = (struct tw_minhash *)opaque;

  for (size_t i = 0; i < 10000; i++)
    tw_minhash_estimate(h, h);
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
      BENCHMARK_FIXTURE(minhash_add, repeat, size, minhash_setup,
                        minhash_teardown),
      BENCHMARK_FIXTURE(minhash_est, repeat, size, minhash_setup,
                        minhash_teardown),
      BENCHMARK_FIXTURE(minhash_merge, repeat, size, minhash_setup,
                        minhash_teardown)};

  run_benchmarks(benchmarks, sizeof(benchmarks) / sizeof(benchmarks[0]));

  return EXIT_SUCCESS;
}
