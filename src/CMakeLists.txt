cmake_minimum_required(VERSION 3.3)
project(src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/home/cheyulin/gitrepos/Open-List-Proportional-Representation/release")
#add_executable(mytest test/deprecated/test_compute_payoff.cpp)
#add_executable(mytest2 test/deprecated/test_init_alternatives.cpp)

set(SOURCE_FILES open_list_party_basic_info.h open_list_party_basic_info.cpp open_list_party.cpp open_list_party.h open_list_solver.cpp open_list_solver.h open_list_includes.h open_list_input_output.h open_list_input_output.cpp)
#set(TEST_FILES test/test_unit.cpp test/test_unit.h)
add_executable(hklc ${SOURCE_FILES} main.cpp)
#add_executable(hklctest test/test_main.cpp ${SOURCE_FILES} ${TEST_FILES})