#pragma once

#include <deque>
#include <exception>

#include "runtime/task/scheduler.hpp"

namespace exe::runtime::sandbox
{

/*
 * Executes received tasks in one thread. Since all tasks are executed in one
 * thread, no synchronization primitives required
 */
class ManualLoop : public task::IScheduler
{
public:
    ManualLoop();
    ~ManualLoop() override;

    ManualLoop(const ManualLoop&) = delete;
    ManualLoop& operator=(const ManualLoop&) = delete;

    void submit(task::Task task) override;

    size_t runAtMostTasks(size_t tasks);
    size_t runTasks();
    bool runNextTask();
    bool empty();
private:
    std::deque<task::Task> taskQueue;
};

} // namespace exe::runtime::sandbox

