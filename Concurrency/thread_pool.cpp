#include <iostream>
#include <thread>
#include <functional>
#include <vector>
#include <chrono>
#include <deque>
#include <mutex>
#include <cassert>
#include <condition_variable>
#include <atomic>

using Task = std::function<void()>;

template <typename T>
class UnboundedBlockingMPMCQueue // multiproducer, multiconsumer
{
    std::deque<T> buffer; // guarded by mutex
    std::mutex mutex;
    std::condition_variable notEmpty;
    bool stopFlag = false;

    T takeLocked();
public:
    std::atomic<size_t> taskCount = 0; // guarded by taskCountMutex in
// ThreadPool. Mutex solely is not enough, reading must be atomic, otherwise 
// wait() will not work

    void put(T task);
    T take();
    void stop();
};

template <typename T>
void UnboundedBlockingMPMCQueue<T>::put(T task)
{
    std::lock_guard guard(mutex);
    buffer.push_back(std::move(task));
    ++taskCount;
    notEmpty.notify_one();
}

template <typename T>
T UnboundedBlockingMPMCQueue<T>::take()
{
    std::unique_lock lock(mutex);
    if (stopFlag) return [](){}; // here the method is left by all workers that
// were executing a task or code in workerRoutine() after a task execution, when
// notify_all() in put() was called. Such workers were not executing take(). As
// they enter it, they must be stopped here or they would enter the cycle
    while (buffer.empty())
    {   // wait() does the following:
        // 1) releases mutex 2) waits 3) reacquires mutex 4) exits "wait"
        notEmpty.wait(lock);
        if (stopFlag) return [](){}; // here the method is left by all workers
// that have waited notify_one() from put()
    }
    return takeLocked();
}

template <typename T>
void UnboundedBlockingMPMCQueue<T>::stop()
{
    stopFlag = true;
    notEmpty.notify_all();
}

template <typename T>
T UnboundedBlockingMPMCQueue<T>::takeLocked()
{
    T front = std::move(buffer.front());
    buffer.pop_front();
    return front;
}

class ThreadPool
{
    std::vector<std::thread> workers;
    UnboundedBlockingMPMCQueue<Task> tasks;
    bool stopFlag = false;
    std::condition_variable cvWait;
    std::mutex cvMutex;
    std::mutex taskCountMutex;
    
    void workerRoutine(size_t threadNumber);
public:
    ThreadPool(size_t workers_);
    ~ThreadPool();
    ThreadPool(const ThreadPool&) = delete;
    void operator=(const ThreadPool&) = delete;
    void submit(Task task); // adds a task to be done
    void wait(); // locks the thread where thread pool was created, until all
// submitted tasks are executed, then continues to execute code in the thread
    void stop(); // interrupts the execution of tasks. Works only between tasks'
// executions
};

void ThreadPool::workerRoutine(size_t threadNumber)
{
    while (true)
    {
        auto task_ = tasks.take();
        try
        {
            task_();
        } catch (const std::exception& exc) {
            std::cout << exc.what();
        }
        std::lock_guard taskCountLock(taskCountMutex);
        --tasks.taskCount;
        if (!tasks.taskCount) cvWait.notify_one();
        if (stopFlag) break;
    }
}

ThreadPool::ThreadPool(size_t workers_ = std::thread::hardware_concurrency()) 
{
    for (size_t i = 0; i < workers_; ++i)
    {
        workers.emplace_back([this, i](){ workerRoutine(i); });
    }
}

ThreadPool::~ThreadPool()
{
    for (auto& worker : workers) worker.join();
}

void ThreadPool::submit(Task task) { tasks.put(task); }

void ThreadPool::wait()
{
    if (!tasks.taskCount) return;
    std::unique_lock lock(cvMutex);
    cvWait.wait(lock);
}

void ThreadPool::stop()
{
    stopFlag = true;
    tasks.stop();
}

int main()
{
    ThreadPool tp;
    size_t n = 100000;
    std::vector<uint8_t> isExecuted(n, 1); // does not work with 
// std::vector<bool> since it modifies its buffer through a bit reference
    for (int i = 0; i < n; ++i)
    {
        tp.submit([&isExecuted, i]()
        {
            isExecuted[i] = 0;
        });
    }
    tp.wait();
    bool allTasksExecuted = true;
    for (int i = 0; i < n; ++i)
    {
        if (isExecuted[i]) 
        {
            allTasksExecuted = false;
            std::cout << i << ' ';
        }
    }
    if (!allTasksExecuted) std::cout << "not ";
    std::cout << "all tasks executed\n";

    
    for (int i = 0; i < 100; ++i)
    {
        tp.submit([i]()
        {
            if (!(i % 20)) throw std::out_of_range(
                std::to_string(i) + " catch me if you can\n");
        });    
    }
    tp.wait();
    tp.stop();
}
