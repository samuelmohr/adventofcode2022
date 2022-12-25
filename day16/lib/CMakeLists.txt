cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 20)

add_library(ProblemInstance STATIC
  implementations.cpp
  valve.h twosymbol.h tunnelmap.h instance.h instancewithelephant.h
)

