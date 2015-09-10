#include <assert.h>

#include <libtwiddle/bitmap.h>
#include <libtwiddle/bloomfilter.h>
#include <libtwiddle/hash.h>

#define TW_BF_SEED1 0xDEADBEEF
#define TW_BF_SEED2 0xCAFECAFE

struct tw_bloomfilter *
tw_bloomfilter_new(uint32_t size, uint32_t k)
{
  assert(size > 0 && k > 0);
  struct tw_bloomfilter *bf = calloc(1, sizeof(struct tw_bloomfilter));
  if(!bf)
    return NULL;

  bf->bitmap = tw_bitmap_new(size);
  if(!(bf->bitmap)) {
    free(bf);
    return NULL;
  }

  bf->info = tw_bloomfilter_info_init(size, k);

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

  if (dst->info.size != src->info.size)
    return NULL;

  tw_bloomfilter_info_copy(src->info, dst->info);
  if (!tw_bitmap_copy(src->bitmap, dst->bitmap))
    return NULL;

  return dst;
}

void
tw_bloomfilter_set(struct tw_bloomfilter *bf,
                   size_t size, const char* buf)
{
  assert(bf && size > 0 && buf);
  uint32_t hash1, hash2;
  murmur3_x86_32(buf, size, TW_BF_SEED1, &hash1);
  murmur3_x86_32(buf, size, TW_BF_SEED2, &hash2);

  const uint32_t k = bf->info.k;
  struct tw_bitmap *bitmap = bf->bitmap;
  const uint32_t b_size = bitmap->info.size;

  for (int i = 0; i < k; ++i) {
    tw_bitmap_set(bitmap, (hash1+i*hash2) % b_size);
  }
}

bool
tw_bloomfilter_test(const struct tw_bloomfilter *bf,
                    size_t size, const char* buf)
{
  assert(bf && size > 0 && buf);
  uint32_t hash1, hash2;
  murmur3_x86_32(buf, size, TW_BF_SEED1, &hash1);
  murmur3_x86_32(buf, size, TW_BF_SEED2, &hash2);

  const uint32_t k = bf->info.k;
  const struct tw_bitmap *bitmap = bf->bitmap;
  const uint32_t b_size = bitmap->info.size;

  for (int i = 0; i < k; ++i) {
    if(!tw_bitmap_test(bitmap, (hash1+i*hash2) % b_size))
      return false;
  }

  return true;
}
