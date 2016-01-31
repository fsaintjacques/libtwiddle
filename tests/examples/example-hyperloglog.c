#include <assert.h>
#include <stdio.h>

#include <twiddle/hyperloglog/hyperloglog.h>

int main(int argc, char *argv[])
{
  const uint8_t precision = 16;
  struct tw_hyperloglog *hll = tw_hyperloglog_new(precision);
  assert(hll);

  const uint32_t n_elems = 10 * (1 << precision);
  for (int i = 0; i < n_elems; ++i) {
    tw_hyperloglog_add(hll, (void *)&i, sizeof(i));
  }

  printf("estimated count: %f, real count: %d\n", tw_hyperloglog_count(hll),
         n_elems);

  return 0;
}
