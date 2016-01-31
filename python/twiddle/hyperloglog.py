from c import libtwiddle
from ctypes import c_long, pointer

class HyperLogLog(object):
  def __init__(self, precision, ptr=None):
    self.hyperloglog = ptr if ptr else libtwiddle.tw_hyperloglog_new(precision)
    self.precision   = precision


  def __del__(self):
    if self.hyperloglog:
      libtwiddle.tw_hyperloglog_free(self.hyperloglog)


  @classmethod
  def copy(cls, h):
    return cls(h.precision, ptr=libtwiddle.tw_hyperloglog_clone(h.hyperloglog))


  @classmethod
  def from_iterable(cls, precision, iterable):
    hyperloglog = HyperLogLog(precision)

    for i in iterable:
      hyperloglog.add(i)

    return hyperloglog


  def __len__(self):
    return self.count()


  def add(self, x):
    h = pointer(c_long(hash(x)))
    libtwiddle.tw_hyperloglog_add(self.hyperloglog, h, 8)


  def __eq__(self, other):
    if not isinstance(other, HyperLogLog):
      return False

    return libtwiddle.tw_hyperloglog_equal(self.hyperloglog, other.hyperloglog)


  def __op(self, other, func, copy=lambda x: HyperLogLog.copy(x)):
    if not isinstance(other, HyperLogLog):
      raise ValueError("Must compare HyperLogLog to HyperLogLog")

    if self.precision != other.precision:
      raise ValueError("HyperLogLogs must be of equal precision to be comparable")

    ret = copy(self)

    func(other.hyperloglog, ret.hyperloglog)

    return ret


  def __iop(self, other, func):
    return self.__op(other, func, copy=lambda x: x)


  def __or__(self, other):
    return self.__op(other, libtwiddle.tw_hyperloglog_merge)


  def __ior__(self, other):
    return self.__iop(other, libtwiddle.tw_hyperloglog_merge)


  def count(self):
    return libtwiddle.tw_hyperloglog_count(self.hyperloglog)
