set(BUILD_SHARED_LIBS YES CACHE BOOL "")
set(BUILD_STATIC_LIBS NO CACHE BOOL "")


    find_package(PkgConfig REQUIRED)
    pkg_check_modules(JSONCPP jsoncpp)
    if(NOT JSONCPP_FOUND)
        message(FATAL_ERROR "JsonCpp library not found.")
    endif()


set(JSONCPP_FOUND ${JSONCPP_FOUND} CACHE INTERNAL "")
set(JSONCPP_LIBRARIES ${JSONCPP_LIBRARY} CACHE INTERNAL "")


if(USE_SYSTEM_CPR)
    find_package(CPR REQUIRED)
    if(NOT CPR_FOUND)
        message(FATAL_ERROR "CPR library not found.")
    endif()
else(NOT CPR_FOUND) # May some lib already brings CPR lib.
    set(BUILD_CPR_TESTS NO CACHE BOOL "")
    find_package(CURL)
    if(CURL_FOUND)
        set(USE_SYSTEM_CURL YES CACHE BOOL "")
    endif()
    add_subdirectory(cpr)
endif()
set(CPR_FOUND ${CPR_FOUND} CACHE INTERNAL "")
set(CPR_LIBRARIES ${CPR_LIBRARIES} CACHE INTERNAL "")
set(CPR_INCLUDE_DIRS ${CPR_INCLUDE_DIRS} CACHE INTERNAL "")
