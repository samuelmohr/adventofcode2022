# only for cmake --version >= 3.15
cmake_minimum_required(VERSION 3.15)

# library name
project(AdventofCodeDay05 VERSION 1.0 DESCRIPTION "Solution for AdventofCode Day 05")

# set the C++20 standard
set(CMAKE_CXX_STANDARD 20)

# compiles the files 
add_executable(AdventofCodeDay05 main.cpp)

# target all libraries
add_subdirectory(lib)
target_link_libraries(AdventofCodeDay05 PUBLIC SupplyStack)


