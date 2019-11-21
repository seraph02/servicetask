project(telegram)
aux_source_directory(. TELEGRAM_APP_SRCS)
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
include_directories("./")
INCLUDE_DIRECTORIES(
    ${ABSCLASS_INCLUDE}
    ${COMM_INCLUDE_DIRS}
    )
add_library(${PROJECT_NAME} ${TELEGRAM_APP_SRCS})
target_link_libraries(${PROJECT_NAME} abs comm ${PYTHON_LIBIARIES})
