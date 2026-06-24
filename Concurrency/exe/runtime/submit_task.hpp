#pragma once

#include "runtime/task/task.hpp"
#include "runtime/view/tasks.hpp"

namespace exe::runtime
{

inline void submitTask(View runtime_, task::Task task_)
{
    tasks(runtime_).submit(std::move(task_));
}

} // namespace exe::runtime

