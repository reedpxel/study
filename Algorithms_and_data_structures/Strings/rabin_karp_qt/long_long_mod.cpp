#include "long_long_mod.h"

Long_long_mod::Long_long_mod(BigInt value) : q((1ll << 61) - 1), value(value % q) {}

Long_long_mod::Long_long_mod(long long value) : q((1ll << 61) - 1), value(BigInt(value) % q) {}

// arithmetic operators
Long_long_mod& Long_long_mod::operator+=(const Long_long_mod& llm)
{
    this->value += llm.value;
    this->value %= q;
    return *this;
}

Long_long_mod Long_long_mod::operator+(const Long_long_mod& llm)
{
    Long_long_mod res = this->value + llm.value;
    if (res.value > q) res.value -= q;
    return res;
}

Long_long_mod& Long_long_mod::operator++()
{
    ++this->value;
    if (this->value == q) this->value = "0";
    return *this;
}

Long_long_mod Long_long_mod::operator++(int)
{
    Long_long_mod res = *this;
    ++this->value;
    if (this->value == q) this->value = "0";
    return res;
}

Long_long_mod Long_long_mod::operator+()
{
    Long_long_mod res = *this;
    return res;
}

Long_long_mod& Long_long_mod::operator-=(const Long_long_mod& llm)
{
    this->value -= llm.value;
    if (this->value < "0") this->value = q + this->value;
    return *this;
}

Long_long_mod Long_long_mod::operator-(const Long_long_mod& llm)
{
    Long_long_mod res = this->value - llm.value;
    if (res.value < "0") res.value = q + res.value;
    return res;
}

Long_long_mod& Long_long_mod::operator--()
{
    --this->value;
    if (this->value < "0") this->value = q - 1;
    return *this;
}

Long_long_mod Long_long_mod::operator--(int)
{
    Long_long_mod res = *this;
    --this->value;
    if (this->value < "0") this->value = q - 1;
    return res;
}

Long_long_mod Long_long_mod::operator-()
{
    Long_long_mod res = *this;
    return res;
}

Long_long_mod& Long_long_mod::operator*=(const Long_long_mod& llm)
{
    this->value *= llm.value;
    this->value %= q;
    return *this;
}

Long_long_mod Long_long_mod::operator*(const Long_long_mod& llm)
{
    Long_long_mod res = this->value * llm.value;
    res.value %= q;
    return res;
}

Long_long_mod Long_long_mod::pow(const Long_long_mod &base, const Long_long_mod &power)
{
    if (power.value < "0") return Long_long_mod("0");
    if (power.value == "0") return Long_long_mod("1");
    Long_long_mod res = base;
    for (BigInt i = "0"; i < power.value; ++i)
    {
        res *= base;
    }
    res.value %= res.q;
    return res;
}

bool Long_long_mod::operator==(const Long_long_mod &llm) { return this->value == llm.value; }

//output
std::ostream& operator<<(std::ostream& output, const Long_long_mod& _llm)
{
    output << _llm.value;
    return output;
}

