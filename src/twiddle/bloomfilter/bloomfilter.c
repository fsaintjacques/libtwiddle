#include <stdlib.h>

#include <twiddle/bitmap/bitmap.h>
#include <twiddle/bloomfilter/bloomfilter.h>
#include <twiddle/utils/hash.h>
#include <twiddle/utils/projection.h>

#define TW_BF_DEFAULT_SEED 3781869495ULL

struct tw_bloomfilter *tw_bloomfilter_new(uint64_t size, uint16_t k)
{
  if (!size || size > TW_BITMAP_MAX_BITS || !k) {
    return NULL;
  }

  struct tw_bloomfilter *bf = calloc(1, sizeof(struct tw_bloomfilter));
  if (!bf) {
    return NULL;
  }

  bf->bitmap = tw_bitmap_new(size);
  if (!(bf->bitmap)) {
    free(bf);
    return NULL;
  }

  bf->k = k;

  return bf;
}

void tw_bloomfilter_free(struct tw_bloomfilter *bf)
{
  if (!bf) {
    return;
  }

  tw_bitmap_free(bf->bitmap);
  free(bf);
}

struct tw_bloomfilter *tw_bloomfilter_copy(const struct tw_bloomfilter *src,
                                           struct tw_bloomfilter *dst)
{
  if (!src || !dst || dst->bitmap->size != src->bitmap->size) {
    return NULL;
  }

  dst->k = src->k;

  if (!tw_bitmap_copy(src->bitmap, dst->bitmap)) {
    return NULL;
  }

  return dst;
}

struct tw_bloomfilter *tw_bloomfilter_clone(const struct tw_bloomfilter *bf)
{
  if (!bf) {
    return NULL;
  }

  struct tw_bloomfilter *new = tw_bloomfilter_new(bf->bitmap->size, bf->k);
  if (!new) {
    return NULL;
  }

  return tw_bloomfilter_copy(bf, new);
}

void tw_bloomfilter_set(struct tw_bloomfilter *bf, const void *key,
                        size_t key_size)
{
  if (!bf || !key || !key_size) {
    return;
  }

  const tw_uint128_t hash = tw_metrohash_128(TW_BF_DEFAULT_SEED, key, key_size);
  const uint16_t k = bf->k;
  struct tw_bitmap *bitmap = bf->bitmap;
  const uint64_t b_size = bitmap->size;

  for (size_t i = 0; i < k; ++i) {
    const uint64_t hash_fn_i = hash.h + (i * hash.l);
    const uint64_t idx = tw_projection_mul_64(hash_fn_i, b_size);
    tw_bitmap_set(bitmap, idx);
  }
}

bool tw_bloomfilter_test(const struct tw_bloomfilter *bf, const void *key,
                         size_t key_size)
{
  if (!bf || !key || !key_size) {
    return false;
  }

  const tw_uint128_t hash = tw_metrohash_128(TW_BF_DEFAULT_SEED, key, key_size);

  const uint16_t k = bf->k;
  const struct tw_bitmap *bitmap = bf->bitmap;
  const uint64_t b_size = bitmap->size;

  for (size_t i = 0; i < k; ++i) {
    const uint64_t hash_fn_i = hash.h + (i * hash.l);
    const uint64_t idx = tw_projection_mul_64(hash_fn_i, b_size);
    if (!tw_bitmap_test(bitmap, idx)) {
      return false;
    }
  }

  return true;
}

bool tw_bloomfilter_empty(const struct tw_bloomfilter *bf)
{
  if (!bf) {
    return false;
  }

  return tw_bitmap_empty(bf->bitmap);
}

bool tw_bloomfilter_full(const struct tw_bloomfilter *bf)
{
  if (!bf) {
    return false;
  }

  return tw_bitmap_full(bf->bitmap);
}

uint64_t tw_bloomfilter_count(const struct tw_bloomfilter *bf)
{
  if (!bf) {
    return 0;
  }

  return tw_bitmap_count(bf->bitmap);
}

float tw_bloomfilter_density(const struct tw_bloomfilter *bf)
{
  if (!bf) {
    return 0.0f;
  }

  return tw_bitmap_density(bf->bitmap);
}

struct tw_bloomfilter *tw_bloomfilter_zero(struct tw_bloomfilter *bf)
{
  if (!bf) {
    return NULL;
  }

  return (tw_bitmap_zero(bf->bitmap)) ? bf : NULL;
}

struct tw_bloomfilter *tw_bloomfilter_fill(struct tw_bloomfilter *bf)
{
  if (!bf) {
    return NULL;
  }

  return (tw_bitmap_fill(bf->bitmap)) ? bf : NULL;
}

struct tw_bloomfilter *tw_bloomfilter_not(struct tw_bloomfilter *bf)
{
  if (!bf) {
    return NULL;
  }

  return (tw_bitmap_not(bf->bitmap)) ? bf : NULL;
}

bool tw_bloomfilter_equal(const struct tw_bloomfilter *a,
                          const struct tw_bloomfilter *b)
{
  if (!a || !b) {
    return false;
  }

  return (a->k == b->k) && tw_bitmap_equal(a->bitmap, b->bitmap);
}

struct tw_bloomfilter *tw_bloomfilter_union(const struct tw_bloomfilter *src,
                                            struct tw_bloomfilter *dst)
{
  if (!src || !dst || src->k != dst->k) {
    return false;
  }

  return (tw_bitmap_union(src->bitmap, dst->bitmap)) ? dst : NULL;
}

struct tw_bloomfilter *
tw_bloomfilter_intersection(const struct tw_bloomfilter *src,
                            struct tw_bloomfilter *dst)
{
  if (!src || !dst || src->k != dst->k) {
    return false;
  }

  return (tw_bitmap_intersection(src->bitmap, dst->bitmap)) ? dst : NULL;
}

struct tw_bloomfilter *tw_bloomfilter_xor(const struct tw_bloomfilter *src,
                                          struct tw_bloomfilter *dst)
{
  if (!src || !dst || src->k != dst->k) {
    return false;
  }

  return (tw_bitmap_xor(src->bitmap, dst->bitmap)) ? dst : NULL;
}
