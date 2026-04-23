cmake_minimum_required(VERSION 3.10)
project(dynamic_bitset)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall")

# Create the main executable 'code'
# The OJ will provide a main.cpp file that includes dynamic_bitset.h
add_executable(code main.cpp)
