cmake_minimum_required(VERSION 3.22)
project(hello_world)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(src)
include_directories(.)

add_executable(hello_world main.cpp)
target_link_libraries(hello_world demo_lib)
add_executable(thread_demo1 practice/thread/thread_demo1.cpp)
add_executable(thread_demo2 practice/thread/thread_demo2.cpp)