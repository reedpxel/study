cmake_minimum_required(VERSION 3.10.0)
project(exe VERSION 1.0.0 LANGUAGES ASM C CXX)
set(CMAKE_CXX_STANDARD 23)

add_compile_options(-Wall -Wextra -Wpedantic -Werror -g)
add_compile_options(-fsanitize=address,undefined,leak)
add_link_options(-fsanitize=address,undefined,leak)
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

    runtime/sandbox/clock.hpp
    runtime/sandbox/manual_loop.cpp
    runtime/sandbox/manual_loop.hpp
    runtime/sandbox/runtime.cpp
    runtime/sandbox/runtime.hpp
    runtime/sandbox/timer_queue.hpp
    runtime/sandbox.hpp

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

    fiber/core/body.hpp
    fiber/core/fiber.cpp
    fiber/core/fiber.hpp
    fiber/sched/sched.cpp
    fiber/sched/sched.hpp

    thread/wait_group.hpp
    thread/wait_group.cpp
  
    exe.hpp
    main.cpp
)
add_executable(exe ${SOURCES})
