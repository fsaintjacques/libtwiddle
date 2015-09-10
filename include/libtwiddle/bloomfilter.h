#ifndef LIBTWIDDLE_BLOOMFILTER_H
#define LIBTWIDDLE_BLOOMFILTER_H

#include <stdbool.h>
#include <stdint.h>

#include <libtwiddle/bitmap.h>

struct tw_bloomfilter_info {
  uint32_t size;
  uint32_t k;
};

#define tw_bloomfilter_info_init(size,k) \
  (struct tw_bloomfilter_info) {.size = size, .k = k }
#define tw_bloomfilter_info_copy(src, dst) \
  dst = (struct tw_bloomfilter_info) {.size = src.size, .k = src.k}

struct tw_bloomfilter {
  struct tw_bloomfilter_info info;
  struct tw_bitmap *bitmap;
};

static inline
void
tw_bloomfilter_hash_positions(uint32_t positions[], uint32_t k,
                              size_t size, const char * buf)
{
  for (int i = 0; i < k; ++i)
    positions[i] = 1;
}

struct tw_bloomfilter *
tw_bloomfilter_new(uint32_t size, uint32_t k);

void
tw_bloomfilter_free(struct tw_bloomfilter *bf);

struct tw_bloomfilter *
tw_bloomfilter_copy(const struct tw_bloomfilter *src,
                    struct tw_bloomfilter *dst);

void
tw_bloomfilter_set(struct tw_bloomfilter *bf,
                   size_t size, const char* buf);

bool
tw_bloomfilter_test(const struct tw_bloomfilter *bf,
                    size_t size, const char* buf);

#endif /* LIBTWIDDLE_BLOOMFILTER_H */
