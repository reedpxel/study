#include <iostream>
#include <vector>

// How it works:
// z-function is a function from a string s and index i in interval 
// [0, s.size() - 1] that is equal to the longest prefix of s that is equal to
// the substring that begins at i. 
// The general idea of getting z-function for all the indices is to save borders
// of the latest z-block (in ints l and r).

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
        // If i lies in a found z-block, z[i] can:
        // 1. not go out of the z-block, then z[i] is z[i - l]
        // 2. go out of the z-block, then z[i] is at least r - i + 1, it may be
        // bigger, but this fact is checked with char-by-char comparison.
        // The following line can be substituted with:
        // if (l <= i && i <= r)
        // {
        //     z[i] = std::min(z[i - l], r - i + 1);
        // }
        z[i] = std::max(0, std::min(z[i - l], r - i + 1));
        // In case if i is located out of the found z-block or the longest 
        // substring that is equal to the prefix comes out of the left border 
        // of the z-block, an element-by-element comparison is done:
        while (i + z[i] < sz && s[i + z[i]] == s[z[i]]) ++z[i];
        // Updating the z-block borders:
        if (r < i + z[i])
        {
            l = i;
            r = l + z[i] - 1;
        }
    }
    return z;
}

// How is z-function used to find a substring occurence in a string:
// 1. A string "pattern" + a sign that is not present in the alphabet + "the
// string" is created
// 2. Values of z-function for all its' indices count
// 3. The pattern is present in the string at index i if z[i] == pattern.size()
std::vector<int> pattern_search(const std::string& text, 
    const std::string& pattern)
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
