cmake_minimum_required(VERSION 3.10.0)
project(coroutine VERSION 1.0.0 LANGUAGES ASM C CXX)
set(CMAKE_CXX_STANDARD 23)

add_compile_options(-Wall -Wextra -Wpedantic -Werror -g)
add_definitions(${GCC_compile_flags})

file (GLOB SOURCES
    main.cpp
    execution_context.hpp
    execution_context.cpp
    execution_context.S
    coroutine.hpp
    coroutine.cpp
    this_coro.hpp
    this_coro.cpp
)
add_executable(coroutine ${SOURCES})
