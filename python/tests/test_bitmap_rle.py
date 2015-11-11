from hypothesis import given, example
from test_helpers import TwiddleTest, single_set, double_set
from twiddle import BitmapRLE

class TestBitmapRLE(TwiddleTest):
  @given(single_set)
  def test_bitmap_find_first_zero(self, n_xs):
    n, xs = n_xs
    x = BitmapRLE.from_indices(n, xs)

    expected = -1 if x.full() else min(set(range(0, n)) - xs)
    first = x.find_first_zero()
    assert(first == expected)


  @given(single_set)
  def test_bitmap_find_first_bit(self, n_xs):
    n, xs = n_xs
    x = BitmapRLE.from_indices(n, xs)

    expected = -1 if x.empty() else min(xs)
    first = x.find_first_bit()
    assert(first == expected)


  @given(single_set)
  def test_bitmap_negation(self, n_xs):
    n, xs = n_xs
    x = BitmapRLE.from_indices(n, xs)

    y = -x

    for idx in xs:
      assert(idx not in y)

    assert(x != y)
    assert(x == -y)


  @given(double_set)
  def test_bitmap_union(self, n_xs_ys):
    n, xs, ys = n_xs_ys
    x, y = BitmapRLE.from_indices(n, xs), BitmapRLE.from_indices(n, ys)

    # tests __or__
    z = x | y
    assert(z == BitmapRLE.from_indices(n, xs | ys))

    # tests __ior__
    x |= y
    assert(x == z)


  @given(double_set)
  def test_bitmap_intersection(self, n_xs_ys):
    n, xs, ys = n_xs_ys
    x, y = BitmapRLE.from_indices(n, xs), BitmapRLE.from_indices(n, ys)

    # tests __and__
    z = x & y
    assert(z == BitmapRLE.from_indices(n, xs & ys))

    # tests __iand__
    x &= y
    assert(x == z)
