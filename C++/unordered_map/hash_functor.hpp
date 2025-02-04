#include <cstdint>
#include <string>

template <typename Key>
struct HashFunctor;

template <>
struct HashFunctor<int64_t>
{
    uint64_t operator()(int64_t value) const noexcept { return value; }
};

template <>
struct HashFunctor<uint64_t>
{
    uint64_t operator()(uint64_t value) const noexcept { return value; }
};

template <>
struct HashFunctor<int32_t>
{
    uint32_t operator()(int32_t value) const noexcept { return value; }
};

template <>
struct HashFunctor<uint32_t>
{
    uint32_t operator()(uint32_t value) const noexcept { return value; }
};

template <>
struct HashFunctor<int16_t>
{
    uint16_t operator()(int16_t value) const noexcept { return value; }  
};

template <>
struct HashFunctor<uint16_t>
{
    uint16_t operator()(uint16_t value) const noexcept { return value; }
};

template <>
struct HashFunctor<int8_t>
{
    uint8_t operator()(int8_t value) const noexcept { return value; }
};

template <>
struct HashFunctor<uint8_t>
{
    uint8_t operator()(uint8_t value) const noexcept { return value; }
};

template <>
struct HashFunctor<float>
{
    uint64_t operator()(float value) const noexcept
    { 
        return static_cast<uint64_t>(value); 
    }
};

template <>
struct HashFunctor<double>
{
    uint64_t operator()(double value) const noexcept 
    { 
        return static_cast<uint64_t>(value); 
    }
};

template <>
struct HashFunctor<long double>
{
    uint64_t operator()(long double value) const noexcept 
    { 
        return static_cast<uint64_t>(value); 
    }
};

template <>
struct HashFunctor<std::string>
{
    uint64_t operator()(const std::string& value) const noexcept
    {
        uint64_t res = 0;
        for (uint8_t ch : value) res += ch;
        return res;
    }
};
