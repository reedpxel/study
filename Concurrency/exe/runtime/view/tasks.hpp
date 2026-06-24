#pragma once

#include "runtime/view.hpp"
#include "runtime/task/scheduler.hpp"

namespace exe::runtime
{

task::IScheduler& tasks(View view);

} // namespace exe::runtime

