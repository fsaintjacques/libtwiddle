from math import log
from hypothesis.strategies import integers, sets, just
from hypothesis.testrunners.forking import ForkingTestCase


single_set = integers(min_value=2**8, max_value=2**16).flatmap(lambda n:
               (just(n), sets(integers(min_value=0, max_value=n-1),
                         min_size=1, max_size=n,
                         average_size=n/log(n, 2))))


double_set = integers(min_value=2**8, max_value=2**16).flatmap(lambda n:
               (just(n), sets(integers(min_value=0, max_value=n-1),
                              min_size=1, max_size=n,
                              average_size=n/log(n, 2)),
                         sets(integers(min_value=0, max_value=n-1),
                              min_size=1, max_size=n,
                              average_size=n/log(n, 2))))


class TwiddleTest(ForkingTestCase):
  pass
