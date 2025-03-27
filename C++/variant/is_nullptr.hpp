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

#endif // IS_NULLPTR

