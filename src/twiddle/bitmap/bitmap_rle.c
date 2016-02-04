#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <twiddle/bitmap/bitmap_rle.h>
#include <twiddle/internal/utils.h>

static inline struct tw_bitmap_rle_word *
tw_bitmap_rle_word_alloc_(struct tw_bitmap_rle *bitmap, uint64_t n_words)
{
  const uint64_t alloc_size = n_words * sizeof(struct tw_bitmap_rle_word);
  struct tw_bitmap_rle_word *data = calloc(1, alloc_size);

  if (!data) {
    return NULL;
  }

  bitmap->alloc_word = n_words;
  bitmap->data = data;

  return data;
}

/* default rle words = 4 cachelines */
#define tw_bitmap_rle_word_alloc(bitmap)                                       \
  tw_bitmap_rle_word_alloc_(bitmap, TW_BITMAP_RLE_WORD_PER_CACHELINE * 4)

static inline struct tw_bitmap_rle_word *
tw_bitmap_rle_word_grow_(struct tw_bitmap_rle *bitmap, uint64_t words)
{
  const uint64_t alloc_word = bitmap->alloc_word + words;
  const uint64_t alloc_size = alloc_word * sizeof(struct tw_bitmap_rle_word);
  struct tw_bitmap_rle_word *data = realloc(bitmap->data, alloc_size);

  if (!data) {
    return NULL;
  }

  bitmap->alloc_word = alloc_word;
  bitmap->data = data;

  return data;
}

/* double size by default */
#define tw_bitmap_rle_word_grow(bitmap)                                        \
  tw_bitmap_rle_word_grow_(bitmap, bitmap->alloc_word)

static inline struct tw_bitmap_rle_word *
tw_bitmap_rle_get_next_word(struct tw_bitmap_rle *bitmap)
{
  if (bitmap->alloc_word == ++(bitmap->last_word_idx)) {
    if (!tw_bitmap_rle_word_grow(bitmap)) {
      return NULL;
    }
  }

  return &(bitmap->data[bitmap->last_word_idx]);
}

struct tw_bitmap_rle *tw_bitmap_rle_new(uint64_t nbits)
{
  assert(0 < nbits && nbits <= TW_BITMAP_MAX_BITS);
  struct tw_bitmap_rle *bitmap = calloc(1, sizeof(struct tw_bitmap_rle));
  if (!bitmap) {
    return NULL;
  }

  bitmap->size = nbits;
  bitmap->count = 0;

  if (!tw_bitmap_rle_word_alloc(bitmap)) {
    free(bitmap);
    return NULL;
  }

  return bitmap;
}

void tw_bitmap_rle_free(struct tw_bitmap_rle *bitmap)
{
  free(bitmap->data);
  free(bitmap);
}

struct tw_bitmap_rle *tw_bitmap_rle_copy(const struct tw_bitmap_rle *src,
                                         struct tw_bitmap_rle *dst)
{
  assert(src && dst);

  if (tw_likely(dst->data != NULL)) {
    free(dst->data);
  }

  dst->size = src->size;
  dst->count = src->count;
  dst->last_pos = src->last_pos;

  const uint64_t alloc_word = src->alloc_word;
  const uint64_t alloc_size = alloc_word * sizeof(struct tw_bitmap_rle_word);
  dst->last_word_idx = src->last_word_idx;
  dst->alloc_word = alloc_word;
  dst->data = calloc(1, alloc_size);

  if (tw_unlikely(!dst->data)) {
    return NULL;
  }

  memcpy(dst->data, src->data, alloc_size);

  return dst;
}

struct tw_bitmap_rle *tw_bitmap_rle_clone(const struct tw_bitmap_rle *bitmap)
{
  assert(bitmap);
  struct tw_bitmap_rle *dst = tw_bitmap_rle_new(bitmap->size);

  if (tw_unlikely(!dst)) {
    return NULL;
  }

  return tw_bitmap_rle_copy(bitmap, dst);
}

void tw_bitmap_rle_set(struct tw_bitmap_rle *bitmap, uint64_t pos)
{
  const struct tw_bitmap_rle_word word = {.pos = pos, .count = 1};
  tw_bitmap_rle_set_word(bitmap, &word);
}

