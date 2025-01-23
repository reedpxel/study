#include <cstdint>
#include <string>

template <typename Key>
struct HashFunctor;

template <>
struct HashFunctor<int64_t>
{
    int64_t operator()(int64_t value) { return value; }
};

template <>
struct HashFunctor<std::string>
{
    uint64_t operator()(const std::string& value)
    {
        uint64_t res = 0;
        for (uint8_t& ch : value) res += ch;
        return res;
    }
};
