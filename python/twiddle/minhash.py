from c import libtwiddle
from ctypes import c_long, pointer

class MinHash(object):
  def __init__(self, n_registers, ptr=None):
    self.minhash     = ptr if ptr else libtwiddle.tw_minhash_new(n_registers)
    self.n_registers = n_registers


  def __del__(self):
    if self.minhash:
      libtwiddle.tw_minhash_free(self.minhash)


  @classmethod
  def copy(cls, h):
    return cls(h.n_registers, ptr=libtwiddle.tw_minhash_clone(h.minhash))


  @classmethod
  def from_iterable(cls, n_registers, iterable):
    minhash = MinHash(n_registers)

    for i in iterable:
      minhash.add(i)

    return minhash


  def add(self, x):
    h = pointer(c_long(hash(x)))
    libtwiddle.tw_minhash_add(self.minhash, h, 8)


  def __eq__(self, other):
    if not isinstance(other, MinHash):
      return False

    return libtwiddle.tw_minhash_equal(self.minhash, other.minhash)


  def __op(self, other, func, copy=lambda x: MinHash.copy(x)):
    if not isinstance(other, MinHash):
      raise ValueError("Must compare MinHash to MinHash")

    if self.n_registers != other.n_registers:
      raise ValueError("MinHashs must be of equal n_registers to be comparable")

    ret = copy(self)

    func(other.minhash, ret.minhash)

    return ret


  def __iop(self, other, func):
    return self.__op(other, func, copy=lambda x: x)


  def __or__(self, other):
    return self.__op(other, libtwiddle.tw_minhash_merge)


  def __ior__(self, other):
    return self.__iop(other, libtwiddle.tw_minhash_merge)


  def estimate(self, other):
    if not isinstance(other, MinHash):
      raise ValueError("Must compare MinHash to MinHash")

    return libtwiddle.tw_minhash_estimate(self.minhash, other.minhash)
