project(fbmcontrol)
aux_source_directory(. FBM_CONTROL_SRCS)
INCLUDE_DIRECTORIES(
    ${ABSCLASS_INCLUDE}
    )
add_library(${PROJECT_NAME} ${FBM_CONTROL_SRCS})
target_link_libraries(${PROJECT_NAME} abs )
