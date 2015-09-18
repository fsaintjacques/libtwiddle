#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <libtwiddle/bitmap_rle.h>
#include <libtwiddle/internal/utils.h>

static __always_inline
struct tw_bitmap_rle_word *
tw_bitmap_rle_word_alloc(struct tw_bitmap_rle *bitmap)
{
  const uint32_t alloc_word = TW_BITMAP_RLE_WORD_PER_CACHELINE * 4;
  const uint32_t alloc_size = alloc_word * sizeof(struct tw_bitmap_rle_word);
  struct tw_bitmap_rle_word *data = calloc(1, alloc_size);

  if (!data) {
    return NULL;
  }

  bitmap->alloc_word = alloc_word;
  bitmap->data = data;

  return data;
}

static __always_inline
struct tw_bitmap_rle_word *
tw_bitmap_rle_word_grow(struct tw_bitmap_rle *bitmap)
{
  const uint32_t alloc_word = bitmap->alloc_word * 2;
  const uint32_t alloc_size = alloc_word * sizeof(struct tw_bitmap_rle_word);
  struct tw_bitmap_rle_word *new_word = realloc(bitmap->data, alloc_size);

  if (!new_word) {
    return NULL;
  }

  bitmap->data = new_word;
  bitmap->alloc_word = alloc_word;

  return new_word;
}

static __always_inline
struct tw_bitmap_rle_word *
tw_bitmap_rle_get_next_word(struct tw_bitmap_rle *bitmap)
{
  if (bitmap->alloc_word == bitmap->cur_word ) {
    if (!tw_bitmap_rle_word_grow(bitmap)) {
      return NULL;
    }
  }

  return &(bitmap->data[++(bitmap->cur_word)]);
}

struct tw_bitmap_rle *
tw_bitmap_rle_new(uint32_t nbits)
{
  struct tw_bitmap_rle *bitmap = calloc(1, sizeof(struct tw_bitmap_rle));
  if (!bitmap)
    return NULL;
  bitmap->info = tw_bitmap_info_init(nbits);

  if (!tw_bitmap_rle_word_alloc(bitmap)) {
    free(bitmap);
    return NULL;
  }

  return bitmap;
}

void
tw_bitmap_rle_free(struct tw_bitmap_rle *bitmap)
{
  free(bitmap->data);
  free(bitmap);
}

struct tw_bitmap_rle *
tw_bitmap_rle_copy(const struct tw_bitmap_rle *src, struct tw_bitmap_rle *dst)
{
  assert(src && dst);

  if (tw_likely(dst->data != NULL))
    free(dst->data);

  dst->info = src->info;
  dst->last_pos = src->last_pos;

  const uint32_t alloc_word = src->alloc_word;
  const uint32_t alloc_size = alloc_word * sizeof(struct tw_bitmap_rle_word);
  dst->cur_word = src->cur_word;
  dst->alloc_word = alloc_word;
  dst->data = calloc(1, alloc_size);

  if (tw_unlikely(!dst->data))
    return NULL;

  memcpy(dst->data, src->data, alloc_size);

  return dst;
}

struct tw_bitmap_rle *
tw_bitmap_rle_clone(const struct tw_bitmap_rle *bitmap)
{
  assert(bitmap);
  struct tw_bitmap_rle *dst = tw_bitmap_rle_new(bitmap->info.size);

  if (tw_unlikely(!dst))
    return NULL;

  return tw_bitmap_rle_copy(bitmap, dst);
}

void
tw_bitmap_rle_set(struct tw_bitmap_rle *bitmap, uint32_t pos)
{
  assert(bitmap &&
         bitmap->last_pos < pos &&
         bitmap->info.count < UINT32_MAX);

  const uint32_t gap = pos - bitmap->last_pos;
  struct tw_bitmap_rle_word *word = &bitmap->data[bitmap->cur_word];

  if (bitmap->info.count == 0) {
    word->pos = pos;
    word->count = 1;
  } else if (gap == 1) {
      word->count++;
  } else {
    word = tw_bitmap_rle_get_next_word(bitmap);
    word->pos = pos;
    word->count = 1;
  }

  bitmap->info.count++;
  bitmap->last_pos = pos;
}

