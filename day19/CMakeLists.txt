cmake_minimum_required(VERSION 3.5)

# library name
project(AdventofCodeDay19 VERSION 1.0 DESCRIPTION "Solution for AdventofCode Day 19")

# set the C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# compiles the files
add_executable(AdventofCodeDay19 main.cpp)

# target all libraries
add_subdirectory(lib)
target_link_libraries(AdventofCodeDay19 PUBLIC ProblemInstance)
target_include_directories(AdventofCodeDay19 PUBLIC
                          "./lib"
                          )

