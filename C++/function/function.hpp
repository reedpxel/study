#include <iostream> // TO DO: remove
#include <type_traits>
#include <cstdint>
#include <memory>
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

#ifndef IS_NULLPTR
#define IS_NULLPTR
template <typename T>
bool isNullptr(T* ptr) 
{
#ifdef __SANITIZE_ADDRESS__
    return ptr == nullptr || ptr == (T*)0xbebebebebebebebe;
#else 
    return ptr == nullptr;
#endif
}
#endif

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
    using invoke_ptr_t = R(*)(void*, Args...);
    using destroy_ptr_t = void(*)(void*);
    using copy_ptr_t = void*(*)(void*, char*);
    using move_ptr_t = copy_ptr_t;

    struct ControlBlock
    {
        destroy_ptr_t destroy_ptr;
        copy_ptr_t copy_ptr;
        move_ptr_t move_ptr;
        size_t x1 = 0;
        size_t x2 = 0;
    };

    friend class ControlBlock;

public:
    using result_type = R;
    
    function() noexcept
            : invoke_ptr(nullptr)
            , ctrlBlockPtr()
    {}

    function(std::nullptr_t) noexcept : function() {}

    function(const function& other)
            : invoke_ptr(other.invoke_ptr)
            , ctrlBlockPtr(other.ctrlBlockPtr)
    {
        functionObjectPtr = ctrlBlockPtr ? 
            ctrlBlockPtr->copy_ptr(other.functionObjectPtr, buffer) :
            nullptr;
    }

    function(function&& other) noexcept
            : invoke_ptr(other.invoke_ptr)
            , ctrlBlockPtr(std::move(other.ctrlBlockPtr))
    {
        functionObjectPtr = ctrlBlockPtr ? 
            ctrlBlockPtr->move_ptr(other.functionObjectPtr,buffer) :
            nullptr;
        other.functionObjectPtr = nullptr;
    }

    template <typename F, std::enable_if_t<!std::is_same_v<
        F, function<void()>&>, bool> = true>
    function(F&& f)
            : invoke_ptr(reinterpret_cast<invoke_ptr_t>(&invokeHelper<
                std::decay_t<F>>))
            , ctrlBlockPtr(new ControlBlock{
                reinterpret_cast<destroy_ptr_t>(&destroyHelper<std::decay_t<
                F>>), reinterpret_cast<copy_ptr_t>(&copyHelper<std::decay_t<
                F>>), reinterpret_cast<move_ptr_t>(&moveHelper<std::decay_t<
                F>>)})
    {
        ctorHelper(std::forward<F>(f));
    }

    ~function()
    {
        if (!isNullptr(functionObjectPtr)) 
        {
            ctrlBlockPtr->destroy_ptr(functionObjectPtr);
        }
    }

    function& operator=(const function& other)
    {
        if (ctrlBlockPtr)
        {
            ctrlBlockPtr->destroy_ptr(functionObjectPtr);
        }
        invoke_ptr = other.invoke_ptr;
        ctrlBlockPtr = other.ctrlBlockPtr;
        functionObjectPtr = ctrlBlockPtr ? 
            ctrlBlockPtr->copy_ptr(other.functionObjectPtr, buffer) : 
            nullptr;
        return *this;
    }

    function& operator=(function&& other)
    {
        if (ctrlBlockPtr)
        {
            ctrlBlockPtr->destroy_ptr(functionObjectPtr);
        }
        invoke_ptr = other.invoke_ptr;
        ctrlBlockPtr = std::move(other.ctrlBlockPtr);
        functionObjectPtr = ctrlBlockPtr ? 
            ctrlBlockPtr->copy_ptr(other.functionObjectPtr, buffer) : 
            nullptr;
        other.functionObjectPtr = nullptr;
        return *this;
    }

    function& operator=(std::nullptr_t) noexcept
    {
        if (ctrlBlockPtr) 
        {
            ctrlBlockPtr->destroy_ptr(functionObjectPtr);
        }
        functionObjectPtr = nullptr;
        return *this;
    }

    template <typename F, std::enable_if_t<!std::is_same_v<
        F, function<void()>&>, bool> = true>
    function& operator=(F&& f)
    {
        if (ctrlBlockPtr)
        {
            ctrlBlockPtr->destroy_ptr(functionObjectPtr);
        }
        invoke_ptr = reinterpret_cast<invoke_ptr_t>(&invokeHelper<
            std::decay_t<F>>);
        ctrlBlockPtr = std::shared_ptr<ControlBlock>(new ControlBlock{
            reinterpret_cast<destroy_ptr_t>(&destroyHelper<std::decay_t<F>>), 
            reinterpret_cast<copy_ptr_t>(&copyHelper<std::decay_t<F>>), 
            reinterpret_cast<move_ptr_t>(&moveHelper<std::decay_t<F>>)});
        ctorHelper(std::forward<F>(f)); 
        return *this;
    }

    template <typename F>
    function& operator=(std::reference_wrapper<F> f) noexcept; // TO DO:
                                                               // implement
    void swap(function& other) // does not work
    {
        std::swap(buffer, other.buffer);
        std::swap(functionObjectPtr, other.functionObjectPtr);
        std::swap(invoke_ptr, other.invoke_ptr);
        std::swap(ctrlBlockPtr, other.ctrlBlockPtr);
    }

    explicit operator bool() const noexcept 
    { 
        return !isNullptr(functionObjectPtr);
    }
    
    R operator()(Args... args) const
    {
        if (!functionObjectPtr) [[unlikely]] 
        {
            throw bad_function_call{};
        }
        return invoke_ptr(functionObjectPtr, std::forward<Args>(args)...);
    }

    const std::type_info& target_type() const noexcept
    {
        if (isNullptr(functionObjectPtr)) return typeid(void);
        return typeid(R(Args...));
    }

    template <typename T>
    T* target() noexcept
    {
        return reinterpret_cast<T*>(functionObjectPtr);
    }

    template <typename T>
    const T* target() const noexcept
    {
        return reinterpret_cast<const T*>(functionObjectPtr);
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
        std::cout << "ctrlBlockPtr: " << ctrlBlockPtr << std::endl;
    }

private:
    template <typename F>
    void ctorHelper(F&& f)
    {
        if constexpr (sizeof(std::decay_t<F>) <= BUFFER_SIZE) 
        { 
            // function object is placed in buffer
            new (buffer) std::decay_t<F>(std::forward<std::decay_t<F>>(f));
            functionObjectPtr = reinterpret_cast<void*>(buffer);
        } else { 
            // function object is placed in dynamic memory
            functionObjectPtr = new std::decay_t<F>(
                std::forward<std::decay_t<F>>(f));
        }
    }

    template <typename F>
    static R invokeHelper(F* fPtr, Args... args)
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
    static void* moveHelper(F* otherFunctionObject, char* bufferPtr) noexcept
    {
        if constexpr (sizeof(F) <= BUFFER_SIZE)
        {
            new (bufferPtr) F(std::move(*otherFunctionObject));
            return reinterpret_cast<F*>(bufferPtr);
        }
        return otherFunctionObject;
    }

private:
    char buffer[BUFFER_SIZE]{}; // is alignas required?
    void* functionObjectPtr = nullptr;
    invoke_ptr_t invoke_ptr;
    std::shared_ptr<ControlBlock> ctrlBlockPtr;
};

template <typename R, typename... ArgTypes>
bool operator==(const function<R(ArgTypes...)>& f, std::nullptr_t) noexcept
{
    return f;
}

