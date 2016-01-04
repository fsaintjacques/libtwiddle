from math import log
from hypothesis import given
from test_helpers import double_set
from twiddle import MinHash
from unittest import TestCase

class TestMinHash(TestCase):
  @given(double_set)
  def test_bloomfilter_union(self, n_xs_ys):
    n, xs, ys = n_xs_ys
    x, y = MinHash.from_iterable(n, xs), MinHash.from_iterable(n, ys)

    # tests __or__
    z = x | y
    assert(x != z)

    # tests __ior__
    x |= y
    assert(x == z)
