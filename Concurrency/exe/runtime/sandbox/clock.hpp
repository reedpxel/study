#pragma once

#include <cassert>
#include <chrono>

#include "runtime/timer/duration.hpp"

namespace exe::runtime::sandbox
{

using Instant = std::chrono::microseconds;

class Clock
{
public:
    void advanceBy(timer::Duration delta)
    {
        now_ += delta;
    }

    void advanceTo(Instant future)
    {
        assert(future >= now_);
        now_ = future;
    }

    Instant now()
    {
        return now_;
    }
private:
    Instant now_;
};

} // namespace exe::runtime::sandbox

