project(comm)
cmake_minimum_required(VERSION 3.0)
add_compile_options(-std=c++11 -fPIC)
file(GLOB COMM_INCLUDE_DIRS ./*.h)
set(COMM_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")
set(COMM_LIBRARIES comm CACHE INTERNAL "")


aux_source_directory(. COMM_SRC_LIST)
add_library(${PROJECT_NAME} ${COMM_SRC_LIST})

