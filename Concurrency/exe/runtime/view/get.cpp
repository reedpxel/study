#include "runtime/view.hpp"

namespace exe::runtime
{

template <typename Service>
Service& get(View view)
{
    return *std::get<Service*>(view);
}

task::IScheduler& tasks(View view)
{
    return get<task::IScheduler>(view);
}

timer::IScheduler& timers(View view)
{
    return get<timer::IScheduler>(view);
}

} // namespace exe::runtime

