project(voxer)
aux_source_directory(. VOXER_APP_SRCS)

INCLUDE_DIRECTORIES(
    ${ABSCLASS_INCLUDE}
    ${COMM_INCLUDE_DIRS}
    ${APP_INCLUDE}
    )
add_library(${PROJECT_NAME} ${VOXER_APP_SRCS})
target_link_libraries(${PROJECT_NAME} abs comm jsoncpp ${CURL_LIBRARY})
