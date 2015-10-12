#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include <twiddle/hyperloglog.h>
#include <twiddle/hash.h>
#include <twiddle/internal/utils.h>

struct tw_hyperloglog *
tw_hyperloglog_new(uint32_t precision)
{
  assert(precision > 3);

  struct tw_hyperloglog *hll = calloc(1, sizeof(struct tw_hyperloglog) + (1 << precision));
  if (!hll) {
    return NULL;
  }

  hll->info = (struct tw_hyperloglog_info) {.precision = precision, .hash_seed = TW_HLL_DEFAULT_SEED };

  return hll;
}

void
tw_hyperloglog_free(struct tw_hyperloglog *hll)
{
  assert(hll);
  free(hll);
}

struct tw_hyperloglog *
tw_hyperloglog_copy(const struct tw_hyperloglog *src,
                          struct tw_hyperloglog *dst)
{
  assert(src && dst);

  const uint32_t precision = src->info.precision;
  if (precision != dst->info.precision) {
    return NULL;
  }

  tw_hyperloglog_info_copy(src->info, dst->info);

  const int n_registers = 1 << precision;
  for (int i = 0; i < n_registers; ++i) {
    dst->registers[i] = src->registers[i];
  }

  return dst;
}

struct tw_hyperloglog *
tw_hyperloglog_clone(const struct tw_hyperloglog *src)
{
  assert(src);

  struct tw_hyperloglog * dst = tw_hyperloglog_new(src->info.precision);
  if (dst == NULL) {
    return NULL;
  }

  return tw_hyperloglog_copy(src, dst);
}

void
tw_hyperloglog_add_hashed(struct tw_hyperloglog *hll, uint64_t hash)
{
  assert(hll);
  const uint32_t precision = hll->info.precision;
  const uint32_t bucket_idx = hash >> (64 - precision);
  const uint8_t leading_zeros = (__builtin_clzll(hash << precision | (1 << (precision -1))) + 1),
          old_val = hll->registers[bucket_idx];
  hll->registers[bucket_idx] = (leading_zeros > old_val) ? leading_zeros : old_val;
}

void
tw_hyperloglog_add(struct tw_hyperloglog *hll,
                   size_t key_size, const char* key_buf)
{
  assert(hll && key_size > 0 && key_buf);
  uint64_t hash[2];
  murmur3_x64_128(key_buf, key_size, hll->info.hash_seed, hash);

  tw_hyperloglog_add_hashed(hll, hash[0]);
}

static
double
linear_count(uint32_t n_registers, uint32_t n_zeros)
{
  return -1.0 * n_registers * log((double)n_zeros / (double)n_registers);
}

static
double
alpha(uint32_t n_registers, uint32_t precision)
{
  switch (precision) {
    case 4:
      return 0.673;
    case 5:
      return 0.697;
    case 6:
      return 0.709;
    default:
      return 0.7213 / (1 + 1.079 / n_registers);
  }
}

static
double
estimate(uint32_t n_registers, uint32_t precision, double inverse_sum)
{
  const double multi = alpha(n_registers, precision) * n_registers * n_registers;
  return multi * (1.0 / inverse_sum);
}

double
tw_hyperloglog_count(const struct tw_hyperloglog *hll)
{
  assert(hll);

  const uint32_t precision = hll->info.precision;
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

  return (n_zeros) ? linear_count(n_registers, n_zeros) :
                     estimate(n_registers, precision, inverse_sum);
}

struct tw_hyperloglog *
tw_hyperloglog_merge(const struct tw_hyperloglog *src,
                           struct tw_hyperloglog *dst)
{
  assert(src && dst);

  if (!tw_hyperloglog_info_equal(src->info, dst->info)) {
    return NULL;
  }

  const uint32_t n_registers = 1 << src->info.precision;
  for (int i = 0; i < n_registers; ++i) {
    dst->registers[i] = (dst->registers[i] < src->registers[i]) ?
      src->registers[i] :
      dst->registers[i] ;
  }

  return dst;
}
