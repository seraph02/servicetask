# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
PROJECT(json2pb)
cmake_minimum_required(VERSION 3.0)
add_compile_options(-std=c++11)
#set(CMAKE_BUILD_TYPE "Debug")
aux_source_directory(. DIR_JSON2PB_SRCS)

LINK_LIBRARIES(
    protobuf
)

find_package(Protobuf REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
link_libraries(${JSONCPP_LIBRARIES})
message("FOUND CONFIG: ${JSONCPP_CONFIG}")
message("INCLUDE DIRS: ${JSONCPP_INCLUDE_DIRS}")
message("LIBRARIES: ${JSONCPP_LIBRARIES}")


include_directories(
    ${PROTOBUF_INCLUDE_DIRS}
    ${JSONCPP_INCLUDE_DIRS}
    ${COMM_INCLUDE_DIRS})
set(JSON2PB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")

add_library(${PROJECT_NAME} ${DIR_JSON2PB_SRCS} comm)


TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROTOBUF_LIBRARIES} ${JSONCPP_LIBRARIES} pthread)

#add_executable(testjson test_json.cpp ${PROTO_SRCS} ${PROTO_HDRS})
#TARGET_LINK_LIBRARIES(testjson ${PROTOBUF_LIBRARIES} json2pb)
install(TARGETS ${PROJECT_NAME} LIBRARY
        DESTINATION lib)
