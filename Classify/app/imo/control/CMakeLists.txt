project(imocontrol)
aux_source_directory(. IMO_CONTROL_SRCS)
INCLUDE_DIRECTORIES(
    ${ABSCLASS_INCLUDE}
    )
add_library(${PROJECT_NAME} ${IMO_CONTROL_SRCS})
target_link_libraries(${PROJECT_NAME} abs )
