#ifndef TWIDDLE_BLOOMFILTER_A2_H
#define TWIDDLE_BLOOMFILTER_A2_H

#include <twiddle/bloomfilter/bloomfilter.h>

struct tw_bloomfilter_a2 {
  float density;

  struct tw_bloomfilter *active;
  struct tw_bloomfilter *passive;
};

struct tw_bloomfilter_a2 *tw_bloomfilter_a2_new(uint64_t size, uint16_t k,
                                                float dentisy);

struct tw_bloomfilter_a2 *
tw_bloomfilter_a2_copy(const struct tw_bloomfilter_a2 *src,
                       struct tw_bloomfilter_a2 *dst);

struct tw_bloomfilter_a2 *
tw_bloomfilter_a2_clone(const struct tw_bloomfilter_a2 *bf);

void tw_bloomfilter_a2_free(struct tw_bloomfilter_a2 *bf);

void tw_bloomfilter_a2_set(struct tw_bloomfilter_a2 *bf, size_t size,
                           const char *buf);

bool tw_bloomfilter_a2_test(const struct tw_bloomfilter_a2 *bf, size_t size,
                            const char *buf);

bool tw_bloomfilter_a2_empty(const struct tw_bloomfilter_a2 *bf);

bool tw_bloomfilter_a2_full(const struct tw_bloomfilter_a2 *bf);

uint64_t tw_bloomfilter_a2_count(const struct tw_bloomfilter_a2 *bf);

float tw_bloomfilter_a2_density(const struct tw_bloomfilter_a2 *bf);

struct tw_bloomfilter_a2 *tw_bloomfilter_a2_zero(struct tw_bloomfilter_a2 *bf);

struct tw_bloomfilter_a2 *tw_bloomfilter_a2_fill(struct tw_bloomfilter_a2 *bf);

struct tw_bloomfilter_a2 *tw_bloomfilter_a2_not(struct tw_bloomfilter_a2 *bf);

bool tw_bloomfilter_a2_equal(const struct tw_bloomfilter_a2 *a,
                             const struct tw_bloomfilter_a2 *b);

struct tw_bloomfilter_a2 *
tw_bloomfilter_a2_union(const struct tw_bloomfilter_a2 *src,
                        struct tw_bloomfilter_a2 *dst);

struct tw_bloomfilter_a2 *
tw_bloomfilter_a2_intersection(const struct tw_bloomfilter_a2 *src,
                               struct tw_bloomfilter_a2 *dst);

struct tw_bloomfilter_a2 *
tw_bloomfilter_a2_xor(const struct tw_bloomfilter_a2 *src,
                      struct tw_bloomfilter_a2 *dst);

#endif /* TWIDDLE_BLOOMFILTER_A2_H */
