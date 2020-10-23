project(app)
aux_source_directory(. DIR_APP_SRCS)
file(GLOB APP_INCLUDE ./*.h)
set(APP_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")

add_subdirectory(fbm)
add_subdirectory(voxer)
add_subdirectory(imo)
add_subdirectory(telegram)
add_subdirectory(skype)
INCLUDE_DIRECTORIES(
    ${COMM_INCLUDE_DIRS}
    ${ABSCLASS_INCLUDE}
    )
add_library(${PROJECT_NAME} ${DIR_APP_SRCS})
target_link_libraries(${PROJECT_NAME} comm abs fbm voxer imo telegram skype)

