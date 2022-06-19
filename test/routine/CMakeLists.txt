set(NAME test)
project(${NAME})

include_directories(${CMAKE_SOURCE_DIR})

aux_source_directory(. TEST_SOURCE_FILES)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(${NAME}
        ${CMAKE_SOURCE_DIR}/src/context.h
        ${CMAKE_SOURCE_DIR}/src/context.cc
        ${CMAKE_SOURCE_DIR}/src/routine.h
        ${CMAKE_SOURCE_DIR}/src/routine.cc
        ${CMAKE_SOURCE_DIR}/src/task_meta.h
        ${CMAKE_SOURCE_DIR}/src/task_meta.cc
        ${CMAKE_SOURCE_DIR}/src/task_control.h
        ${CMAKE_SOURCE_DIR}/src/task_control.cc
        ${CMAKE_SOURCE_DIR}/src/task_group.h
        ${CMAKE_SOURCE_DIR}/src/task_group.cc
        ${CMAKE_SOURCE_DIR}/src/block_queue.hpp
        ${TEST_SOURCE_FILES}
        )