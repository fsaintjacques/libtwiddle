#include <assert.h>
#include <string.h>

#include <twiddle/bloomfilter/bloomfilter.h>

int main(int argc, char *argv[])
{
  const uint64_t nbits = 1024;
  const uint16_t k = 7;
  struct tw_bloomfilter *bf = tw_bloomfilter_new(nbits, k);
  assert(bf);

  char *values[] = {"herp", "derp", "ferp", "merp"};

  for (int i = 0; i < ((sizeof(values) / sizeof(values[0]))); ++i) {
    tw_bloomfilter_set(bf, values[i], strlen(values[i]));
    assert(tw_bloomfilter_test(bf, values[i], strlen(values[i])));
  }

  assert(!tw_bloomfilter_test(bf, "nope", sizeof("nope")));

  return 0;
}