void
tw_bitmap_rle_set_word(struct tw_bitmap_rle *bitmap,
                       const struct tw_bitmap_rle_word *word)
{
  assert(bitmap && word &&
         (!(bitmap->last_pos) || bitmap->last_pos < word->pos) &&
         (bitmap->info.count + word->count) < UINT32_MAX);

  const uint32_t gap = word->pos - bitmap->last_pos;
  struct tw_bitmap_rle_word *cur_word = &bitmap->data[bitmap->cur_word];

  if (bitmap->info.count == 0) {
    cur_word->pos = word->pos;
    cur_word->count = word->count;
  } else if (gap == 1) {
    cur_word->count += word-> count;
  } else {
    cur_word = tw_bitmap_rle_get_next_word(bitmap);
    cur_word->pos = word->pos;
    cur_word->count = word->count;
  }

  bitmap->info.count += word->count;
  bitmap->last_pos = (word->pos + word->count - 1);
}

void
tw_bitmap_rle_set_range(struct tw_bitmap_rle *bitmap,
                        uint32_t start,
                        uint32_t end)
{
  assert(bitmap);
  const struct tw_bitmap_rle_word word = { .pos = start,
                                           .count = end - start + 1 };
  tw_bitmap_rle_set_word(bitmap, &word);
}

bool
tw_bitmap_rle_test(const struct tw_bitmap_rle *bitmap, uint32_t pos)
{
  assert(bitmap);
  if (bitmap->last_pos < pos || (bitmap->info.count == 0)) {
  /**                           ^^^^^^^^^^^^^^^^^^^^^^^^^
   *                            Requires a special handling because
   *                            tw_bitmap_rle_zero() may trick the following
   *                            loop.
   */
    return false;
  }

  const uint32_t cur_word = bitmap->cur_word;
  for (uint32_t i = 0; i <= cur_word; ++i) {
    /**                  ^^
     * The inclusive equality is important because the current word is valid
     */
    const struct tw_bitmap_rle_word word = bitmap->data[i];
    if (word.pos <= pos && pos <= word.pos + word.count - 1)
      return true;
  }

  return false;
}

bool
tw_bitmap_rle_empty(const struct tw_bitmap_rle *bitmap)
{
  assert(bitmap);
  return tw_bitmap_info_empty(bitmap->info);
}

bool
tw_bitmap_rle_full(const struct tw_bitmap_rle *bitmap)
{
  assert(bitmap);
  return tw_bitmap_info_full(bitmap->info);
}

uint32_t
tw_bitmap_rle_count(const struct tw_bitmap_rle *bitmap)
{
  assert(bitmap);
  return tw_bitmap_info_count(bitmap->info);
}

float
tw_bitmap_rle_density(const struct tw_bitmap_rle *bitmap)
{
  assert(bitmap);
  return tw_bitmap_info_density(bitmap->info);
}

struct tw_bitmap_rle *
tw_bitmap_rle_zero(struct tw_bitmap_rle *bitmap)
{
  assert(bitmap);
  const uint32_t cur_word = bitmap->cur_word;
  for (uint32_t i = 0; i <= cur_word; ++i) {
    bitmap->data[i] = tw_bitmap_rle_word_zero;
  }

  bitmap->cur_word = 0U;
  bitmap->info.count = 0U;
  bitmap->last_pos = 0U;

  return bitmap;
}

struct tw_bitmap_rle *
tw_bitmap_rle_fill(struct tw_bitmap_rle *bitmap)
{
  assert(bitmap);
  const uint32_t cur_word = bitmap->cur_word;
  for (uint32_t i = 0; i <= cur_word; ++i) {
    bitmap->data[i] = tw_bitmap_rle_word_zero;
  }

  const uint32_t size = bitmap->info.size;
  bitmap->data[0] = tw_bitmap_rle_word_full(size);
  bitmap->cur_word = 0U;
  bitmap->info.count = size;
  bitmap->last_pos = size - 1;

  return bitmap;
}
