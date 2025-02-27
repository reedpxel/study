cmake_minimum_required(VERSION 3.5.0)
project(shared_ptr VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

set(GCC_compile_flags "-Wall -Wextra -Wpedantic -Werror")
#set(GCC_compile_flags "-Wall -Wextra -Wpedantic -Werror -fsanitize=address,leak")
add_definitions(${GCC_compile_flags})
add_compile_options(-fsanitize=address,leak)
add_link_options(-fsanitize=address,leak)

add_executable(shared_ptr
    base_control_block.hpp
    control_block.hpp
    control_block_with_deleter_and_allocator.hpp
    control_block_make_shared.hpp
    shared_ptr.hpp
    weak_ptr.hpp
    enable_shared_from_this.hpp
    main.cpp) 

