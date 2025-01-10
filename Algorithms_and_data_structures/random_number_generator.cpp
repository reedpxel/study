#include <iostream>
#include <cstdint>

uint64_t randomValue = 0;
uint64_t a = 84589;
uint64_t c = 45989;
uint64_t m = 217728;

uint64_t get_random_number()
{
    randomValue = (a * randomValue + c) % m;
    return randomValue;
}

int main()
{
    for (int i = 0; i < 1000; ++i) std::cout << get_random_number() << ' ';
}

