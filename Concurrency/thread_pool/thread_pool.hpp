#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <cassert>
#include <functional>

#include "blocking_queue.hpp"

class ThreadPool
{
    using Task = std::function<void()>;

public:
    explicit ThreadPool(size_t workerCount = 
        std::thread::hardware_concurrency());
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ~ThreadPool();

    /*
     * Starts executing tasks, submitted in thread pool. Must be called 0 or 1
     * times. If start() called, stop() must be called exactly once. ThreadPool
     * ctor, start() and stop() must be called in the same thread.
     */
    void start() noexcept;

    /*
     * Submits a task to be eventually executed. Tasks can be submitted before
     * or after start() call, but not after stop() call (in this case tasks 
     * will not be put in the task queue). Tasks do not return a value and do
     * not receive arguments, promise-future pairs should be used to return
     * values from tasks.
     */
    void submit(const Task& task) noexcept;

    /*
     * Stops receiving tasks. A precondition for stop() call is the absence of 
     * tasks in a thread pool, calling stop() is UB if the condition is not met.
     * Syncronization primitives such as wait groups should be used to block the
     * thread pool thread until all tasks are executed.
     */
    void stop();

    /*
     * Returns a pointer to the current thread pool if the method is called in 
     * thread pool task or nullptr otherwise.
     */
    static ThreadPool* current() noexcept;

private:
    void workerRoutine();
    
    static thread_local ThreadPool* current_;

private:
    std::vector<std::thread> workers_;
    UnboundedBlockingMPMCQueue<Task> queue_;
    bool launched = false;
};

#endif // THREAD_POOL_H

