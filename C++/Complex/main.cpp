#include <iostream>
#include "complex.h"
#include <vector>

// Literal suffixes test:

// int main()
// {
//     std::vector<Complex> testLiterals = 
//     {
//         "10.1+2.1i"_c,
//         "-10.1+2.1i"_c,
//         "10.1-2.1i"_c,
//         "-10.1-2.1i"_c,

//         "10.1+i2.1"_c,
//         "-10.1+i2.1"_c,
//         "10.1-i2.1"_c,
//         "-10.1-i2.1"_c,

//         "10.1 + 2.1i"_c,
//         "-10.1 + 2.1i"_c,
//         "10.1 - 2.1i"_c,
//         "-10.1 - 2.1i"_c,

//         "10.1 + i2.1"_c,
//         "-10.1 + i2.1"_c,
//         "10.1 - i2.1"_c,
//         "-10.1 - i2.1"_c,

//         "0.012 + i"_c,
//         "0.012 - i"_c,
//         "0.012+i"_c,
//         "0.012-i"_c,

//         "i"_c,
//         "-i"_c,

//         "1"_c,
//         "0"_c,
//         "-1"_c,
//     };
//     for (Complex& complex_ : testLiterals)
//     {
//         std::cout << complex_.real() << ' ' << complex_.imag() << '\n';
//     }
// }

int main()
{
    Complex c1 = "3 - 4i"_c;
    Complex c2 = "3 - i4"_c;
    Complex c3 = "i"_c;
    Complex c4 = "-4 - 3i"_c;
    std::cout << (c1 < c2) << ' ' << (c1 > c2) << ' ' << (c1 == c2) << ' ' << 
        (c1 != c2) << '\n'; // 0 0 1 0
    std::cout << (c1 < c3) << ' ' << (c1 > c3) << ' ' << (c1 == c3) << ' ' << 
        (c1 != c3) << '\n'; // 0 1 0 1
    std::cout << (c1 < c4) << ' ' << (c1 > c4) << ' ' << (c1 == c4) << ' ' << 
        (c1 != c4) << '\n'; // 0 0 0 1
}
