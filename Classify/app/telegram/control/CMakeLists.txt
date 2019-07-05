project(telegramcontrol)
aux_source_directory(. TELEGRAM_CONTROL_SRCS)
INCLUDE_DIRECTORIES(
    ${ABSCLASS_INCLUDE}
    )
add_library(${PROJECT_NAME} ${TELEGRAM_CONTROL_SRCS})
target_link_libraries(${PROJECT_NAME} abs )
