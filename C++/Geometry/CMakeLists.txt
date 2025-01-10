cmake_minimum_required(VERSION 3.5.0)
project(Geometry VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

set(GCC_compile_flags "-Wall -Wextra -Wpedantic")
add_definitions(${GCC_compile_flags})

add_executable(Geometry 
    exceptions.h
    
    primitives/dot.h primitives/dot.cpp
    primitives/line.h primitives/line.cpp
    primitives/segment.h primitives/segment.cpp
    primitives/vector.h primitives/vector.cpp

    shapes/shape.h
    shapes/convex_polygon.h shapes/convex_polygon.cpp
    shapes/quadrilateral.h shapes/quadrilateral.cpp
    shapes/trapezium.h shapes/trapezium.cpp
    shapes/parallelogramm.h shapes/parallelogramm.cpp
    shapes/rectangle.h shapes/rectangle.cpp
    shapes/rhombus.h shapes/rhombus.cpp
    shapes/square.h shapes/square.cpp

    main.cpp)
