# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------
# Copyright ©
#     2015, François Saint-Jacques
#     2011-2015, RedJack, LLC.
# All rights reserved.
#
# Please see the COPYING file in this distribution for license details.
# ----------------------------------------------------------------------

#-----------------------------------------------------------------------
# Build the test cases

# Add calls to make_test for each .c test case in the tests/ directory.
add_c_test(example-bitmap)
add_c_test(example-bitmap-rle)
add_c_test(example-bloomfilter)
add_c_test(example-hyperloglog)
