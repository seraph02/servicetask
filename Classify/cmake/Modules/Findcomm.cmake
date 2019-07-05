# - C++ Requests, Curl for People. Copied from whoshuu/cpr (MIT license)
# This module is a libcurl wrapper written in modern C++.
# It provides an easy, intuitive, and efficient interface to
# a host of networking methods.
#
# Finding this module will define the following variables:
#  CPR_FOUND - True if the core library has been found
#  CPR_LIBRARIES - Path to the core library archive
#  CPR_INCLUDE_DIRS - Path to the include directories. Gives access
#                     to cpr.h, which must be included in every
#                     file that uses this interface

find_path(COMM_INCLUDE_DIR
          NAMES cpr.h cpr/cpr.h)

find_library(COMM_LIBRARY
             NAMES comm
             HINTS ${COMM_LIBRARY_ROOT})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(COMM REQUIRED_VARS COMM_LIBRARY COMM_INCLUDE_DIR)

if(COMM_FOUND)
    set(COMM_LIBRARIES ${COMM_LIBRARY} CACHE INTERNAL "")
    set(COMM_INCLUDE_DIRS ${COMM_INCLUDE_DIR} CACHE INTERNAL "")
endif()
