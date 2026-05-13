#include <mutex>
#include <condition_variable>

class WaitGroup
{
public:
    void add(size_t addToCount = 1);
    void done();
    void wait();

private:
    size_t count = 0;
    std::condition_variable countIsZero;
    std::mutex mutex_;
};

