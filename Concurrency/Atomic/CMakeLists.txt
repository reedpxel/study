cmake_minimum_required(VERSION 3.5.0)
project(Atomic VERSION 0.1.0 LANGUAGES C CXX)
enable_language(C ASM)

file(GLOB SOURCES
        functions.S
        c_functions.h
        atomic.h
        atomic.cpp
        spinlock.h
        spinlock.cpp
        main.cpp)

add_executable(Atomic ${SOURCES})

