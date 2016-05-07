from c import libtwiddle

class Bitmap(object):
  def __init__(self, size, ptr=None):
    self.bitmap = ptr if ptr else libtwiddle.tw_bitmap_new(size)
    self.size   = size


  def __del__(self):
    if self.bitmap:
      libtwiddle.tw_bitmap_free(self.bitmap)


  @classmethod
  def copy(cls, b):
    return cls(b.size, ptr=libtwiddle.tw_bitmap_clone(b.bitmap))


  @classmethod
  def from_indices(cls, size, indices):
    bitmap = Bitmap(size)

    for idx in indices:
      bitmap[idx] = True

    return bitmap


  def __len__(self):
    return self.size


  def __getitem__(self, i):
    if (i < 0) or (i >= len(self)):
      raise ValueError("index must be within bitmap bounds")
    return libtwiddle.tw_bitmap_test(self.bitmap, i)


  def __setitem__(self, i, value):
    if (i < 0) or (i >= len(self)):
      raise ValueError("index must be within bitmap bounds")

    if not isinstance(value, bool):
      raise ValueError("Bitmap accepts only bool values")

    if value:
      libtwiddle.tw_bitmap_set(self.bitmap, i)
    else:
      libtwiddle.tw_bitmap_clear(self.bitmap, i)


  def __contains__(self, x):
    if (x < 0) or (x > self.size - 1):
      return False

    return self[x]


  def __eq__(self, other):
    if not isinstance(other, Bitmap):
      return False

    return libtwiddle.tw_bitmap_equal(self.bitmap, other.bitmap)


  def __neg__(self):
    ret = Bitmap.copy(self)
    libtwiddle.tw_bitmap_not(ret.bitmap)
    return ret


  def __op(self, other, func, copy=lambda x: Bitmap.copy(x)):
    if not isinstance(other, Bitmap):
      raise ValueError("Must compare Bitmap to Bitmap")

    if self.size != other.size:
      raise ValueError("Bitmaps must be of equal size to be comparable")

    ret = copy(self)

    func(other.bitmap, ret.bitmap)

    return ret


  def __iop(self, other, func):
    return self.__op(other, func, copy=lambda x: x)


  def __or__(self, other):
    return self.__op(other, libtwiddle.tw_bitmap_union)


  def __ior__(self, other):
    return self.__iop(other, libtwiddle.tw_bitmap_union)


  def __and__(self, other):
    return self.__op(other, libtwiddle.tw_bitmap_intersection)


  def __iand__(self, other):
    return self.__iop(other, libtwiddle.tw_bitmap_intersection)


  def __xor__(self, other):
    return self.__op(other, libtwiddle.tw_bitmap_xor)


  def __ixor__(self, other):
    return self.__iop(other, libtwiddle.tw_bitmap_xor)


  def empty(self):
    return libtwiddle.tw_bitmap_empty(self.bitmap)


  def full(self):
    return libtwiddle.tw_bitmap_full(self.bitmap)


  def count(self):
    return libtwiddle.tw_bitmap_count(self.bitmap)


  def density(self):
    return libtwiddle.tw_bitmap_density(self.bitmap)


  def zero(self):
    libtwiddle.tw_bitmap_zero(self.bitmap)


  def fill(self):
    libtwiddle.tw_bitmap_fill(self.bitmap)


  def find_first_zero(self):
    return libtwiddle.tw_bitmap_find_first_zero(self.bitmap)


  def find_first_bit(self):
    return libtwiddle.tw_bitmap_find_first_bit(self.bitmap)
