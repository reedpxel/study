cmake_minimum_required(VERSION 3.5.0)
project(Complex VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

add_executable(Complex complex.h complex.cpp main.cpp)

