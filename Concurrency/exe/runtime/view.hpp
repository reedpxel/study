#pragma once

#include <tuple>

#include "runtime/task/scheduler.hpp"
#include "runtime/timer/scheduler.hpp"

namespace exe::runtime
{

using View = std::tuple<task::IScheduler*, timer::IScheduler*>;

} // namespace exe::runtime

