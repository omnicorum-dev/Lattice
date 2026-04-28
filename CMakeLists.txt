cmake_minimum_required(VERSION 4.2)
project(Lattice)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG_MODE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DRELEASE_MODE)
endif()

add_subdirectory(vendor/Medium)

add_executable(LatticeTesting testing/main.cpp)
target_include_directories(LatticeTesting PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(LatticeTesting PRIVATE Medium)