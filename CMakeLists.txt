cmake_minimum_required(VERSION 3.15)
project(foo_lyric_source)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform check
if(NOT WIN32)
    message(FATAL_ERROR "This plugin only supports Windows")
endif()

# Set output directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/foobar2000/SDK
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/foobar2000/shared
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/pfc
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/ATLHelpers
)

# Source files
set(SOURCES
    src/main.cpp
    src/lyric_source.cpp
    src/netease_source.cpp
    src/qqmusic_source.cpp
    src/local_source.cpp
    src/lrc_parser.cpp
    src/lyric_manager.cpp
    src/lyrics_panel.cpp
    src/lyric_search_task.cpp
)

# Header files
set(HEADERS
    src/lyric_source.h
    src/netease_source.h
    src/qqmusic_source.h
    src/local_source.h
    src/lrc_parser.h
    src/lyric_manager.h
    src/lyrics_panel.h
    src/lyric_search_task.h
)

# Create DLL
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    winhttp
    gdiplus
    comctl32
)

# Compiler flags
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W3
        /O2
        /MT
        /DNDEBUG
        /DWIN32
        /D_WINDOWS
        /D_USRDLL
        /D_UNICODE
        /DUNICODE
        /GF
        /Gy
    )
    
    # Disable specific warnings
    target_compile_options(${PROJECT_NAME} PRIVATE
        /wd4996  # Deprecated functions
    )
endif()

# Set output name
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "foo_lyric_source"
    PREFIX ""
)
