from c import libtwiddle

class BitmapRLE(object):
  def __init__(self, size, ptr=None):
    self.bitmap = ptr if ptr else libtwiddle.tw_bitmap_rle_new(size)
    self.size   = size


  def __del__(self):
    if self.bitmap:
      libtwiddle.tw_bitmap_rle_free(self.bitmap)


  @classmethod
  def copy(cls, b):
    return cls(b.size, ptr=libtwiddle.tw_bitmap_rle_clone(b.bitmap))


  @classmethod
  def from_indices(cls, size, indices):
    bitmap = BitmapRLE(size)

    for idx in sorted(indices):
      bitmap[idx] = True

    return bitmap


  def __len__(self):
    return self.size


  def __getitem__(self, i):
    if (i < 0) or (i >= len(self)):
      raise ValueError("index must be within bitmap bounds")
    return libtwiddle.tw_bitmap_rle_test(self.bitmap, i)


  def __setitem__(self, i, value):
    if (i < 0) or (i >= len(self)):
      raise ValueError("index must be within bitmap bounds")

    if not isinstance(value, bool):
      raise ValueError("BitmapRLE accepts only bool values")

    if value:
      libtwiddle.tw_bitmap_rle_set(self.bitmap, i)


  def __contains__(self, x):
    if (x < 0) or (x > self.size - 1):
      return False

    return self[x]


  def __eq__(self, other):
    if not isinstance(other, BitmapRLE):
      return False

    return libtwiddle.tw_bitmap_rle_equal(self.bitmap, other.bitmap)


  def __neg__(self):
    ret = BitmapRLE.copy(self)
    libtwiddle.tw_bitmap_rle_not(self.bitmap, ret.bitmap)
    return ret


  def __op(self, other, func, copy=lambda x: BitmapRLE.copy(x)):
    if not isinstance(other, BitmapRLE):
      raise ValueError("Must compare BitmapRLE to BitmapRLE")

    if self.size != other.size:
      raise ValueError("BitmapRLE must be of equal size to be comparable")

    ret = copy(self)

    func(self.bitmap, other.bitmap, ret.bitmap)

    return ret


  def __iop(self, other, func):
    return self.__op(other, func)


  def __or__(self, other):
    return self.__op(other, libtwiddle.tw_bitmap_rle_union)


  def __ior__(self, other):
    return self.__iop(other, libtwiddle.tw_bitmap_rle_union)


  def __and__(self, other):
    return self.__op(other, libtwiddle.tw_bitmap_rle_intersection)


  def __iand__(self, other):
    return self.__iop(other, libtwiddle.tw_bitmap_rle_intersection)


  def empty(self):
    return libtwiddle.tw_bitmap_rle_empty(self.bitmap)


  def full(self):
    return libtwiddle.tw_bitmap_rle_full(self.bitmap)


  def count(self):
    return libtwiddle.tw_bitmap_rle_count(self.bitmap)


  def density(self):
    return libtwiddle.tw_bitmap_rle_density(self.bitmap)


  def zero(self):
    libtwiddle.tw_bitmap_rle_zero(self.bitmap)


  def fill(self):
    libtwiddle.tw_bitmap_rle_fill(self.bitmap)


  def find_first_zero(self):
    return libtwiddle.tw_bitmap_rle_find_first_zero(self.bitmap)


  def find_first_bit(self):
    return libtwiddle.tw_bitmap_rle_find_first_bit(self.bitmap)
