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

    ~Promise() = default;

    // TODO: copy and move ctors and operator=

    void setValue(const T& value) // TODO: needed T&& ? yes, needed, implement
    {
        // 0. lock mutex
        std::lock_guard guard_(sharedStatePtr->mutex_);
        // 1. put object
        new (sharedStatePtr.get()) std::expected<T, std::exception_ptr>(value);
        // 2. set resultReady to true
        sharedStatePtr->resultReady = true;
        // Один поток может делать запись в resultReady в этом методе, другой 
        // может читать его в future.get(). Тут нужен мьютекс. Не атомик, потому
        // что весь метод целиком должен быть атомарным. 
        // 3. notify one
        sharedStatePtr->cvResultReady.notify_one();
    }
    void setException(std::exception_ptr eptr)
    {
        std::lock_guard guard_(sharedStatePtr->mutex_);
        new (sharedStatePtr.get()) std::expected<T, std::exception_ptr>(std::unexpect, eptr);
        sharedStatePtr->resultReady = true;
        sharedStatePtr->cvResultReady.notify_one();
    }

    Future<T> getFuture()
    {
        return Future<T>{*this};
    }

public: // TODO: make private
    std::shared_ptr<SharedState<T>> sharedStatePtr;
};

#endif // PROMISE_H
 
