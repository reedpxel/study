cmake_minimum_required(VERSION 3.5.0)
project(function VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

set(GCC_compile_flags "-Wall -Wextra -Wpedantic -Werror 
    -Wno-maybe-uninitialized")
add_definitions(${GCC_compile_flags})
add_compile_options(-fsanitize=undefined,address,leak)
add_link_options(-fsanitize=undefined,address,leak)

add_executable(variant
    get_index_by_type.hpp
    get_type_by_index.hpp
    max_alignof.hpp
    max_sizeof.hpp
    variant.hpp
    main.cpp)

