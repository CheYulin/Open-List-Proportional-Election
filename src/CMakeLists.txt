cmake_minimum_required(VERSION 3.3)
project(src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp open_list_party.cpp open_list_party.h open_list_solver.cpp open_list_solver.h open_list_includes.h)
add_executable(src ${SOURCE_FILES})
add_executable(mytest test/test_compute_payoff.cpp)
add_executable(mytest2 test/test_init_alternatives.cpp)