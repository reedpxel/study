#include <deque>
#include <mutex>
#include <condition_variable>
#include <optional>

namespace exe::runtime::multi_thread
{

// All queue methods are thread-safe
template <typename T>
class UnboundedBlockingMPMCQueue
{
public:
    /*
     * Puts an object in a queue if it is not closed.
     */
    void put(const T& obj)
    {
        std::lock_guard guard(mutex_);
        if (closed_)
        {
            return;
        }
        queue_.push_back(obj);
        queueNotEmpty.notify_one();
    }
    
    /*
     * If queue buffer is not empty, takes a task from its' front. If queue is
     * empty and stopped, returns std::nullopt. Otherwise, blocks the current
     * thread until an object is put.
     */
    std::optional<T> take() noexcept
    {
        std::unique_lock lock_(mutex_);
        while (queue_.empty())
        {
            if (closed_)
            {
                return std::nullopt;
            }
            queueNotEmpty.wait(lock_);
        }
        T res = queue_.front();
        queue_.pop_front();
        return {res};
    }

    /*
     * Closes the queue and notifies all threads that waited for objects.
     */
    void close() noexcept
    {
        std::lock_guard guard_(mutex_);
        closed_ = true;
        queueNotEmpty.notify_all();
    }

private:
    std::deque<T> queue_;
    bool closed_ = false;
    std::mutex mutex_;
    std::condition_variable queueNotEmpty;
};

} // namespace exe::runtime::multithread

