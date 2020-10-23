project(skype)
aux_source_directory(. SKY_APP_SRCS)

INCLUDE_DIRECTORIES(
    ${ABSCLASS_INCLUDE}
    ${COMM_INCLUDE_DIRS}
    ${APP_INCLUDE}
    )
add_library(${PROJECT_NAME} ${SKY_APP_SRCS})
target_link_libraries(${PROJECT_NAME} abs comm jsoncpp ${CURL_LIBRARY} boost_regex)
