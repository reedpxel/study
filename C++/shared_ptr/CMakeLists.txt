cmake_minimum_required(VERSION 3.5.0)
project(shared_ptr VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

#set(GCC_compile_flags "-Wall -Wextra -Wpedantic -Werror")
set(GCC_compile_flags "-Wall -Wextra -Wpedantic -Werror -fsanitize=address")
add_definitions(${GCC_compile_flags})
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

add_executable(shared_ptr
    smart_pointer_base.hpp
    shared_ptr.hpp
    weak_ptr.hpp
    main.cpp) 

