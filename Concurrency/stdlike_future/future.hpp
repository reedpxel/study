// TODO: добавить namespace stdlike
// TODO: потестить move-only, copy-only объекты
//
#ifndef FUTURE_H
#define FUTURE_H

#include "promise.hpp"
#include "shared_state.hpp"

template <typename T>
class Future
{
    friend class Promise<T>;

    Future(Promise<T>& promise_) // TODO: mb const Promise& ?
            : sharedStatePtr(promise_.sharedStatePtr)
    {}
public:
    T get()
    {
//        std::this_thread::sleep_for(10s); // TODO: remove
       // 0. залочить мьютекс
        std::unique_lock lock_(sharedStatePtr->mutex_);
       // 1. проверить, есть ли значение
       //   Если нет, заблокироваться на cvResultReady.wait([] resultReady =
       //   true; );
        if (!sharedStatePtr->resultReady)
        {
            sharedStatePtr->cvResultReady.wait(lock_, [this] 
                { 
                    return sharedStatePtr->resultReady; 
                });
        }
       // 2. Если в expected лежит значение, вернуть его. Если лежит
       // std::exception_ptr, кинуть исключение из него
        if (!sharedStatePtr->getResult()->has_value())
        {
            std::rethrow_exception(sharedStatePtr->getResult()->error());
        }
        return T{std::move(sharedStatePtr->getResult()->value())};
    }

    ~Future() = default;

public: // TODO: make private
    std::shared_ptr<SharedState<T>> sharedStatePtr;
};

#endif // FUTURE_H

