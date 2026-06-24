#pragma once

#include "runtime/timer/duration.hpp"
#include "runtime/timer/handler.hpp"

namespace exe::runtime::timer
{

struct IScheduler
{
    virtual void set(Duration, Handler) = 0;

protected:
    virtual ~IScheduler() = default;
};

} // namespace exe::runtime::timer

