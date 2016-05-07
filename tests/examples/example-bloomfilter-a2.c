#include <assert.h>
#include <string.h>

#include <twiddle/bloomfilter/bloomfilter_a2.h>

int main()
{
  const uint64_t nbits = 1024;
  const uint16_t k = 7;
  const float density = 0.50;
  struct tw_bloomfilter_a2 *bf = tw_bloomfilter_a2_new(nbits, k, density);
  assert(bf);

  /**
   * An active-active -bloomfilter (a2-bloomfilter) is a pair of bloomfilters,
   * one active, one passive that gets rotated when
   * active.density >= density_thresold is reached. Before rotation the passive
   * bloomfilter gets cleared.
   *
   * Since density is applied localy to the active bloom filter, the whole
   * bloomfilter density is maximised by 2 * density.
   *
   * Thus an a2-bloomfilter a bloomfilter with a LRU (on write operation)
   * eviction policy.
   */

  for (size_t i = 0; i < nbits * 10; ++i) {
    tw_bloomfilter_a2_set(bf, (void *)&i, sizeof(i));
    assert(tw_bloomfilter_a2_density(bf) < 2 * density);
  }

  tw_bloomfilter_a2_free(bf);

  return 0;
}
