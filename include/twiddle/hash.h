/**
 * Murmur3 was created by Austin Appleby, and the C port
 * and general tidying up was done by Peter Scott.
 */

#ifndef TWIDDLE_HASH_H
#define TWIDDLE_HASH_H

#include <stddef.h>
#include <stdint.h>

void murmur3_x86_32(const void *key, size_t len, uint32_t seed, void *out);
void murmur3_x86_128(const void *key, size_t len, uint32_t seed, void *out);
void murmur3_x64_128(const void *key, size_t len, uint32_t seed, void *out);

#endif /* TWIDDLE_HASH_H */
