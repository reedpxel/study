cmake_minimum_required(VERSION 3.5.0)
project(unordered_map VERSION 0.1.0 LANGUAGES C CXX)
#set(CMAKE_C_COMPILER "/usr/bin/clang")
#set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_STANDARD 20)

set(GCC_compile_flags "-Wall -Wextra -Wpedantic -Werror")
#set(GCC_compile_flags "-Wall -Wextra -Wpedantic -Werror -fsanitize=address")
add_definitions(${GCC_compile_flags})
#add_compile_options(-fsanitize=address)
#add_link_options(-fsanitize=address)

add_executable(unordered_map
               unordered_map.hpp
               hash_functor.hpp
               equal_functor.hpp
               main.cpp) 

