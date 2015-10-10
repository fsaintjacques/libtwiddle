#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include <twiddle/hyperloglog.h>
#include <twiddle/hash.h>
#include <twiddle/internal/utils.h>

struct tw_hyperloglog *
tw_hyperloglog_new(uint32_t size)
{
  assert(size > 0);
  struct tw_hyperloglog *hll = calloc(1, sizeof(struct tw_hyperloglog) + size);
  if (!hll) {
    return NULL;
  }

  hll->size = size;
  hll->hash_seed = TW_HLL_DEFAULT_SEED;

  return hll;
}

void
tw_hyperloglog_free(struct tw_hyperloglog *hll)
{
  assert(hll);
  free(hll);
}

void
tw_hyperloglog_add_hashed(struct tw_hyperloglog *hll, uint64_t hash)
{
  assert(hll);
  uint32_t bucket_idx = hash % hll->size;
  uint8_t leading_zeros = tw_ffsl(hash),
          old_val = hll->registers[bucket_idx];
  hll->registers[bucket_idx] = (leading_zeros > old_val) ? leading_zeros : old_val;
}

void
tw_hyperloglog_add(struct tw_hyperloglog *hll,
                   size_t key_size, const char* key_buf)
{
  assert(hll && key_size > 0 && key_buf);
  uint64_t hash[2];
  murmur3_x64_128(key_buf, key_size, hll->hash_seed, hash);

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
estimate(uint32_t n_registers, double inverse_sum)
{
  const double alpha = 0.7213 / (1 + 1.079 / n_registers);
  const double multi = alpha * n_registers * n_registers;

  return multi * (1.0 / inverse_sum);
}


double
tw_hyperloglog_estimate(const struct tw_hyperloglog *hll)
{
  assert(hll);

  const uint32_t n_registers = hll->size;
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
                     estimate(n_registers, inverse_sum);
}
