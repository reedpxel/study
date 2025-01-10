#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include <exception>

struct invalidLine : std::exception {};
struct noIntersection : std::exception {};
struct linesAreParallel : noIntersection {};
struct linesAreSame : noIntersection {};

#endif