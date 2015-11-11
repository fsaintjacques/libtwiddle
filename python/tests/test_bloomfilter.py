from hypothesis import given
from test_helpers import TwiddleTest, single_set, double_set
from twiddle import BloomFilter

class TestBloomFilter(TwiddleTest):
  @given(single_set)
  def test_bloomfilter_negation(self, n_xs):
    n, xs = n_xs
    x = BloomFilter.from_iterable(n, 8, xs)
    y = -x
    assert(x == -y)


  @given(double_set)
  def test_bloomfilter_union(self, n_xs_ys):
    n, xs, ys = n_xs_ys
    x, y = BloomFilter.from_iterable(n, 8, xs), BloomFilter.from_iterable(n, 8, ys)

    # tests __or__
    z = x | y
    assert(z == BloomFilter.from_iterable(n, 8, xs | ys))

    # tests __ior__
    x |= y
    assert(x == z)


  @given(double_set)
  def test_bloomfilter_intersection(self, n_xs_ys):
    n, xs, ys = n_xs_ys
    x, y = BloomFilter.from_iterable(n, 8, xs), BloomFilter.from_iterable(n, 8, ys)
    zs = xs & ys

    # tests __and__
    z = x & y
    for e in zs:
      assert(e in z)

    # tests __iand__
    x &= y
    for e in zs:
      assert(e in x)
