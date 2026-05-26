#include <mutex>
#include <condition_variable>

class WaitGroup
{
public:
    WaitGroup(size_t count = 0);
    void add(size_t addToCount = 1);
    void done();
    void wait();

private:
    size_t count_;
    std::condition_variable countIsZero;
    std::mutex mutex_;
};

