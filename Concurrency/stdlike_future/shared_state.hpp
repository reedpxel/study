#ifndef SHARED_STATE_H
#define SHARED_STATE_H

#include <mutex>
#include <condition_variable>
#include <expected>
#include <exception>
#include <cassert>

template <typename T>
struct SharedState
{
    SharedState() = default;

    ~SharedState()
    {
        if (resultReady)
        {
            using namespace std;
            getResult()->~expected<T, std::exception_ptr>();
        }
    }

    std::expected<T, std::exception_ptr>* getResult()
    {
        return reinterpret_cast<std::expected<T, std::exception_ptr>*>(this);
    }    

public:
    char resultBuffer[sizeof(std::expected<T, std::exception_ptr>)]{};
    bool resultReady = false;
    std::mutex mutex_;
    std::condition_variable cvResultReady;
};

#endif // SHARED_STATE_H

