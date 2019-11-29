cmake_minimum_required(VERSION 3.15)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/arm-toolchain.cmake)

project(pod-server)

include_directories(lib/include lib)

add_library(canbus STATIC lib/canbus/canbus.cpp)
add_library(task_manager STATIC lib/task_manager/task_manager.cpp lib/task_manager/task.cpp)

file(GLOB_RECURSE files "src/*.cpp")

message((${files}))

add_executable(main ${files})
target_link_libraries(main canbus task_manager pthread)
