cmake_minimum_required(VERSION 3.5.0)
project(allocators_tests VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

set(GCC_compile_flags "-Wall -Wextra -Wpedantic -Werror")
add_definitions(${GCC_compile_flags})

add_executable(allocators_tests
               stack_allocator.hpp
               main.cpp) 

