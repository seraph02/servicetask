project(ServiceCore)
cmake_minimum_required(VERSION 3.0)
add_compile_options(-std=c++11 -g -fPIC)
set(BUILD_ELASTICLIENT_EXAMPLE NO)
set(BUILD_ELASTICLIENT_TESTS NO)
set(USE_SYSTEM_JSONCPP YES)
set(SERVICE_FIND YES)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
#定义COMM_DIR位置
if(NOT COMM_DIR)
    set( COMM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/)
endif()
if( IS_DIRECTORY ${COMM_DIR} )
    add_subdirectory( ${COMM_DIR}/comm comm)
else()
    message(FATAL_ERROR   "INVALID FOLDER 'CASSDK_DIR'=${CASSDK_DIR}" )
endif()
add_subdirectory(DataClassify)
add_subdirectory(elasticlient)
add_subdirectory(Base)
add_subdirectory(Manager)

INCLUDE_DIRECTORIES(
    ${COMM_INCLUDE_DIRS}
    ${BASE_INCLUDE_DIRS}
    ${CTIME_INCLUDE_DIRS}
    ${PROTOFILE_DIRS}
    ${MANAGER_INCLUDE_DIRS}
)
aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME} manager glog)
