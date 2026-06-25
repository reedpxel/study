#pragma once

#include <optional>
#include <map>

#include "runtime/sandbox/clock.hpp"
#include "runtime/timer/scheduler.hpp"
#include "runtime/task/scheduler.hpp"

namespace exe::runtime::sandbox
{

class TimerQueue : public timer::IScheduler
{
public:
    TimerQueue(task::IScheduler* taskScheduler)
            : taskScheduler_(taskScheduler)
            , map_{}
    {}

    void set(timer::Duration duration, timer::Handler handler) override
    {
        map_.insert(std::make_pair(duration, std::move(handler)));
    }

    bool empty()
    {
        return map_.empty();
    }

    std::optional<Instant> nextDeadline()
    {
        if (map_.empty())
        {
            return std::nullopt;
        }
        return map_.begin()->first;
    }

    size_t submitReadyTasks(Instant now)
    {
        auto firstInFuture = map_.upper_bound(now);
        size_t res = std::distance(map_.begin(), firstInFuture);
        for (auto it = map_.begin(); it != firstInFuture; ++it)
        {
            taskScheduler_->submit(it->second);
        }
        map_.erase(map_.begin(), firstInFuture);
        return res;
    }

private:
    task::IScheduler* taskScheduler_;
    std::multimap<std::chrono::microseconds, timer::Handler> map_;
};

} // namespace exe::runtime::sandbox

