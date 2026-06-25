#include "runtime/sandbox/runtime.hpp"

namespace exe::runtime::sandbox
{

Runtime::Runtime()
        : manualLoop_{}
        , timerQueue_{&manualLoop_}
        , clock_{}
{}

size_t Runtime::runAtMostTasks(size_t limit)
{
    return manualLoop_.runAtMostTasks(limit);
}

size_t Runtime::runTasks()
{
    return manualLoop_.runTasks();
}

size_t Runtime::advanceClockBy(timer::Duration delta)
{
    clock_.advanceBy(delta);
    return timerQueue_.submitReadyTasks(clock_.now());
}

size_t Runtime::advanceClockToNextDeadline()
{
    std::optional<Instant> nextDeadline = timerQueue_.nextDeadline();
    if (!nextDeadline.has_value())
    {
        return 0;
    }
    clock_.advanceTo(nextDeadline.value());
    return timerQueue_.submitReadyTasks(clock_.now());
}

bool Runtime::empty()
{
    return manualLoop_.empty() && timerQueue_.empty();
}

} // namespace exe::runtime::sandbox

