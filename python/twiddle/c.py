from ctypes import *
from ctypes.util import find_library
from os import environ, path

def find_twiddle_so():
  libpath = find_library("twiddle")

  if not libpath:
    travis_home_dir = environ.get("TRAVIS_BUILD_DIR")
    if travis_home_dir:
      libpath = path.join(travis_home_dir, "build", "src", "libtwiddle.so")
    else:
      raise ImportError("Couldn't find libtwiddle.so")

  return libpath

libtwiddle = CDLL(find_twiddle_so())

# BITMAP

libtwiddle.tw_bitmap_new.argtypes = [c_uint]
libtwiddle.tw_bitmap_new.restype  = c_void_p

libtwiddle.tw_bitmap_free.argtypes = [c_void_p]
libtwiddle.tw_bitmap_free.restype  = None

libtwiddle.tw_bitmap_copy.argtypes = [c_void_p, c_void_p]
libtwiddle.tw_bitmap_copy.restype  = c_void_p

libtwiddle.tw_bitmap_clone.argtypes = [c_void_p]
libtwiddle.tw_bitmap_clone.restype  = c_void_p

libtwiddle.tw_bitmap_set.argtypes = [c_void_p, c_uint]
libtwiddle.tw_bitmap_set.restype  = None

libtwiddle.tw_bitmap_clear.argtypes = [c_void_p, c_uint]
libtwiddle.tw_bitmap_clear.restype  = None

libtwiddle.tw_bitmap_test.argtypes = [c_void_p, c_uint]
libtwiddle.tw_bitmap_test.restype  = c_bool

libtwiddle.tw_bitmap_test_and_clear.argtypes = [c_void_p, c_uint]
libtwiddle.tw_bitmap_test_and_clear.restype  = c_bool

libtwiddle.tw_bitmap_empty.argtypes = [c_void_p]
libtwiddle.tw_bitmap_empty.restype  = c_bool

libtwiddle.tw_bitmap_full.argtypes = [c_void_p]
libtwiddle.tw_bitmap_full.restype  = c_bool

libtwiddle.tw_bitmap_count.argtypes = [c_void_p]
libtwiddle.tw_bitmap_count.restype  = c_uint

libtwiddle.tw_bitmap_density.argtypes = [c_void_p]
libtwiddle.tw_bitmap_density.restype  = c_float

libtwiddle.tw_bitmap_zero.argtypes = [c_void_p]
libtwiddle.tw_bitmap_zero.restype  = c_void_p

libtwiddle.tw_bitmap_fill.argtypes = [c_void_p]
libtwiddle.tw_bitmap_fill.restype  = c_void_p

libtwiddle.tw_bitmap_find_first_zero.argtypes = [c_void_p]
libtwiddle.tw_bitmap_find_first_zero.restype  = c_int64

libtwiddle.tw_bitmap_find_first_bit.argtypes = [c_void_p]
libtwiddle.tw_bitmap_find_first_bit.restype  = c_int64

libtwiddle.tw_bitmap_not.argtypes = [c_void_p]
libtwiddle.tw_bitmap_not.restype  = c_void_p

libtwiddle.tw_bitmap_equal.argtypes = [c_void_p, c_void_p]
libtwiddle.tw_bitmap_equal.restype  = c_bool

libtwiddle.tw_bitmap_union.argtypes = [c_void_p, c_void_p]
libtwiddle.tw_bitmap_union.restype  = c_void_p

libtwiddle.tw_bitmap_intersection.argtypes = [c_void_p, c_void_p]
libtwiddle.tw_bitmap_intersection.restype  = c_void_p

libtwiddle.tw_bitmap_xor.argtypes = [c_void_p, c_void_p]
libtwiddle.tw_bitmap_xor.restype  = c_void_p

# BITMAP_RLE

libtwiddle.tw_bitmap_rle_new.argtypes = [c_uint]
libtwiddle.tw_bitmap_rle_new.restype  = c_void_p

libtwiddle.tw_bitmap_rle_free.argtypes = [c_void_p]
libtwiddle.tw_bitmap_rle_free.restype  = None

libtwiddle.tw_bitmap_rle_copy.argtypes = [c_void_p, c_void_p]
libtwiddle.tw_bitmap_rle_copy.restype  = c_void_p

libtwiddle.tw_bitmap_rle_clone.argtypes = [c_void_p]
libtwiddle.tw_bitmap_rle_clone.restype  = c_void_p

libtwiddle.tw_bitmap_rle_set.argtypes = [c_void_p, c_uint]
libtwiddle.tw_bitmap_rle_set.restype  = None

libtwiddle.tw_bitmap_rle_test.argtypes = [c_void_p, c_uint]
libtwiddle.tw_bitmap_rle_test.restype  = c_bool

libtwiddle.tw_bitmap_rle_empty.argtypes = [c_void_p]
libtwiddle.tw_bitmap_rle_empty.restype  = c_bool

libtwiddle.tw_bitmap_rle_full.argtypes = [c_void_p]
libtwiddle.tw_bitmap_rle_full.restype  = c_bool

libtwiddle.tw_bitmap_rle_count.argtypes = [c_void_p]
libtwiddle.tw_bitmap_rle_count.restype  = c_uint

libtwiddle.tw_bitmap_rle_density.argtypes = [c_void_p]
libtwiddle.tw_bitmap_rle_density.restype  = c_float

libtwiddle.tw_bitmap_rle_zero.argtypes = [c_void_p]
libtwiddle.tw_bitmap_rle_zero.restype  = c_void_p

