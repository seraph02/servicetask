cmake_minimum_required(VERSION 3.0)
add_compile_options(-std=c++11)
project(esput)
aux_source_directory(. ESPUT_SRC_LIST)
find_package(CURL)
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
INCLUDE_DIRECTORIES(
    ${COMM_INCLUDE_DIRS}
    ${MANAGER_INCLUDE_DIRS}
    ${JSONCPP_INCLUDE_DIRS}
    ${CPR_INCLUDE_DIRS}
    ${ELASTICLIENT_INCLUDE_DIRS}
    )
add_executable(${PROJECT_NAME} ${ESPUT_SRC_LIST})
target_link_libraries(${PROJECT_NAME} comm glog jsoncpp ${ELASTICLIENT_LIBRARIES} manager)

