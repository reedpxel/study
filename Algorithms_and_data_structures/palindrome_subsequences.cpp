#include <iostream>
#include <vector>

unsigned dp(std::vector<int>& v, unsigned l, unsigned r)
{
    if (l > r) return 0;
    if (l == r) return 1;
    if (v[l] == v[r]) return dp(v, l + 1, r - 1) + 2;
    return std::max<unsigned>(dp(v, l + 1, r), dp(v, l, r - 1));
}

// находит наибольшую по длине подпоследовательность в v (не подстроку, числа могут стоять не рядом друг с другом)
// сложность - O(n ^ 2)
unsigned get_longest_palindromic_sequence(std::vector<int>& v) { return dp(v, 0, v.size() - 1); }

int main()
{
    std::vector<std::vector<int>> v;
    v.resize(10, std::vector<int>(10, 1));
    for (auto x : v)
    {
        for (auto y : x)
        {
            std::cout << y;
        }
        std::cout << '\n';
    }
}
