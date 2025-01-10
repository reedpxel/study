#ifndef COMPLEX_H
#define COMPLEX_H
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <compare>

class Complex
{
    double re;
    double im;

    static Complex parseStringAsNumber(const char* str);
    friend Complex operator""_c(const char* str, size_t);
    friend std::ostream& operator<<(std::ostream& out, const Complex& complex_);
public:
    Complex(double re = 0., double im = 0.);
    
    double real() const;
    double imag() const;
    void setReal(double value);
    void setImaginary(double value);

    Complex& operator=(const Complex& other) &;

    Complex& operator+=(const Complex& other);
    Complex& operator++();
    Complex operator++(int);
    Complex operator+() const;

    Complex& operator-=(const Complex& other);
    Complex& operator--();
    Complex operator--(int);
    Complex operator-() const;

    Complex& operator*=(const Complex& other);
    
    Complex& operator/=(const Complex& other);

    Complex pow(double power) const;

    double arg() const;
    double abs() const;

    std::partial_ordering operator<=>(const Complex& other) const;
    // returning type is partial_ordering, because floating-point variables may
    // contain NaNs that can not be compared
    bool operator==(const Complex& other);
    bool operator!=(const Complex& other);
};

Complex operator+(const Complex& a, const Complex& b);
Complex operator-(const Complex& a, const Complex& b);
Complex operator*(const Complex& a, const Complex& b);
Complex operator/(const Complex& a, const Complex& b);

Complex operator""_c(const char* str, size_t);
std::ostream& operator<<(std::ostream& out, const Complex& complex_);

#endif // COMPLEX_H
