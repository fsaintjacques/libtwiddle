from c import libtwiddle
from ctypes import c_int, c_long, pointer

class BloomFilter(object):
  def __init__(self, size, k, ptr=None):
    self.bloomfilter = ptr if ptr else libtwiddle.tw_bloomfilter_new(size, k)
    self.size        = size
    self.k           = k


  def __del__(self):
    if self.bloomfilter:
      libtwiddle.tw_bloomfilter_free(self.bloomfilter)


  @classmethod
  def copy(cls, b):
    return cls(b.size, b.k, ptr=libtwiddle.tw_bloomfilter_clone(b.bloomfilter))


  @classmethod
  def from_iterable(cls, size, k, iterable):
    bloomfilter = BloomFilter(size, k)

    for i in iterable:
      bloomfilter.set(i)

    return bloomfilter


  def __len__(self):
    return self.size


  def __getitem__(self, x):
    h = pointer(c_long(hash(x)))
    return libtwiddle.tw_bloomfilter_test(self.bloomfilter, 8, h)


  def set(self, x):
    h = pointer(c_long(hash(x)))
    libtwiddle.tw_bloomfilter_set(self.bloomfilter, 8, h)


  def test(self, x):
    return self[x]


  def __contains__(self, x):
    return self[x]


  def __eq__(self, other):
    if not isinstance(other, BloomFilter):
      return False

    return libtwiddle.tw_bloomfilter_equal(self.bloomfilter, other.bloomfilter)


  def __neg__(self):
    ret = BloomFilter.copy(self)
    libtwiddle.tw_bloomfilter_not(ret.bloomfilter)
    return ret


  def __op(self, other, func, copy=lambda x: BloomFilter.copy(x)):
    if not isinstance(other, BloomFilter):
      raise ValueError("Must compare BloomFilter to BloomFilter")

    if self.size != other.size:
      raise ValueError("BloomFilters must be of equal size to be comparable")

    ret = copy(self)

    func(other.bloomfilter, ret.bloomfilter)

    return ret


  def __iop(self, other, func):
    return self.__op(other, func, copy=lambda x: x)


  def __or__(self, other):
    return self.__op(other, libtwiddle.tw_bloomfilter_union)


  def __ior__(self, other):
    return self.__iop(other, libtwiddle.tw_bloomfilter_union)


  def __and__(self, other):
    return self.__op(other, libtwiddle.tw_bloomfilter_intersection)


  def __iand__(self, other):
    return self.__iop(other, libtwiddle.tw_bloomfilter_intersection)


  def __xor__(self, other):
    return self.__op(other, libtwiddle.tw_bloomfilter_xor)


  def __ixor__(self, other):
    return self.__iop(other, libtwiddle.tw_bloomfilter_xor)


  def empty(self):
    return libtwiddle.tw_bloomfilter_empty(self.bloomfilter)


  def full(self):
    return libtwiddle.tw_bloomfilter_full(self.bloomfilter)


  def count(self):
    return libtwiddle.tw_bloomfilter_count(self.bloomfilter)


  def density(self):
    return libtwiddle.tw_bloomfilter_density(self.bloomfilter)


  def zero(self):
    libtwiddle.tw_bloomfilter_zero(self.bloomfilter)


  def fill(self):
    libtwiddle.tw_bloomfilter_fill(self.bloomfilter)
