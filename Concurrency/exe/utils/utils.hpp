#ifndef UTILS_H
#define UTILS_H

#include "../schedulers/i_scheduler.hpp"
#include "../fiber.hpp"

namespace utils
{
/*
 * Submits a task in a scheduler. Any scheduler implementation guarantees that
 * the task will be eventually executed
 */
void submitTask(IScheduler& scheduler, const Task& task);
}

#endif // UTILS_H

