#include <iostream>
#include <vector>

std::vector<int> buildPi(const std::string& str)
{
    std::vector<int> pi(str.size());
    for (int i = 1; i < str.size(); ++i)
    {
        int k = pi[i - 1];
        while (k > 0 && str[i] != str[k]) k = pi[k - 1];
        if (str[i] == str[k]) ++k;
        pi[i] = k;
    }
    return pi;
}

std::vector<int> get_pattern(const std::string& text, const std::string& pattern)
{
    std::vector<int> res;
    std::string str = pattern + "#" + text;
    std::vector<int> pi = buildPi(str);
    for (int i = pattern.size() + 1; i < text.size() + pattern.size() + 1; ++i)
    {
        if (pi[i] == pattern.size()) res.push_back(i - 2 * pattern.size());
    }
    return res;
}

int main()
{
    std::string s = "aaaaaaaaaaaaaaaaaaaaaa";
    auto v = get_pattern(s, "aaaa");
    for (auto x : v) std::cout << x << ' ';
}