void tw_bitmap_rle_set_word(struct tw_bitmap_rle *bitmap,
                            const struct tw_bitmap_rle_word *word)
{
  assert(bitmap && word && (!(bitmap->count) || bitmap->last_pos < word->pos) &&
         (bitmap->count + word->count) < TW_BITMAP_MAX_BITS);

  const uint64_t gap = word->pos - bitmap->last_pos;
  struct tw_bitmap_rle_word *last_word = &bitmap->data[bitmap->last_word_idx];

  if (bitmap->count == 0) {
    last_word->pos = word->pos;
    last_word->count = word->count;
  } else if (gap == 1) {
    last_word->count += word->count;
  } else {
    last_word = tw_bitmap_rle_get_next_word(bitmap);
    last_word->pos = word->pos;
    last_word->count = word->count;
  }

  bitmap->count += word->count;
  bitmap->last_pos = tw_bitmap_rle_word_end((*word));
}

void tw_bitmap_rle_set_range(struct tw_bitmap_rle *bitmap, uint64_t start,
                             uint64_t end)
{
  assert(bitmap);
  const struct tw_bitmap_rle_word word = {.pos = start,
                                          .count = end - start + 1};
  tw_bitmap_rle_set_word(bitmap, &word);
}

bool tw_bitmap_rle_test(const struct tw_bitmap_rle *bitmap, uint64_t pos)
{
  assert(bitmap);
  if (bitmap->last_pos < pos || (bitmap->count == 0)) {
    /**                           ^^^^^^^^^^^^^^^^^^^^^^^^^
     *                            Requires a special handling because
     *                            tw_bitmap_rle_zero() may trick the
     * following
     *                            loop.
     */
    return false;
  }

  const uint64_t last_word_idx = bitmap->last_word_idx;
  for (uint64_t i = 0; i <= last_word_idx; ++i) {
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

bool tw_bitmap_rle_empty(const struct tw_bitmap_rle *bitmap)
{
  assert(bitmap);
  return bitmap->count == 0;
}

bool tw_bitmap_rle_full(const struct tw_bitmap_rle *bitmap)
{
  assert(bitmap);
  return bitmap->count == bitmap->size;
}

uint64_t tw_bitmap_rle_count(const struct tw_bitmap_rle *bitmap)
{
  assert(bitmap);
  return bitmap->count;
}

float tw_bitmap_rle_density(const struct tw_bitmap_rle *bitmap)
{
  assert(bitmap);
  return bitmap->count / (float)bitmap->size;
}

struct tw_bitmap_rle *tw_bitmap_rle_zero(struct tw_bitmap_rle *bitmap)
{
  assert(bitmap);
  const uint64_t last_word_idx = bitmap->last_word_idx;
  for (uint64_t i = 0; i <= last_word_idx; ++i) {
    bitmap->data[i] = tw_bitmap_rle_word_zero;
  }

  bitmap->last_word_idx = 0U;
  bitmap->count = 0U;
  bitmap->last_pos = 0U;

  return bitmap;
}

struct tw_bitmap_rle *tw_bitmap_rle_fill(struct tw_bitmap_rle *bitmap)
{
  assert(bitmap);
  const uint64_t last_word_idx = bitmap->last_word_idx;
  for (uint64_t i = 0; i <= last_word_idx; ++i) {
    bitmap->data[i] = tw_bitmap_rle_word_zero;
  }

  const uint64_t size = bitmap->size;
  bitmap->data[0] = tw_bitmap_rle_word_full(size);
  bitmap->last_word_idx = 0U;
  bitmap->count = size;
  bitmap->last_pos = size - 1;

  return bitmap;
}

int64_t tw_bitmap_rle_find_first_zero(const struct tw_bitmap_rle *bitmap)
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

int64_t tw_bitmap_rle_find_first_bit(const struct tw_bitmap_rle *bitmap)
{
  assert(bitmap);
  if (tw_bitmap_rle_empty(bitmap)) {
    return -1;
  }

  return bitmap->data[0].pos;
}

struct tw_bitmap_rle *tw_bitmap_rle_not(const struct tw_bitmap_rle *bitmap,
                                        struct tw_bitmap_rle *dst)
{
  assert(bitmap && dst);

  const uint64_t size = bitmap->size;
  if (size != dst->size) {
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
  const struct tw_bitmap_rle_word first_word = bitmap->data[0];
  if (first_word.pos != 0) {
    tw_bitmap_rle_set_range(dst, 0, first_word.pos - 1);
  }

  uint64_t i = 0;
  struct tw_bitmap_rle_word word = first_word;
  while (i < bitmap->last_word_idx) {
    struct tw_bitmap_rle_word next = bitmap->data[i + 1];
    tw_bitmap_rle_set_range(dst, (word.pos + word.count), next.pos - 1);
    word = next;
    ++i;
  }

  /* maybe last interval */
  const struct tw_bitmap_rle_word last_word =
      bitmap->data[bitmap->last_word_idx];
  const uint64_t last_zero = last_word.pos + last_word.count;
  if (last_zero < size) {
    tw_bitmap_rle_set_range(dst, last_zero, size - 1);
  }

  return dst;
}

bool tw_bitmap_rle_equal(const struct tw_bitmap_rle *a,
                         const struct tw_bitmap_rle *b)
{
  assert(a && b);

  if (a->size != b->size || a->count != b->count) {
    return false;
  }

  /**
   * This assumes that bitmaps were manipulated with methods
   * tw_bitmap_rle_set_* because it merges concurrent words, e.g.
   * [(0, 2), (2, 1)] -> [(0, 3)]. While both reprensentations are equivalent,
   * the following loop works only on the minimal-compact form.
   */
  for (size_t i = 0; i <= a->last_word_idx; ++i) {
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
static void tw_bitmap_rle_set_word_truncate_(struct tw_bitmap_rle *bitmap,
                                             struct tw_bitmap_rle_word *word)
{
  assert(bitmap && word);

  const struct tw_bitmap_rle_word w = *word;
  const uint64_t last_pos = bitmap->last_pos;

  if (tw_bitmap_rle_empty(bitmap) || last_pos < w.pos) {
    tw_bitmap_rle_set_word(bitmap, word);
  } else if (last_pos < tw_bitmap_rle_word_end(w)) {
    const uint64_t start = last_pos + 1, end = tw_bitmap_rle_word_end(w);
    tw_bitmap_rle_set_range(bitmap, start, end);
  }
}

struct tw_bitmap_rle *tw_bitmap_rle_union(const struct tw_bitmap_rle *a,
                                          const struct tw_bitmap_rle *b,
                                          struct tw_bitmap_rle *dst)
{
  assert(a && b && dst);

  const uint64_t size = a->size;
  if (size != b->size && size != dst->size) {
    return NULL;
  }
  tw_bitmap_rle_zero(dst);

  if (tw_bitmap_rle_empty(a)) {
    return tw_bitmap_rle_copy(b, dst);
  }

  if (tw_bitmap_rle_empty(b)) {
    return tw_bitmap_rle_copy(a, dst);
  }

  const uint64_t a_last_idx = a->last_word_idx + 1,
                 b_last_idx = b->last_word_idx + 1;
  uint64_t a_idx = 0, b_idx = 0;
  struct tw_bitmap_rle_word *a_word = &(a->data[0]), *b_word = &(b->data[0]);

  /* Drain both rle_word lists until one is empty */
  while (a_idx < a_last_idx && b_idx < b_last_idx) {
    struct tw_bitmap_rle_word **min_word =
        tw_bitmap_rle_word_min_ref(a_word, b_word);
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

struct tw_bitmap_rle *tw_bitmap_rle_intersection(const struct tw_bitmap_rle *a,
                                                 const struct tw_bitmap_rle *b,
                                                 struct tw_bitmap_rle *dst)
{
  assert(a && b && dst);

  const uint64_t size = a->size;
  if (size != b->size && size != dst->size) {
    return NULL;
  }
  tw_bitmap_rle_zero(dst);

  if (tw_bitmap_rle_empty(a)) {
    return dst;
  }

  if (tw_bitmap_rle_empty(b)) {
    return dst;
  }

  const uint64_t a_last_idx = a->last_word_idx + 1,
                 b_last_idx = b->last_word_idx + 1;
  uint64_t a_idx = 0, b_idx = 0;
  struct tw_bitmap_rle_word *a_word = &(a->data[0]), *b_word = &(b->data[0]);

  /* Drain both rle_word lists until one is empty */
  while (a_idx < a_last_idx && b_idx < b_last_idx) {
    const uint64_t a_start = a_word->pos, b_start = b_word->pos;
    const uint64_t a_end = tw_bitmap_rle_word_end((*a_word)),
                   b_end = tw_bitmap_rle_word_end((*b_word));
    /** let intervals a = [a_0, a_1] and b = [b_0, b_1] then
     *  (a intersect b) <=> (max(a_0, b_0) <= min(a_1, b_1))
     */
    const uint64_t start_pos = (a_start <= b_start) ? b_start : a_start;
    const uint64_t end_pos = (a_end <= b_end) ? a_end : b_end;

    if (start_pos <= end_pos) {
      tw_bitmap_rle_set_range(dst, start_pos, end_pos);
    }

    /** Advance word with the smallest end position */
    if (a_end <= b_end) {
      a_word = &(a->data[++a_idx]);
    } else {
      b_word = &(b->data[++b_idx]);
    }
  }

  return dst;
}
