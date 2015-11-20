cmake_minimum_required(VERSION 3.3)
project(src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp open_list_data.cpp open_list_data.h)
add_executable(src ${SOURCE_FILES})
add_executable(mytest test/test_main.cpp)