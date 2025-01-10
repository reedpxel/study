//#ifndef BIGINT_H
//#define BIGINT_H

#include <iostream>
#include <iomanip>
#include <cstring>
#include <cmath>
#define INPUT_BUFFER_SIZE 1000

class BigInt{
    std::string str;

    //inner help functions
    static std::string add(const char* s1, const char* s2);
    static std::string substract(const char* s1, const char* s2);
    static std::string multiply(const char* _s1, const char* _s2);
    static std::string divide_by_two(const std::string& _s);
    static std::string divide(const char* _s1, const char* _s2);

public:

    BigInt();
    BigInt(const std::string& _str);
    BigInt(const char* _ch);
    BigInt(long long _i);

    //comparison operators
    bool operator<(const BigInt& _bi) const;
    bool operator>(const BigInt& _bi) const;
    bool operator==(const BigInt& _bi) const;
    bool operator!=(const BigInt& _bi) const;
    bool operator<=(const BigInt& _bi) const;
    bool operator>=(const BigInt& _bi) const;

    //arithmetic operations
    BigInt& operator+=(const BigInt& _bi);
    BigInt operator+(const BigInt& _bi) const;
    BigInt& operator++();
    BigInt operator++(int);
    BigInt operator+();
    BigInt& operator-=(const BigInt& _bi);
    BigInt operator-(const BigInt& _bi);
    BigInt& operator--();
    BigInt operator--(int);
    BigInt operator-();
    BigInt& operator*=(const BigInt& _bi);
    BigInt operator*(const BigInt& _bi);
    BigInt& operator/=(const BigInt& _bi);
    BigInt operator/(const BigInt& _bi);
    BigInt& operator%=(const BigInt& _bi);
    BigInt operator%(const BigInt& _bi);

    //input and output operators
    friend std::istream& operator>>(std::istream& input, BigInt& _bi);
    friend std::ostream& operator<<(std::ostream& output, const BigInt& _bi);

    // static public functions
    static BigInt abs(const BigInt& bi);
    static BigInt pow(const BigInt& base, const BigInt& power);
};

//#endif // BigInt
