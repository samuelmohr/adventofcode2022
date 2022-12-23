cmake_minimum_required(VERSION 3.5)

# library name
project(AdventofCodeDay16 VERSION 1.0 DESCRIPTION "Solution for AdventofCode Day 16")

# set the C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# compiles the files
add_executable(AdventofCodeDay16 main.cpp)

# target all libraries
add_subdirectory(lib)
target_link_libraries(AdventofCodeDay16 PUBLIC ProblemInstance)
target_include_directories(AdventofCodeDay16 PUBLIC
                          "./lib"
                          )

