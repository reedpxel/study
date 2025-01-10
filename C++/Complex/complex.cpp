#include "complex.h"

Complex Complex::parseStringAsNumber(const char* str)
{
    bool numberIsNegative = false;
    bool firstNumberIsImaginary = false;
    if (*str == '-') // to parse the cases like "-i123"
    {
        numberIsNegative = true;
        ++str;
    }
    if (*str == 'i')
    {
        firstNumberIsImaginary = true;
        ++str;
    }
    char* numberEnd = nullptr;
    char** pNumberEnd = &numberEnd;
    double number1 = strtod(str, pNumberEnd);
    if (numberIsNegative) number1 = -number1;
    if (*numberEnd == 'i' || firstNumberIsImaginary)
    {
        if (str == numberEnd)
        {
            number1 = numberIsNegative ? -1 : 1;
        }
        return Complex(0, number1);
    }
    str = numberEnd;
    numberIsNegative = false;
    bool breakFlag = false;
    bool iIsMet = false;
    for (; str - numberEnd < 3 && !breakFlag; ++str)
    {
        switch (*str)
        {
        case '+':
        case ' ':
        break;
        case '-':
            numberIsNegative = true;
        break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '.':
            --str;
            breakFlag = true;
        break;
        case 'i':
            iIsMet = true;
            breakFlag = true;
        break;
        case '\0':
            return Complex(number1, 0);
        default:
            return Complex();
        }
    }
    if (*str == 'i')
    {
        ++str; // for case "1 + i2"
        iIsMet = true;
    }
    double number2 = strtod(str, pNumberEnd);
    if (*numberEnd == 'i') iIsMet = true;
    if (!number2 && iIsMet) number2 = 1;
    if (!iIsMet) return Complex();
    return Complex(number1, numberIsNegative ? -number2 : number2);
}

Complex::Complex(double re, double im) : re(re), im(im) {}

double Complex::real() const { return re; }

double Complex::imag() const { return im; }

void Complex::setReal(double value) { re = value; }

void Complex::setImaginary(double value) { im = value; }

Complex& Complex::operator=(const Complex& other) &
{
    re = other.re;
    im = other.im;
    return *this;
}

Complex& Complex::operator+=(const Complex& other)
{
    re += other.re;
    im += other.im;
    return *this;
}

Complex& Complex::operator++()
{
    return *this += 1.;
}

Complex Complex::operator++(int)
{
    Complex res = *this;
    *this += 1.;
    return res;
}

Complex Complex::operator+() const
{
    Complex res = *this;
    return res;
}

Complex& Complex::operator-=(const Complex& other)
{
    re -= other.re;
    im -= other.im;
    return *this;
}

Complex& Complex::operator--()
{
    return *this -= 1.;
}

Complex Complex::operator--(int)
{
    Complex res = *this;
    *this -= 1.;
    return res;
}

Complex Complex::operator-() const
{
    Complex res = *this;
    res.re = -res.re;
    res.im = -res.im;
    return res;
}

Complex& Complex::operator*=(const Complex& other)
{
    re = re * other.re - im * other.im;
    im = im * other.re + re * other.im;
    return *this;
}

Complex& Complex::operator/=(const Complex& other)
{
    re = (re * other.re + im * other.im) / 
        (other.re * other.re + other.im * other.im);
    im = (im * other.re - re * other.im) / 
        (other.re * other.re + other.im * other.im);
    return *this;
}

Complex Complex::pow(double power) const
{
    double absNew = ::pow(abs(), power);
    double argNew = arg() * power;
    return Complex(absNew * cos(argNew), absNew * sin(argNew));
}

double Complex::arg() const
{
    return atan(im / re);
}

double Complex::abs() const
{
    return sqrt(re * re + im * im);
}

std::partial_ordering Complex::operator<=>(const Complex& other) const
{
    return this->abs() <=> other.abs();
}

bool Complex::operator==(const Complex& other)
{
    return re == other.re && im == other.im;
}

bool Complex::operator!=(const Complex& other)
{
    return !(*this == other);
}

Complex operator+(const Complex& a, const Complex& b)
{
    Complex res = a;
    res += b;
    return res;
}

Complex operator-(const Complex& a, const Complex& b)
{
    Complex res = a;
    res -= b;
    return res;
}

Complex operator*(const Complex& a, const Complex& b)
{
    Complex res = a;
    res *= b;
    return res;
}

Complex operator/(const Complex& a, const Complex& b)
{
    Complex res = a;
    res /= b;
    return res;
}

Complex operator""_c(const char* str, size_t)
{
    return Complex::parseStringAsNumber(str);
}

std::ostream& operator<<(std::ostream& out, const Complex& complex_)
{
    std::string res;
    if (complex_.re) res += std::to_string(complex_.re);
    if (complex_.im){
        if (complex_.im > 0 && complex_.re) res.push_back('+');
        res += std::to_string(complex_.im);
        res.push_back('i');
    }
    if (res.empty()) res.push_back('0');
    out << res;
    return out;
}
