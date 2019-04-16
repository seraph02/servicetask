project(telegram)
aux_source_directory(. TELEGRAM_APP_SRCS)

INCLUDE_DIRECTORIES(
    ${ABSCLASS_INCLUDE}
    )
add_library(${PROJECT_NAME} ${TELEGRAM_APP_SRCS})
target_link_libraries(${PROJECT_NAME} abs )
