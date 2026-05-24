cmake_minimum_required(VERSION 3.10.0)
project(exe VERSION 1.0.0 LANGUAGES ASM C CXX)
set(CMAKE_CXX_STANDARD 23)

add_compile_options(-Wall -Wextra -Wpedantic -Werror -g)
add_definitions(${GCC_compile_flags})

file (GLOB SOURCES
    ../thread_pool/thread_pool.hpp
    ../thread_pool/thread_pool.cpp
    ../thread_pool/blocking_queue.hpp
    ../thread_pool/wait_group.hpp
    ../thread_pool/wait_group.cpp

    ../coroutine/execution_context.hpp
    ../coroutine/execution_context.cpp
    ../coroutine/execution_context.S
    ../coroutine/coroutine.hpp
    ../coroutine/coroutine.cpp
    ../coroutine/this_coro.hpp
    ../coroutine/this_coro.cpp
   
    fiber.hpp
    fiber.cpp
    exe.hpp
    main.cpp
)
add_executable(exe ${SOURCES})
