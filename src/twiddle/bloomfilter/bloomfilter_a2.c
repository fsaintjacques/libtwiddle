#include <assert.h>

#include <twiddle/internal/utils.h>
#include <twiddle/bloomfilter/bloomfilter_a2.h>

struct tw_bloomfilter_a2 *tw_bloomfilter_a2_new(uint64_t size, uint16_t k,
                                                float density)
{
  assert(size > 0 && size <= TW_BITMAP_MAX_BITS && k > 0 && 0 < density &&
         density <= 1);

  struct tw_bloomfilter_a2 *bf = calloc(1, sizeof(struct tw_bloomfilter_a2));

  struct tw_bloomfilter *active = tw_bloomfilter_new(size, k);
  if (!active) {
    free(bf);
    return NULL;
  }

  struct tw_bloomfilter *passive = tw_bloomfilter_new(size, k);
  if (!passive) {
    free(bf);
    free(active);
    return NULL;
  }

  bf->density = density;
  bf->active = active;
  bf->passive = passive;

  return bf;
}

struct tw_bloomfilter_a2 *
tw_bloomfilter_a2_copy(const struct tw_bloomfilter_a2 *src,
                       struct tw_bloomfilter_a2 *dst)
{
  assert(src && dst);

  if (!tw_bloomfilter_copy(src->active, dst->active) ||
      !tw_bloomfilter_copy(src->passive, dst->passive)) {
    return NULL;
  }

  return dst;
}

struct tw_bloomfilter_a2 *
tw_bloomfilter_a2_clone(const struct tw_bloomfilter_a2 *bf)
{
  struct tw_bloomfilter_a2 *new = tw_bloomfilter_a2_new(
      bf->active->bitmap->size, bf->active->info.k, bf->density);
  if (!new) {
    return NULL;
  }

  return tw_bloomfilter_a2_copy(bf, new);
}

void tw_bloomfilter_a2_free(struct tw_bloomfilter_a2 *bf)
{
  assert(bf);

  tw_bloomfilter_free(bf->active);
  tw_bloomfilter_free(bf->passive);
  free(bf);
}

static inline bool tw_bloomfilter_a2_rotate_(struct tw_bloomfilter_a2 *bf)
{
  if (tw_unlikely(tw_bloomfilter_density(bf->active) >= bf->density)) {
    struct tw_bloomfilter *tmp = bf->passive;
    bf->passive = bf->active;
    bf->active = tmp;
    tw_bloomfilter_zero(tmp);
    return true;
  }

  return false;
}

void tw_bloomfilter_a2_set(struct tw_bloomfilter_a2 *bf, const void *key,
                           size_t key_size)
{
  assert(bf && key && key_size > 0);

  tw_bloomfilter_a2_rotate_(bf);

  tw_bloomfilter_set(bf->active, key, key_size);
}

bool tw_bloomfilter_a2_test(const struct tw_bloomfilter_a2 *bf, const void *key,
                            size_t key_size)
{
  assert(bf && key && key_size > 0);

  return tw_bloomfilter_test(bf->active, key, key_size) ||
         tw_bloomfilter_test(bf->passive, key, key_size);
}

bool tw_bloomfilter_a2_empty(const struct tw_bloomfilter_a2 *bf)
{
  assert(bf);

  return tw_bloomfilter_empty(bf->active) && tw_bloomfilter_empty(bf->passive);
}

bool tw_bloomfilter_a2_full(const struct tw_bloomfilter_a2 *bf)
{
  assert(bf);

  return tw_bloomfilter_full(bf->active) && tw_bloomfilter_full(bf->passive);
}

uint64_t tw_bloomfilter_a2_count(const struct tw_bloomfilter_a2 *bf)
{
  assert(bf);

  return tw_bloomfilter_count(bf->active) + tw_bloomfilter_count(bf->passive);
}

float tw_bloomfilter_a2_density(const struct tw_bloomfilter_a2 *bf)
{
  assert(bf);

  return (tw_bloomfilter_density(bf->active) +
          tw_bloomfilter_density(bf->passive)) /
         2.0;
}

struct tw_bloomfilter_a2 *tw_bloomfilter_a2_zero(struct tw_bloomfilter_a2 *bf)
{
  assert(bf);

  return (tw_bloomfilter_zero(bf->active) && tw_bloomfilter_zero(bf->passive))
             ? bf
             : NULL;
}

struct tw_bloomfilter_a2 *tw_bloomfilter_a2_fill(struct tw_bloomfilter_a2 *bf)
{
  assert(bf);

  return (tw_bloomfilter_fill(bf->active) && tw_bloomfilter_fill(bf->passive))
             ? bf
             : NULL;
}

struct tw_bloomfilter_a2 *tw_bloomfilter_a2_not(struct tw_bloomfilter_a2 *bf)
{
  assert(bf);

  return (tw_bloomfilter_not(bf->active) && tw_bloomfilter_not(bf->passive))
             ? bf
             : NULL;
}

bool tw_bloomfilter_a2_equal(const struct tw_bloomfilter_a2 *a,
                             const struct tw_bloomfilter_a2 *b)
{
  assert(a && b);

  return (tw_almost_equal(a->density, b->density) &&
          tw_bloomfilter_equal(a->active, b->active) &&
          tw_bloomfilter_equal(a->passive, b->passive));
}

struct tw_bloomfilter_a2 *
tw_bloomfilter_a2_union(const struct tw_bloomfilter_a2 *src,
                        struct tw_bloomfilter_a2 *dst)
{
  assert(src && dst);

  if (!tw_almost_equal(src->density, dst->density)) {
    return NULL;
  }

  return (tw_bloomfilter_union(src->active, dst->active) &&
          tw_bloomfilter_union(src->passive, dst->passive))
             ? dst
             : NULL;
}

struct tw_bloomfilter_a2 *
tw_bloomfilter_a2_intersection(const struct tw_bloomfilter_a2 *src,
                               struct tw_bloomfilter_a2 *dst)
{
  assert(src && dst);

  if (!tw_almost_equal(src->density, dst->density)) {
    return NULL;
  }

  return (tw_bloomfilter_intersection(src->active, dst->active) &&
          tw_bloomfilter_intersection(src->passive, dst->passive))
             ? dst
             : NULL;
}

struct tw_bloomfilter_a2 *
tw_bloomfilter_a2_xor(const struct tw_bloomfilter_a2 *src,
                      struct tw_bloomfilter_a2 *dst)
{
  assert(src && dst);

  if (!tw_almost_equal(src->density, dst->density)) {
    return NULL;
  }

  return (tw_bloomfilter_xor(src->active, dst->active) &&
          tw_bloomfilter_xor(src->passive, dst->passive))
             ? dst
             : NULL;
}
