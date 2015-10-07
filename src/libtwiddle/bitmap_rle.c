#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <libtwiddle/bitmap_rle.h>
#include <libtwiddle/internal/utils.h>

static __always_inline
struct tw_bitmap_rle_word *
tw_bitmap_rle_word_alloc_(struct tw_bitmap_rle *bitmap, uint32_t n_words)
{
  const uint32_t alloc_size = n_words * sizeof(struct tw_bitmap_rle_word);
  struct tw_bitmap_rle_word *data = calloc(1, alloc_size);

  if (!data) {
    return NULL;
  }

  bitmap->alloc_word = n_words;
  bitmap->data = data;

  return data;
}

/* default rle words = 4 cachelines */
#define tw_bitmap_rle_word_alloc(bitmap) \
  tw_bitmap_rle_word_alloc_(bitmap, TW_BITMAP_RLE_WORD_PER_CACHELINE * 4)

static __always_inline
struct tw_bitmap_rle_word *
tw_bitmap_rle_word_grow_(struct tw_bitmap_rle *bitmap, uint32_t words)
{
  const uint32_t alloc_word = bitmap->alloc_word + words;
  const uint32_t alloc_size = alloc_word * sizeof(struct tw_bitmap_rle_word);
  struct tw_bitmap_rle_word *new_word = realloc(bitmap->data, alloc_size);

  if (!new_word) {
    return NULL;
  }

  bitmap->data = new_word;
  bitmap->alloc_word = alloc_word;

  return new_word;
}

/* double size by default */
#define tw_bitmap_rle_word_grow(bitmap) \
  tw_bitmap_rle_word_grow_(bitmap, bitmap->alloc_word)

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
  if (!bitmap) {
    return NULL;
  }
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

  if (tw_likely(dst->data != NULL)) {
    free(dst->data);
  }

  dst->info = src->info;
  dst->last_pos = src->last_pos;

  const uint32_t alloc_word = src->alloc_word;
  const uint32_t alloc_size = alloc_word * sizeof(struct tw_bitmap_rle_word);
  dst->cur_word = src->cur_word;
  dst->alloc_word = alloc_word;
  dst->data = calloc(1, alloc_size);

  if (tw_unlikely(!dst->data)) {
    return NULL;
  }

  memcpy(dst->data, src->data, alloc_size);

  return dst;
}

struct tw_bitmap_rle *
tw_bitmap_rle_clone(const struct tw_bitmap_rle *bitmap)
{
  assert(bitmap);
  struct tw_bitmap_rle *dst = tw_bitmap_rle_new(bitmap->info.size);

  if (tw_unlikely(!dst)) {
    return NULL;
  }

  return tw_bitmap_rle_copy(bitmap, dst);
}

