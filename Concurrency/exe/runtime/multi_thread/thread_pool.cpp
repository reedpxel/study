#include "runtime/multi_thread/thread_pool.hpp"

namespace exe::runtime::multi_thread
{

ThreadPool::ThreadPool(size_t workerCount 
    /*= std::thread::hardware_concurrency()*/)
{
    assert(workerCount != 0);
    workers_.reserve(workerCount);
}

ThreadPool::~ThreadPool()
{
    assert(!launched_);
}

void ThreadPool::start() noexcept
{
    launched_ = true;
    size_t workerCount = workers_.capacity();
    for (size_t i = 0; i < workerCount; ++i)
    {
        workers_.emplace_back([this] 
            {
                this->workerRoutine(); 
            });
    }
}

void ThreadPool::submit(task::Task task) noexcept
{
    queue_.put(task);
}

void ThreadPool::stop()
{
    queue_.close();
    launched_ = false;
    for (auto& th : workers_)
    {
        th.join();
    }
}

ThreadPool* ThreadPool::current() noexcept
{
    return current_;
}

bool ThreadPool::launched() noexcept
{
    return launched_;
}

void ThreadPool::workerRoutine()
{
    current_ = this;
    while (true)
    {
        std::optional<task::Task> task = queue_.take();
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

} // namespace exe::runtime::multi_thread

