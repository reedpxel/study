# TODO: test with clang
#set(CMAKE_C_COMPILER "/usr/bin/clang")
#set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

cmake_minimum_required(VERSION 3.10.0)
project(stdlike_future VERSION 1.0.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)

add_compile_options(-Wall -Wextra -Wpedantic -Werror -g)
#add_compile_options(-fsanitize=address,undefined,leak)
#add_link_options(-fsanitize=address,undefined,leak)
add_compile_options(-fsanitize=thread)
add_link_options(-fsanitize=thread)
add_definitions(${GCC_compile_flags})

file (GLOB SOURCES
    main.cpp
    promise.hpp
    future.hpp
    shared_state.hpp
)
add_executable(stdlike_future ${SOURCES})
