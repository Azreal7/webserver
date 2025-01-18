cmake_minimum_required(VERSION 2.16)

project(webserver)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/obj)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/obj)

# 如果目录不存在则创建
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/obj)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

file(GLOB SERVER_SRC
    "${SRC_DIR}/*.cpp"
    "${INCLUDE_DIR}/*.h"
    "${CMAKE_SOURCE_DIR}/server.cpp"
)

set(CLIENT_SRC
    "${SRC_DIR}/util.cpp"
    "${INCLUDE_DIR}/util.h"
    "${CMAKE_SOURCE_DIR}/client.cpp"
)

add_executable(server ${SERVER_SRC})

add_executable(client ${CLIENT_SRC})