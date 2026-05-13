cmake_minimum_required(VERSION 3.10.0)
project(thread_pool VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

set(GCC_compile_flags "-Wall -Wextra -Wpedantic -Werror -g")
add_compile_options(-fsanitize=address,undefined,leak)
add_link_options(-fsanitize=address,undefined,leak)
add_definitions(${GCC_compile_flags})

file (GLOB SOURCES
    main.cpp
    thread_pool.hpp
    thread_pool.cpp
    blocking_queue.hpp
    wait_group.hpp
    wait_group.cpp
)
add_executable(thread_pool ${SOURCES})
