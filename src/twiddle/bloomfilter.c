#include <assert.h>

#include <twiddle/bitmap.h>
#include <twiddle/bloomfilter.h>
#include <twiddle/hash.h>

struct tw_bloomfilter *
tw_bloomfilter_new(uint32_t size, uint32_t k)
{
  assert(size > 0 && k > 0);
  struct tw_bloomfilter *bf = calloc(1, sizeof(struct tw_bloomfilter));
  if (!bf) {
    return NULL;
  }

  bf->bitmap = tw_bitmap_new(size);
  if (!(bf->bitmap)) {
    free(bf);
    return NULL;
  }

  bf->info = tw_bloomfilter_info_init(size, k);
  bf->hash_seed = TW_BF_DEFAULT_SEED;

  return bf;
}

void
tw_bloomfilter_free(struct tw_bloomfilter *bf)
{
  assert(bf);
  tw_bitmap_free(bf->bitmap);
  free(bf);
}

struct tw_bloomfilter *
tw_bloomfilter_copy(const struct tw_bloomfilter *src,
                    struct tw_bloomfilter *dst)
{
  assert(src && dst);

  if (dst->info.size != src->info.size) {
    return NULL;
  }

  tw_bloomfilter_info_copy(src->info, dst->info);
  if (!tw_bitmap_copy(src->bitmap, dst->bitmap)) {
    return NULL;
  }

  return dst;
}

struct tw_bloomfilter *
tw_bloomfilter_clone(const struct tw_bloomfilter *bf)
{
  assert(bf);

  struct tw_bloomfilter *new = tw_bloomfilter_new(bf->info.size, bf->info.k);
  if (!new) {
    return NULL;
  }

  return tw_bloomfilter_copy(bf, new);
}

void
tw_bloomfilter_set(struct tw_bloomfilter *bf,
                   size_t size, const char* buf)
{
  assert(bf && size > 0 && buf);
  uint64_t hash[2];
  murmur3_x64_128(buf, size, bf->hash_seed, hash);

  const uint32_t k = bf->info.k;
  struct tw_bitmap *bitmap = bf->bitmap;
  const uint32_t b_size = bitmap->info.size;

  for (int i = 0; i < k; ++i) {
    tw_bitmap_set(bitmap, (hash[0] + i * hash[0]) % b_size);
  }
}

bool
tw_bloomfilter_test(const struct tw_bloomfilter *bf,
                    size_t size, const char* buf)
{
  assert(bf && size > 0 && buf);
  uint64_t hash[2];
  murmur3_x64_128(buf, size, bf->hash_seed, hash);

  const uint32_t k = bf->info.k;
  const struct tw_bitmap *bitmap = bf->bitmap;
  const uint32_t b_size = bitmap->info.size;

  for (int i = 0; i < k; ++i) {
    if (!tw_bitmap_test(bitmap, (hash[0] + i * hash[0]) % b_size)) {
      return false;
    }
  }

  return true;
}

bool
tw_bloomfilter_empty(const struct tw_bloomfilter *bf)
{
  assert(bf);
  return tw_bitmap_empty(bf->bitmap);
}

bool
tw_bloomfilter_full(const struct tw_bloomfilter *bf)
{
  assert(bf);
  return tw_bitmap_full(bf->bitmap);
}

uint32_t
tw_bloomfilter_count(const struct tw_bloomfilter *bf)
{
  assert(bf);
  return tw_bitmap_count(bf->bitmap);
}

float
tw_bloomfilter_density(const struct tw_bloomfilter *bf)
{
  assert(bf);
  return tw_bitmap_density(bf->bitmap);
}

struct tw_bloomfilter *
tw_bloomfilter_zero(struct tw_bloomfilter *bf)
{
  assert(bf);
  return (tw_bitmap_zero(bf->bitmap)) ? bf : NULL;
}

struct tw_bloomfilter *
tw_bloomfilter_fill(struct tw_bloomfilter *bf)
{
  assert(bf);
  return (tw_bitmap_fill(bf->bitmap)) ? bf : NULL;
}

struct tw_bloomfilter *
tw_bloomfilter_not(struct tw_bloomfilter *bf)
{
  assert(bf);
  return (tw_bitmap_not(bf->bitmap))? bf : NULL;
}


bool
tw_bloomfilter_equal(const struct tw_bloomfilter *a,
                     const struct tw_bloomfilter *b)
{
  assert(a && b);

  if (!tw_bloomfilter_info_equal(a->info, b->info)) {
    return false;
  }

  return tw_bitmap_equal(a->bitmap, b->bitmap);
}


struct tw_bloomfilter *
tw_bloomfilter_union(const struct tw_bloomfilter *src,
                           struct tw_bloomfilter *dst)
{
  assert(src && dst);

  if (!tw_bloomfilter_info_equal(src->info, dst->info)) {
    return NULL;
  }

  return (tw_bitmap_union(src->bitmap, dst->bitmap)) ? dst : NULL;
}


struct tw_bloomfilter *
tw_bloomfilter_intersection(const struct tw_bloomfilter *src,
                                  struct tw_bloomfilter *dst)
{
  assert(src && dst);

  if (!tw_bloomfilter_info_equal(src->info, dst->info)) {
    return NULL;
  }

  return (tw_bitmap_intersection(src->bitmap, dst->bitmap)) ? dst : NULL;
}


struct tw_bloomfilter *
tw_bloomfilter_xor(const struct tw_bloomfilter *src,
                         struct tw_bloomfilter *dst)
{
  assert(src && dst);

  if (!tw_bloomfilter_info_equal(src->info, dst->info)) {
    return NULL;
  }

  return (tw_bitmap_xor(src->bitmap, dst->bitmap)) ? dst : NULL;
}
