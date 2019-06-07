project(control)
aux_source_directory(. DIR_CONTROL_SRCS)
file(GLOB CONTROL_INCLUDE ./*.h)
set(CONTROL_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")
INCLUDE_DIRECTORIES(
    ${ABSCLASS_INCLUDE}
    )

add_library(${PROJECT_NAME} ${DIR_CONTROL_SRCS})
target_link_libraries(${PROJECT_NAME} abs )
