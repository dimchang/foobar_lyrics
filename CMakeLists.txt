cmake_minimum_required(VERSION 3.15)
project(foo_lyric_source)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT WIN32)
    message(FATAL_ERROR "This plugin only supports Windows")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/foobar2000/SDK
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/foobar2000/shared
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/pfc
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/ATLHelpers
)

# Source files - MATCHING ACTUAL FILES
set(SOURCES
    src/main.cpp
    src/lyrics_panel.cpp
    src/lyric_search_task.cpp
    src/sources.cpp
)

set(HEADERS
    src/lyric_source.h
    src/lyrics_panel.h
    src/lyric_search_task.h
    src/lyric_manager.h
    src/lrc_parser.h
    src/sources.h
)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    winhttp
    gdiplus
    comctl32
)

# MinGW-compatible flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall")

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "foo_lyric_source"
    PREFIX ""
)