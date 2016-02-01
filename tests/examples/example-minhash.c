#include <assert.h>
#include <stdio.h>

#include <twiddle/hash/minhash.h>

int main()
{
  const uint32_t n_registers = 1 << 13;
  struct tw_minhash *a = tw_minhash_new(n_registers);
  assert(a);
  struct tw_minhash *b = tw_minhash_clone(a);
  assert(b);

  const uint32_t n_elems = 10 * n_registers;
  for (size_t i = 0; i < n_elems; ++i) {
    const size_t key_size = sizeof(i);
    const void *key = (void *)&i;
    if (i % 3 == 0) {
      tw_minhash_add(a, key, key_size);
    }

    if (i % 5 == 0) {
      tw_minhash_add(b, key, key_size);
    }
  }

  printf("estimated jaccard: %f\n", tw_minhash_estimate(a, b));

  tw_minhash_free(b);
  tw_minhash_free(a);

  return 0;
}
