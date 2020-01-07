cmake_minimum_required(VERSION 3.15)
project(Fat)

set(CMAKE_CXX_STANDARD 14)

include_directories(includes)
add_executable(FATParser main.cpp includes/HexReader.h includes/utils.h)