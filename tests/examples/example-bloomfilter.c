#include <assert.h>
#include <string.h>

#include <twiddle/bloomfilter/bloomfilter.h>

int main()
{
  const uint64_t nbits = 1024;
  const uint16_t k = 7;
  struct tw_bloomfilter *bf = tw_bloomfilter_new(nbits, k);
  assert(bf);

  const char *values[] = {"herp", "derp", "ferp", "merp"};

  for (size_t i = 0; i < ((sizeof(values) / sizeof(values[0]))); ++i) {
    tw_bloomfilter_set(bf, values[i], strlen(values[i]));
    assert(tw_bloomfilter_test(bf, values[i], strlen(values[i])));
  }

  assert(!tw_bloomfilter_test(bf, "nope", sizeof("nope")));

  tw_bloomfilter_free(bf);

  return 0;
}
