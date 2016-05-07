from math import log
from hypothesis import given
from test_helpers import TwiddleTest, single_set, double_set
from twiddle import HyperLogLog
from unittest import TestCase

class TestHyperLogLog(TestCase):
  @given(double_set)
  def test_bloomfilter_union(self, n_xs_ys):
    n, xs, ys = n_xs_ys
    n = int(log(n, 2))
    x, y = HyperLogLog.from_iterable(n, xs), HyperLogLog.from_iterable(n, ys)

    # tests __or__
    z = x | y
    assert(x != z)

    # tests __ior__
    x |= y
    assert(x == z)