libtwiddle.tw_bitmap_rle_fill.argtypes = [c_void_p]
libtwiddle.tw_bitmap_rle_fill.restype  = c_void_p

libtwiddle.tw_bitmap_rle_find_first_zero.argtypes = [c_void_p]
libtwiddle.tw_bitmap_rle_find_first_zero.restype  = c_int64

libtwiddle.tw_bitmap_rle_find_first_bit.argtypes = [c_void_p]
libtwiddle.tw_bitmap_rle_find_first_bit.restype  = c_int64

libtwiddle.tw_bitmap_rle_not.argtypes = [c_void_p, c_void_p]
libtwiddle.tw_bitmap_rle_not.restype  = c_void_p

libtwiddle.tw_bitmap_rle_equal.argtypes = [c_void_p, c_void_p]
libtwiddle.tw_bitmap_rle_equal.restype  = c_bool

libtwiddle.tw_bitmap_rle_union.argtypes = [c_void_p, c_void_p, c_void_p]
libtwiddle.tw_bitmap_rle_union.restype  = c_void_p

libtwiddle.tw_bitmap_rle_intersection.argtypes = [c_void_p, c_void_p, c_void_p]
libtwiddle.tw_bitmap_rle_intersection.restype  = c_void_p

# BLOOMFILTER

libtwiddle.tw_bloomfilter_new.argtypes = [c_uint, c_uint]
libtwiddle.tw_bloomfilter_new.restype  = c_void_p

libtwiddle.tw_bloomfilter_free.argtypes = [c_void_p]
libtwiddle.tw_bloomfilter_free.restype  = None

libtwiddle.tw_bloomfilter_copy.argtypes = [c_void_p, c_void_p]
libtwiddle.tw_bloomfilter_copy.restype  = c_void_p

libtwiddle.tw_bloomfilter_clone.argtypes = [c_void_p]
libtwiddle.tw_bloomfilter_clone.restype  = c_void_p

libtwiddle.tw_bloomfilter_set.argtypes = [c_void_p, c_uint, c_void_p]
libtwiddle.tw_bloomfilter_set.restype  = None

libtwiddle.tw_bloomfilter_test.argtypes = [c_void_p, c_uint, c_void_p]
libtwiddle.tw_bloomfilter_test.restype  = c_bool

libtwiddle.tw_bloomfilter_empty.argtypes = [c_void_p]
libtwiddle.tw_bloomfilter_empty.restype  = c_bool

libtwiddle.tw_bloomfilter_full.argtypes = [c_void_p]
libtwiddle.tw_bloomfilter_full.restype  = c_bool

libtwiddle.tw_bloomfilter_count.argtypes = [c_void_p]
libtwiddle.tw_bloomfilter_count.restype  = c_uint

libtwiddle.tw_bloomfilter_density.argtypes = [c_void_p]
libtwiddle.tw_bloomfilter_density.restype  = c_float

libtwiddle.tw_bloomfilter_zero.argtypes = [c_void_p]
libtwiddle.tw_bloomfilter_zero.restype  = c_void_p

libtwiddle.tw_bloomfilter_fill.argtypes = [c_void_p]
libtwiddle.tw_bloomfilter_fill.restype  = c_void_p

libtwiddle.tw_bloomfilter_not.argtypes = [c_void_p]
libtwiddle.tw_bloomfilter_not.restype  = c_void_p

libtwiddle.tw_bloomfilter_equal.argtypes = [c_void_p, c_void_p]
libtwiddle.tw_bloomfilter_equal.restype  = c_bool

libtwiddle.tw_bloomfilter_union.argtypes = [c_void_p, c_void_p]
libtwiddle.tw_bloomfilter_union.restype  = c_void_p

libtwiddle.tw_bloomfilter_intersection.argtypes = [c_void_p, c_void_p]
libtwiddle.tw_bloomfilter_intersection.restype  = c_void_p

libtwiddle.tw_bloomfilter_xor.argtypes = [c_void_p, c_void_p]
libtwiddle.tw_bloomfilter_xor.restype  = c_void_p

 # HYPERLOGLOG

libtwiddle.tw_hyperloglog_new.argtypes = [c_ushort]
libtwiddle.tw_hyperloglog_new.restype  = c_void_p

libtwiddle.tw_hyperloglog_free.argtypes = [c_void_p]
libtwiddle.tw_hyperloglog_free.restype  = None

libtwiddle.tw_hyperloglog_copy.argtypes = [c_void_p, c_void_p]
libtwiddle.tw_hyperloglog_copy.restype  = c_void_p

libtwiddle.tw_hyperloglog_clone.argtypes = [c_void_p]
libtwiddle.tw_hyperloglog_clone.restype  = c_void_p

libtwiddle.tw_hyperloglog_add_hashed.argtypes = [c_void_p, c_ulong]
libtwiddle.tw_hyperloglog_add_hashed.restype  = None

libtwiddle.tw_hyperloglog_add.argtypes = [c_void_p, c_uint, c_void_p]
libtwiddle.tw_hyperloglog_add.restype  = None

libtwiddle.tw_hyperloglog_count.argtypes = [c_void_p]
libtwiddle.tw_hyperloglog_count.restype  = c_double

libtwiddle.tw_hyperloglog_equal.argtypes = [c_void_p, c_void_p]
libtwiddle.tw_hyperloglog_equal.restype  = c_bool

libtwiddle.tw_hyperloglog_merge.argtypes = [c_void_p, c_void_p]
libtwiddle.tw_hyperloglog_merge.restype  = c_void_p
