# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------
# Copyright ©
#     2015, François Saint-Jacques
#     2011-2015, RedJack, LLC.
# All rights reserved.
#
# Please see the LICENSE.txt file in this distribution for license
# details.
# ----------------------------------------------------------------------

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h")
