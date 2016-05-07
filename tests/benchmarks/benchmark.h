#ifndef TESTS_BENCHMARKS_BENCHMARK_H_
#define TESTS_BENCHMARKS_BENCHMARK_H_

#include <assert.h>
#include <stdio.h>

struct benchmark {
  const char *name;

  size_t size;
  /* number of repetitions */
  size_t repeat;
  /* skip the first `size` results for warmup */
  size_t skip;

  void *opaque;

  void (*setup)(struct benchmark *);
  void (*benchmark)(void *);
  void (*teardown)(struct benchmark *);
};

#define BENCHMARK_FIXTURE(bench, b_repeat, b_size, b_setup, b_teardown)        \
  ((struct benchmark){.name = #bench,                                          \
                      .size = (b_size),                                        \
                      .repeat = (b_repeat),                                    \
                      .skip = (size_t)((b_repeat)*0.05),                       \
                      .benchmark = bench,                                      \
                      .setup = b_setup,                                        \
                      .teardown = b_teardown})

#define BENCHMARK(bench, b_repeat, b_size)                                     \
  BENCHMARK_FIXTURE(bench, b_repeat, b_size, NULL, NULL)

void run_benchmark(struct benchmark *b);

void run_benchmarks(struct benchmark *b, size_t size)
{
  assert(b && size);

  for (size_t i = 0; i < size; ++i) {
    run_benchmark(&b[i]);
  }
}

#define RDTSC_START(cycles)                                                    \
  do {                                                                         \
    register uint32_t cyc_high, cyc_low;                                       \
    __asm volatile("cpuid\n\t"                                                 \
                   "rdtsc\n\t"                                                 \
                   "mov %%edx, %0\n\t"                                         \
                   "mov %%eax, %1\n\t"                                         \
                   : "=r"(cyc_high), "=r"(cyc_low)::"%rax", "%rbx", "%rcx",    \
                     "%rdx");                                                  \
    (cycles) = ((uint64_t)cyc_high << 32) | cyc_low;                           \
  } while (0)

#define RDTSC_FINAL(cycles)                                                    \
  do {                                                                         \
    register uint32_t cyc_high, cyc_low;                                       \
    __asm volatile("rdtscp\n\t"                                                \
                   "mov %%edx, %0\n\t"                                         \
                   "mov %%eax, %1\n\t"                                         \
                   "cpuid\n\t"                                                 \
                   : "=r"(cyc_high), "=r"(cyc_low)::"%rax", "%rbx", "%rcx",    \
                     "%rdx");                                                  \
    (cycles) = ((uint64_t)cyc_high << 32) | cyc_low;                           \
  } while (0)

void run_benchmark(struct benchmark *b)
{
  assert(b);

  const size_t repeat = b->repeat;
  const size_t size = b->size;
  const size_t skip = b->skip;
  const char *name = b->name;

  if (b->setup) {
    b->setup(b);
  }

  for (size_t i = 0; i < repeat; i++) {
    uint64_t cycles_start, cycles_final;

    __asm volatile("" ::: /* pretend to clobber */ "memory");

    RDTSC_START(cycles_start);

    b->benchmark(b->opaque);

    RDTSC_FINAL(cycles_final);

    if (i >= skip) {
      printf("%s,%.2F\n", name,
             ((double)(cycles_final - cycles_start) / (double)size));
    }
  }

  if (b->teardown) {
    b->teardown(b);
  }
}

#endif /* TESTS_BENCHMARKS_BENCHMARK_H_ */
