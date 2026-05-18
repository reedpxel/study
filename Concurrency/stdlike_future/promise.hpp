#ifndef PROMISE_H
#define PROMISE_H

#include "future.hpp"
#include "shared_state.hpp"

template <typename T>
class Future;

template <typename T>
class Promise
{
    friend class Future<T>;

public:
    Promise()
            : sharedStatePtr(std::make_shared<SharedState<T>>())
    {}

    Promise(const Promise&) = delete;
    Promise& operator=(const Promise&) = delete;
    
    Promise(Promise&&) = default;
    Promise& operator=(Promise&&) = default;

    ~Promise() = default;
    
    void setValue(const T& value)
    {
        // 0. lock mutex
        std::lock_guard guard_(sharedStatePtr->mutex_);
        // 1. put object
        new (sharedStatePtr.get()) std::expected<T, std::exception_ptr>(value);
        // 2. set resultReady to true
        sharedStatePtr->resultReady = true;
        // One thread may write in resultReady in this method, another may read
        // from resultReady in future.get(), a mutex required
        // 3. notify one
        sharedStatePtr->cvResultReady.notify_one();
    }

    void setValue(T&& value)
    {
        std::lock_guard guard_(sharedStatePtr->mutex_);
        new (sharedStatePtr.get()) std::expected<T, std::exception_ptr>(
            std::move(value));
        sharedStatePtr->resultReady = true;
        sharedStatePtr->cvResultReady.notify_one();
    }

    void setException(std::exception_ptr eptr)
    {
        std::lock_guard guard_(sharedStatePtr->mutex_);
        new (sharedStatePtr.get()) std::expected<T, std::exception_ptr>(
            std::unexpect, eptr);
        sharedStatePtr->resultReady = true;
        sharedStatePtr->cvResultReady.notify_one();
    }

    Future<T> getFuture()
    {
        return Future<T>{*this};
    }

private:
    std::shared_ptr<SharedState<T>> sharedStatePtr;
};

#endif // PROMISE_H
 
