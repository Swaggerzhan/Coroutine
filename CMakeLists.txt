cmake_minimum_required(VERSION 3.16)
set(NAME routine)
project(${NAME})

set(CMAKE_CXX_STANDARD 14)

aux_source_directory(src SRC_SOURCE_FILES)

message( "*** USAGE: ${SRC_SOURCE_FILES}" )

add_subdirectory(test/main)
add_subdirectory(test/routine)

add_library(${NAME}
        SHARED
        ${SRC_SOURCE_FILES}
        src/block_queue.hpp
        src/work_stealing_queue.hpp
        )



