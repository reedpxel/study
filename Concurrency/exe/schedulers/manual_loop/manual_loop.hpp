#ifndef MANUAL_LOOP_H
#define MANUAL_LOOP_H

#include <deque>
#include <exception>

#include "../i_scheduler.hpp"

/*
 * Executes received tasks in one thread. Since all tasks are executed in one
 * thread, no synchronization primitives required
 */
class ManualLoop : public IScheduler
{
public:
    ManualLoop();
    ~ManualLoop() override;

    ManualLoop(const ManualLoop&) = delete;
    ManualLoop& operator=(const ManualLoop&) = delete;

    void submit(const Task& task) override;

    size_t runAtMostTasks(size_t tasks);
    size_t runTasks();
    bool runNextTask();
    bool empty();
private:
    std::deque<Task> taskQueue;
};

#endif // MANUAL_LOOP_H

