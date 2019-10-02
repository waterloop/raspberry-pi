cmake_minimum_required(VERSION 3.15)

project(pod-server)

include_directories(lib/include)

add_library(canbus STATIC lib/canbus/canbus.cpp)

add_executable(main main.cpp)
target_link_libraries(main canbus)
