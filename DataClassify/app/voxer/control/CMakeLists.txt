project(voxercontrol)
aux_source_directory(. VOXER_CONTROL_SRCS)
INCLUDE_DIRECTORIES(
    ${ABSCLASS_INCLUDE}
    )
add_library(${PROJECT_NAME} ${VOXER_CONTROL_SRCS})
target_link_libraries(${PROJECT_NAME} abs )
