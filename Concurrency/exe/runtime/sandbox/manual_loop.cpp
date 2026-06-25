#include "manual_loop.hpp"

namespace exe::runtime::sandbox
{

ManualLoop::ManualLoop() {}

ManualLoop::~ManualLoop() {}

void ManualLoop::submit(task::Task task)
{
    taskQueue.push_back(task);
}

size_t ManualLoop::runAtMostTasks(size_t tasks)
{
    size_t tasksCompleted = 0;
    for (; tasksCompleted < tasks && !taskQueue.empty(); ++tasksCompleted)
    {
        try
        {
            taskQueue.front()();
            taskQueue.pop_front();
        }
        catch (...)
        {
            std::terminate();
        }
    }
    return tasksCompleted;
}

size_t ManualLoop::runTasks()
{
    size_t tasksCompleted = 0;
    for (; !taskQueue.empty(); ++tasksCompleted)
    {
        try
        {
            taskQueue.front()();
            taskQueue.pop_front();
        }
        catch (...)
        {
            std::terminate();
        }
    }
    return tasksCompleted;
}

bool ManualLoop::runNextTask()
{
    return runAtMostTasks(1) == 1;
}

bool ManualLoop::empty()
{
    return taskQueue.empty();
}

} // namespace exe::runtime::sandbox

