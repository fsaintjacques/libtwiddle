#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <twiddle/hash/metrohash.h>
#include <twiddle/hyperloglog/hyperbitbit.h>

#define TW_HYPERBITBIT_RANK 5
#define TW_HYPERBITBIT_SEED 0xB17B1700FFFFFFFF
#define TW_HYPERBITBIT_SWAP_THRESH 31
#define TW_HYPERBITBIT_BIAS 5.4

static void tw_hyperbitbit_init(struct tw_hyperbitbit *hbb)
{
  if (!hbb)
    return;

  hbb->current_rank = TW_HYPERBITBIT_RANK;
  hbb->rank_bitmap = 0;
  hbb->next_rank_bitmap = 0;
}

struct tw_hyperbitbit *tw_hyperbitbit_new()
{
  struct tw_hyperbitbit *hbb = calloc(1, sizeof(struct tw_hyperbitbit));
  tw_hyperbitbit_init(hbb);

  return hbb;
}

void tw_hyperbitbit_free(struct tw_hyperbitbit *hbb) { free(hbb); }

struct tw_hyperbitbit *tw_hyperbitbit_copy(const struct tw_hyperbitbit *src,
                                           struct tw_hyperbitbit *dst)
{
  if (!src || !dst)
    return NULL;

  return memcpy(dst, src, sizeof(struct tw_hyperbitbit));
}

struct tw_hyperbitbit *tw_hyperbitbit_clone(const struct tw_hyperbitbit *src)
{
  struct tw_hyperbitbit *dst = tw_hyperbitbit_new();
  return tw_hyperbitbit_copy(src, dst);
}

void tw_hyperbitbit_add(struct tw_hyperbitbit *hbb, const void *key,
                        size_t key_size)
{
  const tw_uint128_t hash =
      tw_metrohash_128(TW_HYPERBITBIT_SEED, key, key_size);
  /* Take the lowest bit for idx */
  const uint64_t bitmap = 1 << (hash.l & 0xFF);
  const uint8_t rank = __builtin_ffsl(hash.h);

  hbb->rank_bitmap |= (rank > hbb->current_rank) ? bitmap : 0ULL;
  hbb->next_rank_bitmap |= (rank > (hbb->current_rank + 1)) ? bitmap : 0ULL;

  if (__builtin_popcountl(hbb->rank_bitmap) > TW_HYPERBITBIT_SWAP_THRESH) {
    hbb->current_rank++;
    hbb->rank_bitmap = hbb->next_rank_bitmap;
    hbb->next_rank_bitmap = 0;
  }
}

double tw_hyperbitbit_count(const struct tw_hyperbitbit *hbb)
{
  if (!hbb)
    return 0.0;

  const double rank = hbb->current_rank,
               density = (__builtin_popcountl(hbb->rank_bitmap) / 32.0);

  return pow(2.0, TW_HYPERBITBIT_BIAS + rank + density);
}

bool tw_hyperbitbit_equal(const struct tw_hyperbitbit *fst,
                          const struct tw_hyperbitbit *snd)
{
  if (!fst || !snd)
    return false;

  return memcmp(fst, snd, sizeof(struct tw_hyperbitbit)) == 0;
}

struct tw_hyperbitbit *tw_hyperbitbit_merge(const struct tw_hyperbitbit *src,
                                            struct tw_hyperbitbit *dst)
{
  if (!src || !dst)
    return NULL;

  /* 3 cases, give diff = (src->current_rank - dst->current_rank)
   * case +-1: merge current_bitmap with next_bitmap
   * case 0: merges pairwise bitmaps
   * default: take biggest.
   **/

  return dst;
}
