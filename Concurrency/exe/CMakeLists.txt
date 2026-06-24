cmake_minimum_required(VERSION 3.10.0)
project(exe VERSION 1.0.0 LANGUAGES ASM C CXX)
set(CMAKE_CXX_STANDARD 23)

add_compile_options(-Wall -Wextra -Wpedantic -Werror -g)
add_definitions(${GCC_compile_flags})

include_directories(${PROJECT_SOURCE_DIR})
file (GLOB SOURCES
    runtime/multi_thread/blocking_queue.hpp
    runtime/multi_thread/runtime.cpp
    runtime/multi_thread/runtime.hpp
    runtime/multi_thread/thread_pool.cpp
    runtime/multi_thread/thread_pool.hpp
    runtime/multi_thread/timer_thread.cpp
    runtime/multi_thread/timer_thread.hpp

    runtime/multi_thread.hpp
    runtime/sandbox/*
    runtime/set_timer.hpp
    runtime/submit_task.hpp
    
    runtime/task/scheduler.hpp
    runtime/task/task.hpp

    runtime/timer/duration.hpp
    runtime/timer/handler.hpp
    runtime/timer/scheduler.hpp

    runtime/view/get.cpp
    runtime/view/tasks.hpp
    runtime/view/timers.hpp
    
    runtime/view.hpp

    ../coroutine/execution_context.hpp
    ../coroutine/execution_context.cpp
    ../coroutine/execution_context.S
    ../coroutine/coroutine.hpp
    ../coroutine/coroutine.cpp
    ../coroutine/this_coro.hpp
    ../coroutine/this_coro.cpp
 
    multi_thread.hpp
    set_timer.hpp
    submit_task.hpp
    view.hpp

    thread/wait_group.hpp
    thread/wait_group.cpp
  
    fiber.hpp
    fiber.cpp
    exe.hpp
    main.cpp
)
add_executable(exe ${SOURCES})
