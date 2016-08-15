#include "check/check.h"
#include <inttypes.h>
#include <stdio.h>

#undef _ck_assert_ptr
#define _ck_assert_ptr(X, OP, Y)                                               \
  do {                                                                         \
    const void *_ck_x = (X);                                                   \
    const void *_ck_y = (Y);                                                   \
    ck_assert_msg(_ck_x OP _ck_y,                                              \
                  "Assertion '%s' failed: %s == %#x, %s == %#x",               \
                  #X " " #OP " " #Y, #X, _ck_x, #Y, _ck_y);                    \
  } while (0)

#define DESCRIBE_TEST fprintf(stderr, "--- %s\n", __func__);

#define _ck_assert_type(type, typefmt, X, OP, Y)                               \
  do {                                                                         \
    type _ck_x = (X);                                                          \
    type _ck_y = (Y);                                                          \
    ck_assert_msg(_ck_x OP _ck_y, "Assertion '%s' failed: %s == %" typefmt     \
                                  " , %s == %" typefmt,                        \
                  #X " " #OP " " #Y, #X, _ck_x, #Y, _ck_y);                    \
  } while (0)

#define ck_assert_uint8_t_eq(X, Y) _ck_assert_type(uint8_t, PRIu8, X, ==, Y)
#define ck_assert_uint8_t_ne(X, Y) _ck_assert_type(uint8_t, PRIu8, X, !=, Y)
#define ck_assert_uint8_t_lt(X, Y) _ck_assert_type(uint8_t, PRIu8, X, <, Y)
#define ck_assert_uint8_t_gt(X, Y) _ck_assert_type(uint8_t, PRIu8, X, >, Y)
#define ck_assert_uint8_t_le(X, Y) _ck_assert_type(uint8_t, PRIu8, X, <=, Y)
#define ck_assert_uint8_t_ge(X, Y) _ck_assert_type(uint8_t, PRIu8, X, >=, Y)

#define ck_assert_uint16_t_eq(X, Y) _ck_assert_type(uint16_t, PRIu16, X, ==, Y)
#define ck_assert_uint16_t_ne(X, Y) _ck_assert_type(uint16_t, PRIu16, X, !=, Y)
#define ck_assert_uint16_t_lt(X, Y) _ck_assert_type(uint16_t, PRIu16, X, <, Y)
#define ck_assert_uint16_t_gt(X, Y) _ck_assert_type(uint16_t, PRIu16, X, >, Y)
#define ck_assert_uint16_t_le(X, Y) _ck_assert_type(uint16_t, PRIu16, X, <=, Y)
#define ck_assert_uint16_t_ge(X, Y) _ck_assert_type(uint16_t, PRIu16, X, >=, Y)

#define ck_assert_uint32_t_eq(X, Y) _ck_assert_type(uint32_t, PRIu32, X, ==, Y)
#define ck_assert_uint32_t_ne(X, Y) _ck_assert_type(uint32_t, PRIu32, X, !=, Y)
#define ck_assert_uint32_t_lt(X, Y) _ck_assert_type(uint32_t, PRIu32, X, <, Y)
#define ck_assert_uint32_t_gt(X, Y) _ck_assert_type(uint32_t, PRIu32, X, >, Y)
#define ck_assert_uint32_t_le(X, Y) _ck_assert_type(uint32_t, PRIu32, X, <=, Y)
#define ck_assert_uint32_t_ge(X, Y) _ck_assert_type(uint32_t, PRIu32, X, >=, Y)

#define ck_assert_uint64_t_eq(X, Y) _ck_assert_type(uint64_t, PRIu64, X, ==, Y)
#define ck_assert_uint64_t_ne(X, Y) _ck_assert_type(uint64_t, PRIu64, X, !=, Y)
#define ck_assert_uint64_t_lt(X, Y) _ck_assert_type(uint64_t, PRIu64, X, <, Y)
#define ck_assert_uint64_t_gt(X, Y) _ck_assert_type(uint64_t, PRIu64, X, >, Y)
#define ck_assert_uint64_t_le(X, Y) _ck_assert_type(uint64_t, PRIu64, X, <=, Y)
#define ck_assert_uint64_t_ge(X, Y) _ck_assert_type(uint64_t, PRIu64, X, >=, Y)

#define ck_assert_int8_t_eq(X, Y) _ck_assert_type(int8_t, PRId8, X, ==, Y)
#define ck_assert_int8_t_ne(X, Y) _ck_assert_type(int8_t, PRId8, X, !=, Y)
#define ck_assert_int8_t_lt(X, Y) _ck_assert_type(int8_t, PRId8, X, <, Y)
#define ck_assert_int8_t_gt(X, Y) _ck_assert_type(int8_t, PRId8, X, >, Y)
#define ck_assert_int8_t_le(X, Y) _ck_assert_type(int8_t, PRId8, X, <=, Y)
#define ck_assert_int8_t_ge(X, Y) _ck_assert_type(int8_t, PRId8, X, >=, Y)

#define ck_assert_int16_t_eq(X, Y) _ck_assert_type(int16_t, PRId16, X, ==, Y)
#define ck_assert_int16_t_ne(X, Y) _ck_assert_type(int16_t, PRId16, X, !=, Y)
#define ck_assert_int16_t_lt(X, Y) _ck_assert_type(int16_t, PRId16, X, <, Y)
#define ck_assert_int16_t_gt(X, Y) _ck_assert_type(int16_t, PRId16, X, >, Y)
#define ck_assert_int16_t_le(X, Y) _ck_assert_type(int16_t, PRId16, X, <=, Y)
#define ck_assert_int16_t_ge(X, Y) _ck_assert_type(int16_t, PRId16, X, >=, Y)

#define ck_assert_int32_t_eq(X, Y) _ck_assert_type(int32_t, PRId32, X, ==, Y)
#define ck_assert_int32_t_ne(X, Y) _ck_assert_type(int32_t, PRId32, X, !=, Y)
#define ck_assert_int32_t_lt(X, Y) _ck_assert_type(int32_t, PRId32, X, <, Y)
#define ck_assert_int32_t_gt(X, Y) _ck_assert_type(int32_t, PRId32, X, >, Y)
#define ck_assert_int32_t_le(X, Y) _ck_assert_type(int32_t, PRId32, X, <=, Y)
#define ck_assert_int32_t_ge(X, Y) _ck_assert_type(int32_t, PRId32, X, >=, Y)

#define ck_assert_int64_t_eq(X, Y) _ck_assert_type(int64_t, PRId64, X, ==, Y)
#define ck_assert_int64_t_ne(X, Y) _ck_assert_type(int64_t, PRId64, X, !=, Y)
#define ck_assert_int64_t_lt(X, Y) _ck_assert_type(int64_t, PRId64, X, <, Y)
#define ck_assert_int64_t_gt(X, Y) _ck_assert_type(int64_t, PRId64, X, >, Y)
#define ck_assert_int64_t_le(X, Y) _ck_assert_type(int64_t, PRId64, X, <=, Y)
#define ck_assert_int64_t_ge(X, Y) _ck_assert_type(int64_t, PRId64, X, >=, Y)

/*
#undef _ck_assert_type_all
#undef _ck_assert_type
*/
