#include <iostream>
#include "long_long_mod.h"
#include <vector>

Long_long_mod get_hash_substring(const std::string& string,
                                 size_t left,
                                 size_t right)
{
    Long_long_mod res = 0;
    Long_long_mod current_factor = 2;
    for (size_t i = left; i <= right; ++i)
    {
//        std::cout << static_cast<Long_long_mod>(string[i]) << ' ' << current_factor << '\n';
        res += static_cast<Long_long_mod>(string[i]) * current_factor;
        current_factor *= 2;
    }
    return res;
}

bool compare_naive(const std::string& text,
                   size_t index,
                   const std::string& pattern)
{
    for (size_t i = index; i < index + pattern.size(); ++i)
    {
        if (text[i] != pattern[i - index]) return false;
    }
    return true;
}

std::vector<size_t> find_pattern_rabin_karp(const std::string& text, const std::string& pattern)
{
    if (text.empty() || pattern.empty()) return {};
    std::vector<size_t> res;
    Long_long_mod pattern_hash = get_hash_substring(pattern, 0, pattern.size() - 1);
    for (size_t i = 0; i < text.size() - pattern.size() + 1; ++i)
    {
//        std::cout << i << ' ';
//        for (int j = i; j < i + pattern.size(); ++j) std::cout << text[j];
//        std::cout << ' ' << pattern_hash << ' ' << get_hash_substring(text, i, i + pattern.size() - 1) << '\n';
        if (get_hash_substring(text, i, i + pattern.size() - 1) == pattern_hash
                && compare_naive(text, i, pattern)) res.push_back(i);
    }
    return res;
}

int main()
{
    std::string text = "This is a list of opeC++rators in the C and C++ programming languages. All the operators (except typeof) listed exist in C++; the column"
                       ", states whether an operator is also present in C. Note that C++ does not support operator overloading. Most of the operators available "
                       "in C and C++ are also available in other C-family languages sC++uch as C#, D, Java, Perl, and PHP with the same precedence, associativity, "
                       "and semantics. ";
    std::string pattern = "C++";
    std::vector<size_t> res = find_pattern_rabin_karp(text, pattern);
    for (auto x : res)
    {
        std::cout << x << ' ';
        for (int i = 0; i < pattern.size(); ++i) std::cout << text[x + i];
        std::cout << '\n';
    };
}
