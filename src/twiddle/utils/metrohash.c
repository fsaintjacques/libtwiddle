#include <twiddle/utils/hash.h>

#include "../macrology.h"
#include "internal.h"

static const uint64_t k0_64 = 0xD6D018F5;
static const uint64_t k1_64 = 0xA2AA033B;
static const uint64_t k2_64 = 0x62992FC1;
static const uint64_t k3_64 = 0x30BC5B29;

uint64_t tw_metrohash_64(const uint64_t seed, const void *key,
                         const size_t key_len)
{
  const uint8_t *ptr = (uint8_t *)key;
  const uint8_t *const end = ptr + key_len;

  uint64_t h = (seed + k2_64) * k0_64;

  // clang-format off
  if (key_len >= 32) {
    uint64_t v[4];
    v[0] = h;
    v[1] = h;
    v[2] = h;
    v[3] = h;

    do {
      v[0] += cread_u64(ptr) * k0_64; ptr += 8; v[0] = rotr64(v[0],29) + v[2];
      v[1] += cread_u64(ptr) * k1_64; ptr += 8; v[1] = rotr64(v[1],29) + v[3];
      v[2] += cread_u64(ptr) * k2_64; ptr += 8; v[2] = rotr64(v[2],29) + v[0];
      v[3] += cread_u64(ptr) * k3_64; ptr += 8; v[3] = rotr64(v[3],29) + v[1];
    } while (ptr <= (end - 32));

    v[2] ^= rotr64(((v[0] + v[3]) * k0_64) + v[1], 37) * k1_64;
    v[3] ^= rotr64(((v[1] + v[2]) * k1_64) + v[0], 37) * k0_64;
    v[0] ^= rotr64(((v[0] + v[2]) * k0_64) + v[3], 37) * k1_64;
    v[1] ^= rotr64(((v[1] + v[3]) * k1_64) + v[2], 37) * k0_64;
    h += v[0] ^ v[1];
  }

  if ((end - ptr) >= 16) {
    uint64_t v0 = h + (cread_u64(ptr) * k2_64); ptr += 8; v0 = rotr64(v0,29) * k3_64;
    uint64_t v1 = h + (cread_u64(ptr) * k2_64); ptr += 8; v1 = rotr64(v1,29) * k3_64;
    v0 ^= rotr64(v0 * k0_64, 21) + v1;
    v1 ^= rotr64(v1 * k3_64, 21) + v0;
    h += v1;
  }

  if ((end - ptr) >= 8) {
    h += cread_u64(ptr) * k3_64; ptr += 8;
    h ^= rotr64(h, 55) * k1_64;
  }

  if ((end - ptr) >= 4) {
    h += cread_u32(ptr) * k3_64; ptr += 4;
    h ^= rotr64(h, 26) * k1_64;
  }

  if ((end - ptr) >= 2) {
    h += cread_u16(ptr) * k3_64; ptr += 2;
    h ^= rotr64(h, 48) * k1_64;
  }

  if ((end - ptr) >= 1) {
    h += cread_u8(ptr) * k3_64;
    h ^= rotr64(h, 37) * k1_64;
  }

  h ^= rotr64(h, 28);
  h *= k0_64;
  h ^= rotr64(h, 29);
  // clang-format on

  return h;
}

static const uint64_t k0_128 = 0xC83A91E1;
static const uint64_t k1_128 = 0x8648DBDB;
static const uint64_t k2_128 = 0x7BDEC03B;
static const uint64_t k3_128 = 0x2F5870A5;

tw_uint128_t tw_metrohash_128(const uint64_t seed, const void *key,
                              size_t key_len)
{
  const uint8_t *ptr = (uint8_t *)key;
  const uint8_t *const end = ptr + key_len;

  uint64_t v[4];

  // clang-format off
  v[0] = (seed - k0_128) * k3_128;
  v[1] = (seed + k1_128) * k2_128;

  if (key_len >= 32) {
    v[2] = (seed + k0_128) * k2_128;
    v[3] = (seed - k1_128) * k3_128;

    do {
      v[0] += cread_u64(ptr) * k0_128; ptr += 8; v[0] = rotr64(v[0],29) + v[2];
      v[1] += cread_u64(ptr) * k1_128; ptr += 8; v[1] = rotr64(v[1],29) + v[3];
      v[2] += cread_u64(ptr) * k2_128; ptr += 8; v[2] = rotr64(v[2],29) + v[0];
      v[3] += cread_u64(ptr) * k3_128; ptr += 8; v[3] = rotr64(v[3],29) + v[1];
    } while (ptr <= (end - 32));

    v[2] ^= rotr64(((v[0] + v[3]) * k0_128) + v[1], 21) * k1_128;
    v[3] ^= rotr64(((v[1] + v[2]) * k1_128) + v[0], 21) * k0_128;
    v[0] ^= rotr64(((v[0] + v[2]) * k0_128) + v[3], 21) * k1_128;
    v[1] ^= rotr64(((v[1] + v[3]) * k1_128) + v[2], 21) * k0_128;
  }

  if ((end - ptr) >= 16) {
    v[0] += cread_u64(ptr) * k2_128; ptr += 8; v[0] = rotr64(v[0],33) * k3_128;
    v[1] += cread_u64(ptr) * k2_128; ptr += 8; v[1] = rotr64(v[1],33) * k3_128;
    v[0] ^= rotr64((v[0] * k2_128) + v[1], 45) * k1_128;
    v[1] ^= rotr64((v[1] * k3_128) + v[0], 45) * k0_128;
  }

  if ((end - ptr) >= 8) {
    v[0] += cread_u64(ptr) * k2_128; ptr += 8; v[0] = rotr64(v[0],33) * k3_128;
    v[0] ^= rotr64((v[0] * k2_128) + v[1], 27) * k1_128;
  }

  if ((end - ptr) >= 4) {
    v[1] += cread_u32(ptr) * k2_128; ptr += 4; v[1] = rotr64(v[1],33) * k3_128;
    v[1] ^= rotr64((v[1] * k3_128) + v[0], 46) * k0_128;
  }

  if ((end - ptr) >= 2) {
    v[0] += cread_u16(ptr) * k2_128; ptr += 2; v[0] = rotr64(v[0],33) * k3_128;
    v[0] ^= rotr64((v[0] * k2_128) + v[1], 22) * k1_128;
  }

  if ((end - ptr) >= 1) {
    v[1] += cread_u8(ptr) * k2_128; v[1] = rotr64(v[1],33) * k3_128;
    v[1] ^= rotr64((v[1] * k3_128) + v[0], 58) * k0_128;
  }

  v[0] += rotr64((v[0] * k0_128) + v[1], 13);
  v[1] += rotr64((v[1] * k1_128) + v[0], 37);
  v[0] += rotr64((v[0] * k2_128) + v[1], 13);
  v[1] += rotr64((v[1] * k3_128) + v[0], 37);
  // clang-format on

  return (tw_uint128_t){.h = v[0], .l = v[1]};
}
