#include <iostream> // TO DO: remove
#define BUFFER_SIZE 16

/* With what function must be able to be initialized:
 * - C-style function
 * - functional object
 * - lambda-function
 * - pointer to method
 * - pointer to field
 * - std::bind
 */

template <typename>
class function;

template <typename R, typename... Args>
class function<R(Args...)>
{
    using invoke_ptr_t = R(*)(void*, Args...);
    using destroy_ptr_t = void(*)(void*);
    
    char buffer[BUFFER_SIZE]{}; // is alignas required?
    void* functionObjectPtr;
    invoke_ptr_t invoke_ptr;
    destroy_ptr_t destroy_ptr;

    template <typename F>
    static void invoker(F* fPtr, Args... args)
    { // here must be std::invoke
        return (*fPtr)(std::forward<Args>(args)...);
    }

    template <typename F>
    static void destroyer(F* fPtr)
    {
        if constexpr (sizeof(F) <= BUFFER_SIZE)
        {
            fPtr->~F();
        } else {
            delete fPtr;
        }
    }
public:
    template <typename F>
    function(F&& f)
            : functionObjectPtr(nullptr)
            , invoke_ptr(reinterpret_cast<invoke_ptr_t>(&invoker<F>))
            , destroy_ptr(reinterpret_cast<destroy_ptr_t>(&destroyer<F>))
    {
        if constexpr (sizeof(f) <= BUFFER_SIZE) // function object is placed in
                                                // buffer
        {
            new (buffer) F(f);
            functionObjectPtr = reinterpret_cast<F*>(buffer);
        } else { // function object is placed in dynamic memory
            functionObjectPtr = new F(f);
        }
    }

    R operator()(Args... args) const
    {
        return invoke_ptr(functionObjectPtr, std::forward<Args>(args)...);
    }

    ~function()
    {
        destroy_ptr(functionObjectPtr);
    }
};

