#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <twiddle/internal/utils.h>
#include <twiddle/hash/minhash.h>
#include <twiddle/hash/metrohash.h>

struct tw_minhash *tw_minhash_new(uint32_t n_registers)
{
  if (n_registers == 0) {
    return NULL;
  }

  struct tw_minhash *hash =
      calloc(1, sizeof(struct tw_minhash) +
                    TW_BYTES_PER_MINHASH_REGISTER * n_registers);
  if (!hash) {
    return NULL;
  }

  hash->info = tw_minhash_info_init(n_registers, TW_MINHASH_DEFAULT_SEED);

  return hash;
}

void tw_minhash_free(struct tw_minhash *hash) { free(hash); }

struct tw_minhash *tw_minhash_copy(const struct tw_minhash *src,
                                   struct tw_minhash *dst)
{
  assert(src && dst);

  if (!tw_minhash_info_equal(src->info, dst->info)) {
    return NULL;
  }

  memcpy(dst->registers, src->registers,
         src->info.n_registers * TW_BYTES_PER_MINHASH_REGISTER);

  return dst;
}

struct tw_minhash *tw_minhash_clone(const struct tw_minhash *hash)
{
  assert(hash);

  struct tw_minhash *copy = tw_minhash_new(hash->info.n_registers);
  if (!copy) {
    return NULL;
  }
  copy->info.hash_seed = hash->info.hash_seed;

  return tw_minhash_copy(hash, copy);
}

void tw_minhash_add(struct tw_minhash *hash, size_t key_size,
                    const char *key_buf)
{
  assert(hash && key_size > 0 && key_buf);

  const uint64_t hashed =
      tw_metrohash_64(hash->info.hash_seed, key_buf, key_size);

  const uint32_t n_registers = hash->info.n_registers;
  for (size_t i = 0; i < n_registers; ++i) {
    const uint32_t hashed_i = ((uint32_t)hashed + i * (uint32_t)(hashed >> 32));
    hash->registers[i] = tw_max(hash->registers[i], hashed_i);
  }
}

float tw_minhash_estimate(const struct tw_minhash *a,
                          const struct tw_minhash *b)
{
  assert(a && b);

  if (!tw_minhash_info_equal(a->info, b->info)) {
    return 0.0;
  }

  const uint32_t n_registers = a->info.n_registers;
  uint32_t registers_equal = 0;
  for (int i = 0; i < n_registers; ++i) {
    registers_equal += (a->registers[i] == b->registers[i]);
  }

  return (float)registers_equal / (float)n_registers;
}

bool tw_minhash_equal(const struct tw_minhash *a, const struct tw_minhash *b)
{
  assert(a && b);

  if (!tw_minhash_info_equal(a->info, b->info)) {
    return false;
  }

  const uint32_t n_registers = a->info.n_registers;
  for (int i = 0; i < n_registers; ++i) {
    if (a->registers[i] != b->registers[i]) {
      return false;
    }
  }

  return true;
}

struct tw_minhash *tw_minhash_merge(const struct tw_minhash *src,
                                    struct tw_minhash *dst)
{
  assert(src && dst);

  if (!tw_minhash_info_equal(src->info, dst->info)) {
    return NULL;
  }

  const uint32_t n_registers = src->info.n_registers;
  for (int i = 0; i < n_registers; ++i) {
    dst->registers[i] = tw_max(dst->registers[i], src->registers[i]);
  }

  return dst;
}
