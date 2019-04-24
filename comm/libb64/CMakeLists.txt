project(libb64)
cmake_minimum_required(VERSION 3.0)
add_compile_options(-std=c++11 -fPIC)
file(GLOB LIBBASE64_INCLUDE_DIRS ./*.h)
set(LIBBASE64_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")
set(LIBBASE64_LIBRARIES libb64 CACHE INTERNAL "")


aux_source_directory(. LIBBASE64_SRC_LIST)
add_library(${PROJECT_NAME} ${LIBBASE64_SRC_LIST})
