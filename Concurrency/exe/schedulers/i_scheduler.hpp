#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <functional>

using Task = std::function<void()>;

class IScheduler
{
public:
    virtual void submit(const Task& task) = 0;
    virtual ~IScheduler() {};
};

#endif // SCHEDULER_H
