#include <iostream>
#include <vector>

std::vector<int> buildZ(const std::string& s)
{
    if (s.empty()) return {};
    int sz = s.size();
    std::vector<int> z(sz);
    z[0] = sz;
    int l = -1;
    int r = -1;
    for (int i = 1; i < sz; ++i)
    {
        z[i] = std::max(0, std::min(z[i - l], r - i + 1));
        while (i + z[i] < sz && s[i + z[i]] == s[z[i]]) ++z[i];
        if (r < i + z[i])
        {
            l = i;
            r = l + z[i] - 1;
        }
    }
    return z;
}

std::vector<int> pattern_search(const std::string& text, const std::string& pattern)
{
    std::string sum = pattern + "#" + text;
    std::vector<int> z = buildZ(sum);
    std::vector<int> res;
    for (int i = pattern.size() + 1; i < sum.size(); ++i)
    {
        if (z[i] == pattern.size()) res.push_back(i - pattern.size() - 1);
    }
    return res;
}

int main()
{
    std::string text = "aaaaabaaaaaaaaaaaaaaaaa";
    std::string pattern = "aaa";
    std::vector<int> found_substrings = pattern_search(text, pattern);
    for (auto x : found_substrings) std::cout << x << ' ';
}