void
tw_bitmap_rle_set(struct tw_bitmap_rle *bitmap, uint32_t pos)
{
  assert(bitmap &&
         (!(bitmap->info.count) || bitmap->last_pos < pos) &&
         pos < bitmap->info.size &&
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
         (!(bitmap->info.count) || bitmap->last_pos < word->pos) &&
         (bitmap->info.count + word->count) < UINT32_MAX);

  const uint32_t gap = word->pos - bitmap->last_pos;
  struct tw_bitmap_rle_word *cur_word = &bitmap->data[bitmap->cur_word];

  if (bitmap->info.count == 0) {
    cur_word->pos = word->pos;
    cur_word->count = word->count;
  } else if (gap == 1) {
    cur_word->count += word->count;
  } else {
    cur_word = tw_bitmap_rle_get_next_word(bitmap);
    cur_word->pos = word->pos;
    cur_word->count = word->count;
  }

  bitmap->info.count += word->count;
  bitmap->last_pos = tw_bitmap_rle_word_end((*word));
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
    if (word.pos <= pos && pos <= tw_bitmap_rle_word_end(word)) {
      return true;
    }
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

int64_t
tw_bitmap_rle_find_first_zero(const struct tw_bitmap_rle *bitmap)
{
  assert(bitmap);
  if (tw_bitmap_rle_full(bitmap)) {
    return -1;
  }

  const struct tw_bitmap_rle_word word = bitmap->data[0];
  return (word.pos != 0) ? 0 : word.pos + word.count;
  /**    ^^^^^^^^^^^^^^^       ^^^^^^^^^^^^^^^^^^^^^
   *     bit at zero?          handles empty bitmap since 0 + 0 = 0
   */
}

int64_t
tw_bitmap_rle_find_first_bit(const struct tw_bitmap_rle *bitmap)
{
  assert(bitmap);
  if (tw_bitmap_rle_empty(bitmap)) {
    return -1;
  }

  return bitmap->data[0].pos;
}

struct tw_bitmap_rle *
tw_bitmap_rle_not(const struct tw_bitmap_rle *bitmap,
                        struct tw_bitmap_rle *dst)
{
  assert(bitmap && dst);

  const uint32_t size = bitmap->info.size;
  if (size != dst->info.size) {
    return NULL;
  }
  tw_bitmap_rle_zero(dst);

  /**
   * Negating a set of intervals embedded in a strict one [0, nbits-1] might
   * have 3 different outcomes, i.e. if S = |intervals| then we might witness
   * outcomes with S-1, S, or S+1 intervals. The following code treat the
   * first and last as special cases.
   */

  /* maybe first interval */
  const struct tw_bitmap_rle_word first = bitmap->data[0];
  if (first.pos != 0) {
    tw_bitmap_rle_set_range(dst, 0, first.pos - 1);
  }

  uint32_t i = 0;
  struct tw_bitmap_rle_word cur_word = first;
  while (i < bitmap->cur_word) {
    struct tw_bitmap_rle_word next = bitmap->data[i + 1];
    tw_bitmap_rle_set_range(dst, (cur_word.pos + cur_word.count), next.pos - 1);
    cur_word = next;
    ++i;
  }

  /* maybe last interval */
  const struct tw_bitmap_rle_word last = bitmap->data[bitmap->cur_word];
  const uint32_t last_zero = last.pos + last.count;
  if (last_zero < size) {
    tw_bitmap_rle_set_range(dst, last_zero, size - 1);
  }

  return dst;
}

bool
tw_bitmap_rle_equal(const struct tw_bitmap_rle *a,
                    const struct tw_bitmap_rle *b)
{
  assert(a && b);

  if (a->info.size != b->info.size || a->info.count != b->info.count) {
    return false;
  }

  /**
   * This assumes that bitmaps were manipulated with methods
   * tw_bitmap_rle_set_* because it merges concurrent words, e.g.
   * [(0, 2), (2, 1)] -> [(0, 3)]. While both reprensentations are equivalent,
   * the following loop works only on the minimal-compact form.
   */
  for (size_t i = 0; i <= a->cur_word; ++i) {
    if (!tw_bitmap_rle_word_equal(a->data[i], b->data[i])) {
      return false;
    }
  }

  return true;
}

/**
 * Private helper similar to tw_bitmap_rle_set_word excepts that it truncates
 * correctly when `word` is intersecting with the last added word in `bitmap`.
 * It is also a NOOP when `word` is fully contained in last added word.
 */
static
void
tw_bitmap_rle_set_word_truncate_(struct tw_bitmap_rle *bitmap,
                                 struct tw_bitmap_rle_word *word)
{
  assert(bitmap && word);

  const struct tw_bitmap_rle_word w = *word;
  const uint32_t last_pos = bitmap->last_pos;

  if (tw_bitmap_rle_empty(bitmap) || last_pos < w.pos) {
    tw_bitmap_rle_set_word(bitmap, word);
  } else if (last_pos < tw_bitmap_rle_word_end(w)) {
    const uint32_t start = last_pos + 1,
                   end = tw_bitmap_rle_word_end(w);
    tw_bitmap_rle_set_range(bitmap, start, end);
  }
}

struct tw_bitmap_rle *
tw_bitmap_rle_union(const struct tw_bitmap_rle *a,
                    const struct tw_bitmap_rle *b,
                          struct tw_bitmap_rle *dst)
{
  assert(a && b && dst);

  const uint32_t size = a->info.size;
  if (size != b->info.size && size != dst->info.size) {
    return NULL;
  }
  tw_bitmap_rle_zero(dst);

  if (tw_bitmap_rle_empty(a)) {
    return tw_bitmap_rle_copy(b, dst);
  }

  if (tw_bitmap_rle_empty(b)) {
    return tw_bitmap_rle_copy(a, dst);
  }

  const uint32_t a_last_idx = a->cur_word + 1, b_last_idx = b->cur_word + 1;
  uint32_t       a_idx = 0,                    b_idx = 0;
  struct tw_bitmap_rle_word *a_word = &(a->data[0]), *b_word = &(b->data[0]);

  /* Drain both rle_word lists until one is empty */
  while (a_idx < a_last_idx && b_idx < b_last_idx) {
    struct tw_bitmap_rle_word **min_word = tw_bitmap_rle_word_min_ref(a_word, b_word);
    tw_bitmap_rle_set_word_truncate_(dst, *min_word);
    if (*min_word == a_word) {
      a_word = &(a->data[++a_idx]);
    } else {
      b_word = &(b->data[++b_idx]);
    }
  }

  /** Drain remaining list */
  while (a_idx < a_last_idx) {
    tw_bitmap_rle_set_word_truncate_(dst, a_word);
    a_word = &(a->data[++a_idx]);
  }
  while (b_idx < b_last_idx) {
    tw_bitmap_rle_set_word_truncate_(dst, b_word);
    b_word = &(b->data[++b_idx]);
  }

  return dst;
}
