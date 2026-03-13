cmake_minimum_required(VERSION 3.5.0)
project(fiber VERSION 1.0.0 LANGUAGES ASM C CXX)
enable_language(C ASM) # TODO: make correctly

set(CMAKE_CXX_STANDARD 20)

#set(GCC_compile_flags "-Wall -Wextra -Wpedantic # -Werror TODO: turn on
#    -Wno-maybe-uninitialized -g")
set(GCC_compile_flags "-g")
add_definitions(${GCC_compile_flags})
#add_compile_options(-fsanitize=undefined,address,leak)
#add_link_options(-fsanitize=undefined,address,leak)

file (GLOB SOURCES
    functions_asm.S
    scheduler.hpp
    scheduler.cpp
    execution_context.hpp
    execution_context.cpp
    fiber.hpp
    fiber.cpp
    main.cpp
)
add_executable(fiber ${SOURCES})
