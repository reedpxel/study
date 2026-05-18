#ifndef FUTURE_H
#define FUTURE_H

#include "promise.hpp"
#include "shared_state.hpp"

// std::future may be created from:
// 1. std::promise::get_future()
// 2. std::async
// 3. std::packaged_task
// This future only supports creation from promises

template <typename T>
class Future
{
    friend class Promise<T>;

    Future(const Promise<T>& promise_)
            : sharedStatePtr(promise_.sharedStatePtr)
    {}

public:
    T get()
    {
        // 0. lock mutex
        std::unique_lock lock_(sharedStatePtr->mutex_);
        // 1. check if result is ready, block if it's not
        if (!sharedStatePtr->resultReady)
        {
            sharedStatePtr->cvResultReady.wait(lock_, [this] 
                { 
                    return sharedStatePtr->resultReady; 
                });
        }
        // 2. If expected contents a value, return it. If it contents an
        // std::exception_ptr, throw exception that it stores
        if (!sharedStatePtr->getResult()->has_value())
        {
            std::rethrow_exception(sharedStatePtr->getResult()->error());
        }
        if constexpr (std::is_move_constructible_v<T>)
        {
            return T{std::move(sharedStatePtr->getResult()->value())};
        }
        else
        {
            return T{sharedStatePtr->getResult()->value()};
        }
    }

    Future(const Future&) = delete;
    Future& operator=(const Future&) = delete;

    Future(Future&&) = default;
    Future& operator=(Future&&) = default;

    ~Future() = default;

private:
    std::shared_ptr<SharedState<T>> sharedStatePtr;
};

#endif // FUTURE_H

