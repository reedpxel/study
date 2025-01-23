#include <cstdint>
#include <string>

template <typename Key>
struct HashFunctor;

template <>
struct HashFunctor<int64_t>
{
    uint64_t operator()(int64_t value) { return value; }
};

template <>
struct HashFunctor<uint64_t>
{
    uint64_t operator()(uint64_t value) { return value; }
};

template <>
struct HashFunctor<int32_t>
{
    uint32_t operator()(int32_t value) { return value; }
};

template <>
struct HashFunctor<uint32_t>
{
    uint32_t operator()(uint32_t value) { return value; }
};

template <>
struct HashFunctor<int16_t>
{
    uint16_t operator()(int16_t value) { return value; }  
};

template <>
struct HashFunctor<uint16_t>
{
    uint16_t operator()(uint16_t value) { return value; }
};

template <>
struct HashFunctor<int8_t>
{
    uint8_t operator()(int8_t value) { return value; }
};

template <>
struct HashFunctor<uint8_t>
{
    uint8_t operator()(uint8_t value) { return value; }
};

template <>
struct HashFunctor<std::string>
{
    uint64_t operator()(const std::string& value)
    {
        uint64_t res = 0;
        for (uint8_t ch : value) res += ch;
        return res;
    }
};
