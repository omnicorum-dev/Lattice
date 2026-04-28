cmake_minimum_required(VERSION 4.2)
project(Lattice)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(vendor/Medium)

add_executable(Lattice main.cpp)
