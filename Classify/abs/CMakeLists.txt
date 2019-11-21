project(abs)
aux_source_directory(. DIR_ABS_SRCS)
file(GLOB ABSCLASS_INCLUDE ./*.h)
set(ABSCLASS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")
INCLUDE_DIRECTORIES(${COMM_INCLUDE_DIRS})


add_library(${PROJECT_NAME} ${DIR_ABS_SRCS})
target_link_libraries(${PROJECT_NAME} comm jsoncpp ${CURL_LIBRARY} boost_regex)

