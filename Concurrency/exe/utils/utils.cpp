#include "utils.hpp"

namespace utils
{
    void submitTask(IScheduler& scheduler, const Task& task)
    {
        Fiber* fiber = new Fiber{scheduler, task};
        scheduler.submit([fiber] { fiber->resume(); });
    }
}

