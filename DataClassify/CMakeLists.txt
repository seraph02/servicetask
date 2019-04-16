cmake_minimum_required(VERSION 2.8)

project(dataclassify)
aux_source_directory(. DataClassifySRC_LIST)
find_package(CURL)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
if(NOT SERVICE_FIND)
#定义COMM_DIR位置
if(NOT COMM_DIR)
    set( COMM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../)
endif()
if( IS_DIRECTORY ${COMM_DIR} )
    add_subdirectory( ${COMM_DIR}/comm comm)
else()
    message(FATAL_ERROR   "INVALID FOLDER 'CASSDK_DIR'=${CASSDK_DIR}" )
endif()
endif()
add_subdirectory(abs)
add_subdirectory(app)
INCLUDE_DIRECTORIES(
    ${COMM_INCLUDE_DIRS}
    ${APP_INCLUDE_DIR}
    ${ABSCLASS_INCLUDE}
    )
add_executable(${PROJECT_NAME} ${DataClassifySRC_LIST})
target_link_libraries(${PROJECT_NAME} comm abs glog app jsoncpp)

