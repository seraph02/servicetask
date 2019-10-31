project(base)
cmake_minimum_required(VERSION 3.0)
add_compile_options(-std=c++11 -fPIC)
aux_source_directory(. DIR_BASELIB_SRCS)
set(BASE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")
LINK_LIBRARIES(
    protobuf
)
if(NOT ROOT_DIR)
    set( ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../)
endif()
find_package(Protobuf REQUIRED)
FILE(GLOB ProtoFiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ../protofile/*.proto)
#set(ProtoFiles
#    ${CMAKE_CURRENT_SOURCE_DIR} ../protofile/taskinfo.proto
#    ${CMAKE_CURRENT_SOURCE_DIR} ../protofile/esinfo.proto
#)
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${ProtoFiles} )

set(PROTOFILE_DIRS ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")

INCLUDE_DIRECTORIES(
    ${PROTOFILE_DIRS}
    ${COMM_INCLUDE_DIRS}
    ${JSON2PB_INCLUDE_DIRS}
    ${PROTOBUF_INCLUDE_DIRS}
    )

add_library (${PROJECT_NAME} ${DIR_BASELIB_SRCS} ${PROTO_SRCS} ${PROTO_HDRS} )
TARGET_LINK_LIBRARIES(${PROJECT_NAME} comm pthread glog)

