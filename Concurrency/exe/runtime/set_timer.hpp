#pragma once

#include "runtime/view/timers.hpp"

namespace exe::runtime
{

inline void setTimer(View runtime_, timer::Duration duration, 
    timer::Handler handler)
{
    timers(runtime_).set(duration, std::move(handler));
}

} // namespace exe::runtime

