cmake_minimum_required(VERSION 2.8)

project(put2db)
aux_source_directory(. EXPClassifySRC_LIST)
find_package(CURL)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
file(GLOB EXPCLASSIFY_INCLUDE_DIRS ./*.h)
set(EXPCLASSIFY_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
if(NOT SERVICE_FIND)
#定义COMM_DIR位置
if(NOT ROOT_DIR)
    set( ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ )
endif()
if(NOT COMM_DIR)
    set( COMM_DIR ${ROOT_DIR}/comm )
endif()
if( IS_DIRECTORY ${COMM_DIR} )
    add_subdirectory( ${ROOT_DIR}/comm comm)
endif()

if(NOT ES_DIR)
    set( ES_DIR ${ROOT_DIR}/elasticlient )
endif()
if( IS_DIRECTORY ${ES_DIR} )
    add_subdirectory( ${ROOT_DIR}/elasticlient elasticlient)
else()
    message(FATAL_ERROR   "INVALID FOLDER 'CASSDK_DIR'=${CASSDK_DIR}" )
endif()

endif()

INCLUDE_DIRECTORIES(
    ${COMM_INCLUDE_DIRS}
    ${JSONCPP_INCLUDE_DIRS}
    ${JSON2PB_INCLUDE_DIRS}
    ${CPR_INCLUDE_DIRS}
    ${ELASTICLIENT_INCLUDE_DIRS}
    ${COMM_INCLUDE_DIRS}
    ${PROTOBUF_INCLUDE_DIRS}
    ${BASE_INCLUDE_DIRS}
    ${PROTOFILE_DIRS}

)

#add_executable(${PROJECT_NAME} ${EXPClassifySRC_LIST} )
#target_link_libraries(${PROJECT_NAME} comm glog jsoncpp ${CPR_LIBRARIES} ${ELASTICLIENT_LIBRARIES} base)
add_library (lib${PROJECT_NAME} "dbput.cpp" "expclassify.cpp")
add_executable(${PROJECT_NAME} "main.cpp")
target_link_libraries(lib${PROJECT_NAME} comm glog jsoncpp ${CPR_LIBRARIES} ${ELASTICLIENT_LIBRARIES} base)
target_link_libraries(${PROJECT_NAME} lib${PROJECT_NAME})
