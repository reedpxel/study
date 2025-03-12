#include <iostream> // TO DO: remove
#include <type_traits>
#include <cstdint>
#include <utility>
#define BUFFER_SIZE 16

/* With what function must be able to be initialized:
 * - C-style function
 * - functional object
 * - lambda-function
 * - pointer to method
 * - pointer to field
 * - std::bind
 */

struct Debug
{
    template <typename T>
    Debug(T) = delete;
};

struct bad_function_call : std::exception {};

template <typename>
class function;

template <typename R, typename... Args>
class function<R(Args...)>
{
public: // TO DO: remove
    using invoke_ptr_t = R(*)(void*, Args...);
    using destroy_ptr_t = void(*)(void*);
    using copy_ptr_t = void*(*)(void*, char*);
    using move_ptr_t = copy_ptr_t;

    char buffer[BUFFER_SIZE]{}; // is alignas required?
    void* functionObjectPtr = nullptr;
    invoke_ptr_t invoke_ptr;
    destroy_ptr_t destroy_ptr; // TO DO: lay pointers in ControlBlock
    copy_ptr_t copy_ptr;
    move_ptr_t move_ptr;

    template <typename F>
    static void invokeHelper(F* fPtr, Args... args)
    { // here must be std::invoke
        return (*fPtr)(std::forward<Args>(args)...);
    }

    template <typename F>
    static void destroyHelper(F* fPtr)
    {
        if constexpr (sizeof(std::decay_t<F>) <= BUFFER_SIZE) 
        {
            fPtr->~F();
        } else {
            delete fPtr;
        }
    }

    template <typename F>
    static void* copyHelper(F* otherFunctionObject, char* bufferPtr)
    {
        if constexpr (sizeof(F) <= BUFFER_SIZE)
        {
            new (bufferPtr) F(*otherFunctionObject);
            return reinterpret_cast<F*>(bufferPtr);
        }
        return new F(*otherFunctionObject);
    }

    template <typename F>
    static void* moveHelper(F* otherFunctionObject, char* bufferPtr)
    {
        if constexpr (sizeof(F) <= BUFFER_SIZE)
        {
            new (bufferPtr) F(std::move(*otherFunctionObject));
            return reinterpret_cast<F*>(bufferPtr);
        }
        return new F(std::move(*otherFunctionObject));
    }

public:
    using result_type = R;
    
    function() noexcept
            : invoke_ptr(reinterpret_cast<invoke_ptr_t>(nullptr))
            , destroy_ptr(reinterpret_cast<destroy_ptr_t>(nullptr))
            , copy_ptr(reinterpret_cast<copy_ptr_t>(nullptr))
            , move_ptr(reinterpret_cast<move_ptr_t>(nullptr))
    {}

    function(std::nullptr_t) noexcept : function() {}

    function(const function& other)
        : invoke_ptr(other.invoke_ptr)
        , destroy_ptr(other.destroy_ptr)
        , copy_ptr(other.copy_ptr)
        , move_ptr(other.move_ptr)
    {
        functionObjectPtr = copy_ptr(other.functionObjectPtr, buffer);
    }

    function(function&& other) noexcept // TO DO: why is it noexcept, if it
                                        // may call new?
        : invoke_ptr(other.invoke_ptr)
        , destroy_ptr(other.destroy_ptr)
        , copy_ptr(other.copy_ptr)
        , move_ptr(other.move_ptr)
    {
        functionObjectPtr = move_ptr(other.functionObjectPtr, buffer);
        other.functionObjectPtr = nullptr;
    }

    template <typename F, std::enable_if_t<!std::is_same_v<
        F, function<void()>&>, bool> = true>
    function(F&& f)
            : invoke_ptr(reinterpret_cast<invoke_ptr_t>(&invokeHelper<
                std::decay_t<F>>))
            , destroy_ptr(reinterpret_cast<destroy_ptr_t>(&destroyHelper<
                std::decay_t<F>>))
            , copy_ptr(reinterpret_cast<copy_ptr_t>(&copyHelper<
                std::decay_t<F>>))
            , move_ptr(reinterpret_cast<move_ptr_t>(&moveHelper<
                std::decay_t<F>>))
    {
        if constexpr (sizeof(std::decay_t<F>) <= BUFFER_SIZE) 
        { 
            // function object is placed in buffer
            new (buffer) std::decay_t<F>(f);
            functionObjectPtr = reinterpret_cast<void*>(buffer);
        } else { // function object is placed in dynamic memory
            functionObjectPtr = new std::decay_t<F>(f);
        }
    }

    R operator()(Args... args) const
    {
        if (!functionObjectPtr) [[unlikely]] 
        {
            throw bad_function_call("The function is empty\n");
        }
        return invoke_ptr(functionObjectPtr, std::forward<Args>(args)...);
    }

    ~function()
    {
        destroy_ptr(functionObjectPtr);
    }

    void print() const noexcept // TO DO: remove
    {
        std::cout << "buffer: ptr: " << reinterpret_cast<const void*>(buffer) 
            << ' ' << " content: ";
        for (size_t i = 0; i < BUFFER_SIZE; ++i)
        {
            std::cout << std::hex << (static_cast<uint16_t>(buffer[i]) & 0xff)
                << ' ';
        }
        std::cout << std::dec << "\nfunctionObjectPtr: " << functionObjectPtr 
            << '\n';
        std::cout << "invoke_ptr: " << reinterpret_cast<void*>(invoke_ptr) << 
            std::endl;
        std::cout << "destroy_ptr: " << reinterpret_cast<void*>(destroy_ptr) <<
            std::endl;
        std::cout << "copy_ptr: " << reinterpret_cast<void*>(copy_ptr) <<
            std::endl;
        std::cout << "move_ptr: " << reinterpret_cast<void*>(move_ptr) <<
            std::endl;
    }
};

