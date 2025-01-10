#ifndef LONG_LONG_MOD_H
#define LONG_LONG_MOD_H
#include "bigint.h"

class Long_long_mod
{
    BigInt q; /*(1ll << 61) - 1;*/
    BigInt value;
public:
    Long_long_mod(BigInt value);
    Long_long_mod(long long value);

    // arithmetic operators
    Long_long_mod& operator+=(const Long_long_mod& llm);
    Long_long_mod operator+(const Long_long_mod& llm);
    Long_long_mod& operator++();
    Long_long_mod operator++(int);
    Long_long_mod operator+();

    Long_long_mod& operator-=(const Long_long_mod& llm);
    Long_long_mod operator-(const Long_long_mod& llm);
    Long_long_mod& operator--();
    Long_long_mod operator--(int);
    Long_long_mod operator-();

    Long_long_mod& operator*=(const Long_long_mod& llm);
    Long_long_mod operator*(const Long_long_mod& llm);

    static Long_long_mod pow(const Long_long_mod& base, const Long_long_mod& power);

    // logic operator
    bool operator==(const Long_long_mod& llm);

    // output
    friend std::ostream& operator<<(std::ostream& output, const Long_long_mod& _llm);
};

#endif // LONG_LONG_MOD_H
