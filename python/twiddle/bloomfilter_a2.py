from c import libtwiddle
from ctypes import c_int, c_long, pointer

class BloomFilterA2(object):
  def __init__(self, size, k, density, ptr=None):
    self.bloomfilter = ptr if ptr else libtwiddle.tw_bloomfilter_a2_new(size, k, density)
    self.size        = size
    self.k           = k
    self.density     = density


  def __del__(self):
    if self.bloomfilter:
      libtwiddle.tw_bloomfilter_a2_free(self.bloomfilter)


  @classmethod
  def copy(cls, b):
    return cls(b.size, b.k, ptr=libtwiddle.tw_bloomfilter_a2_clone(b.bloomfilter))


  @classmethod
  def from_iterable(cls, size, k, density, iterable):
    bloomfilter = BloomFilterA2(size, k, density)

    for i in iterable:
      bloomfilter.set(i)

    return bloomfilter


  def __len__(self):
    return self.size


  def __getitem__(self, x):
    h = pointer(c_long(hash(x)))
    return libtwiddle.tw_bloomfilter_a2_test(self.bloomfilter, h, 8)


  def set(self, x):
    h = pointer(c_long(hash(x)))
    libtwiddle.tw_bloomfilter_a2_set(self.bloomfilter, h, 8)


  def test(self, x):
    return self[x]


  def __contains__(self, x):
    return self[x]


  def __eq__(self, other):
    if not isinstance(other, BloomFilterA2):
      return False

    return libtwiddle.tw_bloomfilter_a2_equal(self.bloomfilter, other.bloomfilter)


  def __neg__(self):
    ret = BloomFilterA2.copy(self)
    libtwiddle.tw_bloomfilter_a2_not(ret.bloomfilter)
    return ret


  def __op(self, other, func, copy=lambda x: BloomFilterA2.copy(x)):
    if not isinstance(other, BloomFilterA2):
      raise ValueError("Must compare BloomFilterA2 to BloomFilterA2")

    if self.size != other.size:
      raise ValueError("BloomFiltersA2 must be of equal size to be comparable")

    ret = copy(self)

    func(other.bloomfilter, ret.bloomfilter)

    return ret


  def __iop(self, other, func):
    return self.__op(other, func, copy=lambda x: x)


  def __or__(self, other):
    return self.__op(other, libtwiddle.tw_bloomfilter_a2_union)


  def __ior__(self, other):
    return self.__iop(other, libtwiddle.tw_bloomfilter_a2_union)


  def __and__(self, other):
    return self.__op(other, libtwiddle.tw_bloomfilter_a2_intersection)


  def __iand__(self, other):
    return self.__iop(other, libtwiddle.tw_bloomfilter_a2_intersection)


  def __xor__(self, other):
    return self.__op(other, libtwiddle.tw_bloomfilter_a2_xor)


  def __ixor__(self, other):
    return self.__iop(other, libtwiddle.tw_bloomfilter_a2_xor)


  def empty(self):
    return libtwiddle.tw_bloomfilter_a2_empty(self.bloomfilter)


  def full(self):
    return libtwiddle.tw_bloomfilter_a2_full(self.bloomfilter)


  def count(self):
    return libtwiddle.tw_bloomfilter_a2_count(self.bloomfilter)


  def density(self):
    return libtwiddle.tw_bloomfilter_a2_density(self.bloomfilter)


  def zero(self):
    libtwiddle.tw_bloomfilter_a2_zero(self.bloomfilter)


  def fill(self):
    libtwiddle.tw_bloomfilter_a2_fill(self.bloomfilter)
