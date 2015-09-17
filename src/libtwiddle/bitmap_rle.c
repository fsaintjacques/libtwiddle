#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <libtwiddle/bitmap_rle.h>
#include <libtwiddle/internal/utils.h>

struct tw_bitmap_rle *
tw_bitmap_rle_new(uint32_t nbits)
{
  struct tw_bitmap_rle *bitmap = calloc(1, sizeof(struct tw_bitmap_rle));
  if (!bitmap)
    return NULL;

  const uint32_t alloc_word = TW_BITMAP_RLE_WORD_PER_CACHELINE * 4;
  const uint32_t alloc_size = alloc_word * sizeof(struct tw_bitmap_rle_word);
  struct tw_bitmap_rle_word *data = calloc(1, alloc_size);
  if (!data) {
    free(bitmap);
    return NULL;
  }

  bitmap->info = tw_bitmap_info_init(nbits);
  bitmap->alloc_word = alloc_word;
  bitmap->data = data;

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
  dst->alloc_word = alloc_word;
  dst->data = calloc(1, alloc_size);

  if (tw_unlikely(!dst->data))
    return NULL;

  memcpy(dst->data, src->data, alloc_size);

  return NULL;
}

void
tw_bitmap_rle_set(struct tw_bitmap_rle *bitmap, uint32_t pos)
{
  uint32_t gap = pos - bitmap->last_pos;

  assert(gap >= 0);

  struct tw_bitmap_rle_word *word = &bitmap->data[bitmap->cur_word];

  if (bitmap->info.count == 0) {
    word->pos = pos;
    word->count = 1;
  } else if (gap == 1 && word->count < UINT32_MAX) {
    word->count++;
  } else {
    word = tw_bitmap_rle_get_next_word(bitmap);
    word->pos = pos;
    word->count = 1;
  }

  bitmap->info.count++;
  bitmap->last_pos = pos;
}

bool
tw_bitmap_rle_test(const struct tw_bitmap_rle *bitmap, uint32_t pos)
{
  if (bitmap->last_pos < pos)
    return false;

  const uint32_t cur_word = bitmap->cur_word;
  /**
   * The inclusive equality is important because the current word is valid!
   */
  for (uint32_t i = 0; i <= cur_word; ++i) {
    struct tw_bitmap_rle_word word = bitmap->data[i];
    if (word.pos <= pos && pos <= word.pos + word.count - 1)
      return true;
  }

  return false;
}
