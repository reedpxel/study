#include <iostream>
#include <vector>
#include <memory>
#include <cmath>

// получение n-го числа Фибоначчи за O(log n), log n - т.к. возведение матрицы в степень делается за O(log n), а не за O(n), за счет оптимизации в зависимости
// от являющихся логарифмом 2 или нет промежуточных степеней

typedef std::vector<std::vector<unsigned long long>> m2_t;

m2_t& operator*=(m2_t& m1, const m2_t& m2)
{
    unsigned long long res00 = m1[0][0] * m2[0][0] + m1[0][1] * m2[1][0];
    unsigned long long res01 = m1[0][0] * m2[0][1] + m1[0][1] * m2[1][1];
    unsigned long long res10 = m1[1][0] * m2[0][0] + m1[1][1] * m2[1][0];
    unsigned long long res11 = m1[1][0] * m2[0][1] + m1[1][1] * m2[1][1];
    m1[0][0] = res00;
    m1[0][1] = res01;
    m1[1][0] = res10;
    m1[1][1] = res11;
    return m1;
}

m2_t operator*(const m2_t& m1, const m2_t& m2)
{
    m2_t res = m1;
    return res*= m2;
}

m2_t& operator^=(m2_t& m, unsigned power)
{
    m2_t tmp = m;
    while (power > 1)
    {
        float f = log2(power);
        if (f = round(f)) 
        {
            m *= tmp;
            --power;
        } else {
            m *= m;
            power >>= 1;
        }
    }
    return m;
}

m2_t operator^(const m2_t& m, unsigned power)
{
    m2_t res = m;
    return res ^= power;
}

unsigned long long fibonacci(unsigned n)
{
    if (!n || n == 1) return 1;
    m2_t A = {{1, 1}, {1, 0}};
    A ^= n - 1;
    return A[0][0] + A[0][1];
}

int main()
{
    for (int i = 0; i < 100; ++i) std::cout << fibonacci(i) << ' ';
}

