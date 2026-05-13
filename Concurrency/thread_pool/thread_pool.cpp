#include "thread_pool.hpp"

ThreadPool::ThreadPool(size_t workerCount 
    /*= std::thread::hardware_concurrency()*/)
{
    assert(workerCount != 0);
    workers_.reserve(workerCount);
}

ThreadPool::~ThreadPool()
{
    assert(!launched);
}

void ThreadPool::start() noexcept
{
    launched = true;
    size_t workerCount = workers_.capacity();
    for (size_t i = 0; i < workerCount; ++i)
    {
        workers_.emplace_back([this] 
            {
                this->workerRoutine(); 
            });
    }
}

void ThreadPool::submit(const Task& task) noexcept
{
    queue_.put(task);
}

void ThreadPool::stop()
{
    queue_.close();
    launched = false;
    for (auto& th : workers_)
    {
        th.join();
    }
}

ThreadPool* ThreadPool::current() noexcept
{
    return current_;
}

void ThreadPool::workerRoutine()
{
    current_ = this;
    while (true)
    {
        std::optional<Task> task = queue_.take();
        if (!task.has_value())
        {
            return;
        }
        try
        {
            task.value()();
        }
        catch (...)
        {
            std::terminate();
        }
    }
}

thread_local ThreadPool* ThreadPool::current_ = nullptr;

