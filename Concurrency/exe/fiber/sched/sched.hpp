#pragma once

#include <chrono>

#include "fiber/core/body.hpp"
#include "fiber/core/fiber.hpp"
#include "runtime/view.hpp"
#include "runtime/view/tasks.hpp"
#include "runtime/submit_task.hpp"

namespace exe::fiber
{

void go(runtime::View view, Body body);
void go(Body body);
void sleepFor(std::chrono::milliseconds delay);
void yield();

}

