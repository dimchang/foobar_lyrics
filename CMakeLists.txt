cmake_minimum_required(VERSION 3.15)
project(foo_lyric_source)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release)

set(SOURCES src/main.cpp)

add_library(foo_lyric_source SHARED ${SOURCES})

target_link_libraries(foo_lyric_source winhttp gdiplus)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

set_target_properties(foo_lyric_source PROPERTIES OUTPUT_NAME "foo_lyric_source" PREFIX "")