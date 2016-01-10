#include <assert.h>
#include <math.h>
#include <string.h>

#include <twiddle/hyperloglog/hyperloglog.h>
#include <twiddle/hash/metrohash.h>
#include <twiddle/internal/utils.h>

struct tw_hyperloglog *tw_hyperloglog_new(uint8_t precision)
{
  if (precision < TW_HLL_MIN_PRECISION || precision > TW_HLL_MAX_PRECISION) {
    return NULL;
  }

  struct tw_hyperloglog *hll = calloc(1, sizeof(struct tw_hyperloglog));
  if (!hll) {
    return NULL;
  }

  size_t alloc_size = (1 << precision) * sizeof(uint8_t);

  if (posix_memalign((void *)&hll->registers, sizeof(uint64_t), alloc_size)) {
    free(hll);
    return NULL;
  }

  memset(hll->registers, 0, alloc_size);

  hll->info = (struct tw_hyperloglog_info){.precision = precision,
                                           .hash_seed = TW_HLL_DEFAULT_SEED};

  return hll;
}

void tw_hyperloglog_free(struct tw_hyperloglog *hll)
{
  assert(hll);
  free(hll->registers);
  free(hll);
}

struct tw_hyperloglog *tw_hyperloglog_copy(const struct tw_hyperloglog *src,
                                           struct tw_hyperloglog *dst)
{
  assert(src && dst);

  const uint8_t precision = src->info.precision;
  if (precision != dst->info.precision) {
    return NULL;
  }

  tw_hyperloglog_info_copy(src->info, dst->info);

  const uint32_t n_registers = 1 << precision;
  for (int i = 0; i < n_registers; ++i) {
    dst->registers[i] = src->registers[i];
  }

  return dst;
}

struct tw_hyperloglog *tw_hyperloglog_clone(const struct tw_hyperloglog *src)
{
  assert(src);

  struct tw_hyperloglog *dst = tw_hyperloglog_new(src->info.precision);
  if (dst == NULL) {
    return NULL;
  }

  return tw_hyperloglog_copy(src, dst);
}

void tw_hyperloglog_add_hashed(struct tw_hyperloglog *hll, uint64_t hash)
{
  assert(hll);
  const uint8_t precision = hll->info.precision;
  const uint32_t bucket_idx = hash >> (64 - precision);
  const uint8_t leading_zeros = (__builtin_clzll(hash << precision |
                                                 (1 << (precision - 1))) +
                                 1),
                old_val = hll->registers[bucket_idx];
  hll->registers[bucket_idx] =
      (leading_zeros > old_val) ? leading_zeros : old_val;
}

void tw_hyperloglog_add(struct tw_hyperloglog *hll, size_t key_size,
                        const char *key_buf)
{
  assert(hll && key_size > 0 && key_buf);
  const uint64_t hash = tw_metrohash_64(hll->info.hash_seed, key_buf, key_size);
  tw_hyperloglog_add_hashed(hll, hash);
}

extern double estimate(uint8_t precision, uint32_t n_zeros, double inverse_sum);

double tw_hyperloglog_count(const struct tw_hyperloglog *hll)
{
  assert(hll);

  const uint8_t precision = hll->info.precision;
  const uint32_t n_registers = 1 << precision;
  uint32_t n_zeros = 0;
  double inverse_sum = 0.0;

  for (int i = 0; i < n_registers; ++i) {
    const uint8_t val = hll->registers[i];
    inverse_sum += pow(2, -1.0 * val);
    if (val == 0) {
      ++n_zeros;
    }
  }

  return estimate(precision, n_zeros, inverse_sum);
}

bool tw_hyperloglog_equal(const struct tw_hyperloglog *a,
                          const struct tw_hyperloglog *b)
{
  assert(a && b);

  if (!tw_hyperloglog_info_equal(a->info, b->info)) {
    return false;
  }

  const uint8_t precision = a->info.precision;
  const uint32_t n_registers = 1 << precision;

  for (int i = 0; i < n_registers; ++i) {
    if (a->registers[i] != b->registers[i]) {
      return false;
    }
  }

  return true;
}

struct tw_hyperloglog *tw_hyperloglog_merge(const struct tw_hyperloglog *src,
                                            struct tw_hyperloglog *dst)
{
  assert(src && dst);

  if (!tw_hyperloglog_info_equal(src->info, dst->info)) {
    return NULL;
  }

  const uint32_t n_registers = 1 << src->info.precision;
  for (int i = 0; i < n_registers; ++i) {
    dst->registers[i] = (dst->registers[i] < src->registers[i])
                            ? src->registers[i]
                            : dst->registers[i];
  }

  return dst;
}
