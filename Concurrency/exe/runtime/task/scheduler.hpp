#pragma once

#include "runtime/task/task.hpp"

namespace exe::runtime::task
{

struct IScheduler
{
    virtual void submit(Task) = 0;

protected:
    virtual ~IScheduler() = default;
};

} // namespace exe::runtime::task

