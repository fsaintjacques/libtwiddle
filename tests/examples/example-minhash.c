#include <assert.h>
#include <stdio.h>

#include <twiddle/hash/minhash.h>

int main(int argc, char *argv[])
{
  const uint32_t n_registers = 1 << 13;
  struct tw_minhash *a = tw_minhash_new(n_registers);
  assert(a);
  struct tw_minhash *b = tw_minhash_clone(a);
  assert(b);

  const uint32_t n_elems = 10 * n_registers;
  for (int i = 0; i < n_elems; ++i) {
    if (i % 3 == 0) {
      tw_minhash_add(a, sizeof(i), (char *)&i);
    }

    if (i % 5 == 0) {
      tw_minhash_add(b, sizeof(i), (char *)&i);
    }
  }

  printf("estimated jaccard: %f\n", tw_minhash_estimate(a, b));

  tw_minhash_free(b);
  tw_minhash_free(a);

  return 0;
}
