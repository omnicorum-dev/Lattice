cmake_minimum_required(VERSION 3.31)
project(Base2026)

set(CMAKE_CXX_STANDARD 17)

#add_executable(Base2026 main.cpp)

add_library(BaseLayer INTERFACE)
target_include_directories(BaseLayer INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)