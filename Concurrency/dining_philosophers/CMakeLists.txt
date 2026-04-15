cmake_minimum_required(VERSION 3.5.0)
project(dining_philosophers VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

set(GCC_compile_flags "-Wall -Wextra -Wpedantic -Werror \
    -Wno-maybe-uninitialized -Wno-unused-result -g")
#add_compile_options(-fsanitize=thread)
#add_link_options(-fsanitize=thread)
add_definitions(${GCC_compile_flags})

file (GLOB SOURCES
    main.cpp
    fork.hpp
    fork.cpp
    philosopher.hpp
    philosopher.cpp
    table.hpp
    table.cpp
)
add_executable(dining_philosophers ${SOURCES})
